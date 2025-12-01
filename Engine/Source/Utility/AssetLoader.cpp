#include "AssetLoader.hpp"

#include <Core/Containers/UnorderedMap.hpp>
#include <Core/Platform/Base/IO.hpp>

#include "Application/Application.hpp"
#include "Renderer/Assets/Image.hpp"
#include "Renderer/Managers/MeshManager.hpp"

//#define DEBUG_MESH
//#define DEBUG_INDICES

#define SIGN(p1, p2, p3) (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x)* (p1.y - p3.y)

#define FourCC(C1, C2, C3, C4) \
    (static_cast<uint32>(C4) << 24) | (static_cast<uint32>(C3) << 16) | (static_cast<uint32>(C2) << 8) | (static_cast<uint32>(C1))

namespace ME::Utility
{
	constexpr const char8* DefaultMeshName = TEXT("unnamed");
	namespace LocalFunctions
	{
		void Split(const ME::Core::String& str,
			ME::Core::Array<ME::Core::String>& out,
			ME::Core::String splitStr)
		{
			out.Clear();

			ME::Core::String temp;

			for (int32 i = 0; i < static_cast<int32>(str.Size()); i++)
			{
				ME::Core::String test = str.Substring(i, splitStr.Size());

				if (test == splitStr)
				{
					out.EmplaceBack(temp);
					temp.Clear();
					i += static_cast<int32>(splitStr.Size()) - 1;
				}
				else
					temp += str[i];
			}
			if (!temp.Empty())
				out.EmplaceBack(temp);
		}

		void TrimLineEnding(ME::Core::String& in)
		{
			if (!in.Empty() && (in[in.Size() - 1] == TEXT('\r') || in[in.Size() - 1] == '\n'))
				in.PopBack();
			if (!in.Empty() && (in[in.Size() - 1] == TEXT('\r') || in[in.Size() - 1] == '\n'))
				in.PopBack();
		}

		ME::Core::String GetTokenValues(ME::Core::String& in)
		{
			if (in.Empty())
				return TEXT("");

			TrimLineEnding(in);

			SIZE_T tokStart = in.FindFirstNot(TEXT(" \t"));
			SIZE_T spaceStart = in.FindFirst(TEXT(" \t"), tokStart);
			SIZE_T valStart = in.FindFirstNot(TEXT(" \t"), spaceStart);
			SIZE_T valEnd = in.FindLastNot(TEXT(" \t"));

			if (valStart != -1 && valEnd != -1)
				return in.Substring(valStart, valEnd - valStart + 1);
			else if (valStart != -1)
				return in.Substring(valStart);
			return TEXT("");
		}

		inline ME::Core::String GetToken(const ME::Core::String& in)
		{
			if (!in.Empty())
			{
				SIZE_T tokStart = in.FindFirstNot(TEXT(" \t"));
				SIZE_T tokEnd = in.FindFirst(TEXT(" \t"), tokStart);

				if (tokStart != -1 && tokEnd != -1)
					return in.Substring(tokStart, tokEnd - tokStart);
				else
					return in.Substring(tokStart);
			}
			return TEXT("");
		}

		template <class T>
		inline const T& FindElement(const ME::Core::Array<T>& elements, ME::Core::String& index)
		{
			int32 idx = static_cast<int32>(ME::Core::StringToInt(index, nullptr));
			if (idx == 0)
			{
				ME_ERROR("Invalid vertex index 0 in OBJ file.");
				static T defaultVal{};
				return defaultVal;
			}
			if (idx < 0)
				idx = static_cast<int32>(elements.Size()) + idx;
			else
				idx--;
			if (idx < 0 || idx >= static_cast<int32>(elements.Size()))
			{
				ME_ERROR("Vertex index out of bounds: {} (array size: {})", idx + 1, elements.Size());
				static T defaultVal{};
				return defaultVal;
			}
			return elements[idx];
		}

		inline void CenterMesh(ME::Core::Array<ME::Assets::Vertex>& vertices)
		{
			if (vertices.Size() == 0)
				return;

			ME::Core::Math::Vector3D32 centroid(0.0f, 0.0f, 0.0f);
			for (const auto& vertex : vertices)
				centroid += vertex.Position;
			centroid /= static_cast<float32>(vertices.Size());

			for (auto& vertex : vertices)
				vertex.Position -= centroid;
		}

		inline void CenterMeshGroup(ME::Core::Array<ME::Core::Memory::Reference<ME::Assets::Mesh>>& meshes)
		{
			if (meshes.Empty())
				return;

			SIZE_T totalVertices = 0;
			ME::Core::Math::Vector3D32 centroid(0.0f, 0.0f, 0.0f);
			for (const auto& mesh : meshes)
			{
				totalVertices += mesh->GetVertices().Size();
				for (const auto& vertex : mesh->GetVertices())
					centroid += vertex.Position;
			}

			if (totalVertices == 0)
				return;

			centroid /= static_cast<float32>(totalVertices);

			for (auto& mesh : meshes)
				for (auto& vertex : mesh->GetVertices())
					vertex.Position -= centroid;

			ME::Core::Math::Vector3D32 newCentroid(0.0f, 0.0f, 0.0f);
			SIZE_T newTotalVertices = 0;
			for (const auto& mesh : meshes)
			{
				newTotalVertices += mesh->GetVertices().Size();
				for (const auto& vertex : mesh->GetVertices())
					newCentroid += vertex.Position;
			}
			newCentroid /= static_cast<float32>(newTotalVertices);
		}
	}

	AssetLoadResult AssetLoader::Load(const ME::Core::String& filePath, bool centered,
		AssetFileFormats format)
	{
		switch (format)
		{
			case AssetFileFormats::OBJ:
				return LoadOBJ(filePath.String(), centered);
			case AssetFileFormats::TRG:
				return LoadTGA(filePath.String());
		    case AssetFileFormats::WAV:
			    return LoadWAV(filePath.String());
			default:
				ME_ERROR("Unsupported asset format!: {}", static_cast<uint32>(format));
				return AssetLoadResult();
		}
	}

	AssetLoadResult AssetLoader::LoadOBJ(const char8* filePath, bool centered)
	{
		if (!ME::Core::IO::PFileExists(filePath))
		{
			ME_ERROR("File {0} not found!", ME_LOGGER_TEXT(filePath));
			return AssetLoadResult();
		}

		ME::Core::Memory::Reference<ME::Core::IO::File> file;

		file = ME::Core::IO::POpenFile(filePath);
		if (!file || !file->IsOpen())
		{
			ME_ERROR("File is not opened! Error: {0}",
				file ? static_cast<uint32>(file->GetFileLastError()) : 0);
			return AssetLoadResult();
		}

		bool readingStarted = false;
		AssetLoadResult result;
		result.Meshes = ME::Core::Array<ME::Core::Memory::Reference<Assets::Mesh>>();

		ME::Core::Memory::Reference<ME::Assets::Mesh> mesh;

		ME::Core::Array<Assets::Vertex> vertices(10000);
		ME::Core::Array<uint32> indices(10000);
		ME::Core::Array<Assets::Vertex> faceVertices(3);
		ME::Core::Array<uint32> faceIndices(3);
		ME::Core::UnorderedMap<Assets::Vertex, uint32> vertexToIndex(1000);

		ME::Core::Array<ME::Core::Math::Vector3D32> positions(1000);
		ME::Core::Array<ME::Core::Math::Vector2D32> uvCoords(1000);
		ME::Core::Array<ME::Core::Math::Vector3D32> normals(1000);

		ME::Core::String meshName;
		ME::Core::String line;
		ME::Core::String token = TEXT("");
		ME::Core::String value = TEXT("");
		ME::Core::Array<ME::Core::String> values(4);
		ME::Core::Array<ME::Core::String> faceValues(3);

		uint32 faceCount = 0;

		vertices.Clear();
		indices.Clear();
		positions.Clear();
		uvCoords.Clear();
		normals.Clear();

		while (file->Read(line, Core::IO::StringReadMode::Line))
		{
			token = LocalFunctions::GetToken(line);

			if (token == TEXT("#")) continue;
			else if (token == TEXT("o") || token == TEXT("g"))
			{
				if (!indices.Empty() && !vertices.Empty())
				{
					mesh = ME::Render::Manager::MeshManager::Get().CreateMesh();
					mesh->UpdateMeshInfo(vertices, indices);
					mesh->SetGroupName(meshName);
					result.Meshes.EmplaceBack(mesh);

					ME_INFO("Mesh vertex count: {}", vertices.Size());
					ME_INFO("Mesh indices count: {}", indices.Size());
					ME_INFO("Mesh vertexToIndex size: {}", vertexToIndex.Size());

					vertices.Clear();
					indices.Clear();
					meshName.Clear();
					vertexToIndex.Clear();

					meshName = LocalFunctions::GetTokenValues(line);
				}
				else
					meshName = LocalFunctions::GetTokenValues(line);
			}
			else if (token == TEXT("mtllib")) continue;
			else if (token == TEXT("usemtl")) continue;
			else if (token == TEXT("v"))
			{
				Core::Math::Vector3D32 position;
				value = LocalFunctions::GetTokenValues(line);
				LocalFunctions::Split(value, values, TEXT(" "));
				position.X = static_cast<float32>(Core::StringToFloat(values[0], nullptr));
				position.Y = static_cast<float32>(Core::StringToFloat(values[1], nullptr));
				position.Z = static_cast<float32>(Core::StringToFloat(values[2], nullptr));
				positions.EmplaceBack(position);
			}
			else if (token == TEXT("vt"))
			{
				Core::Math::Vector2D32 coords;
				value = LocalFunctions::GetTokenValues(line);
				LocalFunctions::Split(value, values, TEXT(" "));
				coords.X = static_cast<float32>(Core::StringToFloat(values[0], nullptr));
				coords.Y = static_cast<float32>(Core::StringToFloat(values[1], nullptr));
				uvCoords.EmplaceBack(coords);
			}
			else if (token == TEXT("vn"))
			{
				Core::Math::Vector3D32 normal;
				value = LocalFunctions::GetTokenValues(line);
				LocalFunctions::Split(value, values, TEXT(" "));
				normal.X = static_cast<float32>(Core::StringToFloat(values[0], nullptr));
				normal.Y = static_cast<float32>(Core::StringToFloat(values[1], nullptr));
				normal.Z = static_cast<float32>(Core::StringToFloat(values[2], nullptr));
				normals.EmplaceBack(normal);
			}
			else if (token == TEXT("vp")) continue;
			else if (token == TEXT("f"))
			{
				value = LocalFunctions::GetTokenValues(line);
				LocalFunctions::Split(value, values, TEXT(" "));
				if (values.Size() < 3)
					continue;

				faceIndices.Clear();
				faceVertices.Clear();

				for (const Core::String& val : values)
				{
					LocalFunctions::Split(val, faceValues, TEXT("/"));

					Assets::Vertex vertex;
					vertex.Position = LocalFunctions::FindElement(positions, faceValues[0]);
					if (faceValues.Size() > 1 && !faceValues[1].Empty())
						vertex.TextureCoords = LocalFunctions::FindElement(uvCoords, faceValues[1]);
					if (faceValues.Size() > 2 && !faceValues[2].Empty())
						vertex.Normal = LocalFunctions::FindElement(normals, faceValues[2]);

					faceVertices.EmplaceBack(vertex);
				}

				if (faceVertices.Size() == 3)
				{
				    for (const Assets::Vertex& vertex : faceVertices)
				    {
						if (vertexToIndex.Contains(vertex))
							indices.EmplaceBack(vertexToIndex.At(vertex));
						else
						{
							uint32 index = static_cast<uint32>(vertices.Size());
							indices.EmplaceBack(index);
							vertices.EmplaceBack(vertex);
							vertexToIndex.Insert(vertex, index);
						}
				    }
					continue;
				}

				faceIndices.Clear();
				Triangulate(faceVertices, faceIndices);

				for (const uint32& localIndex : faceIndices)
				{
					Assets::Vertex vertex = faceVertices[localIndex];

					if (vertexToIndex.Contains(vertex))
						indices.EmplaceBack(vertexToIndex.At(vertex));
					else
					{
						uint32 index = static_cast<uint32>(vertices.Size());
						indices.EmplaceBack(index);
						vertices.EmplaceBack(vertex);
						vertexToIndex.Insert(vertex, index);
					}
				}
			}
			else if (token == TEXT("l")) continue;
			else if (token == TEXT("s")) continue;
		}
		if (!indices.Empty() && !vertices.Empty())
		{
			mesh = ME::Render::Manager::MeshManager::Get().CreateMesh();
			mesh->UpdateMeshInfo(vertices, indices);
			mesh->SetGroupName(meshName);
			result.Meshes.EmplaceBack(mesh);
		}

		if (centered) {
			LocalFunctions::CenterMeshGroup(result.Meshes);
		}

		file->Close();
		return result;
	}

	AssetLoadResult AssetLoader::LoadTGA(const char8* filePath)
	{
		if (!ME::Core::IO::PFileExists(filePath))
		{
			ME_ERROR("File {0} not found!", CONVERT_TEXT(filePath));
			return {};
		}

		auto file = ME::Core::IO::POpenFile(filePath);
		if (!file || !file->IsOpen())
		{
			ME_ERROR("File is not opened! Error: {0}",
				file ? static_cast<uint32>(file->GetFileLastError()) : 0);
			return {};
		}

		uint8* data = nullptr;
		SIZE_T size = 0;
		size = file->GetFileSize();
		data = new uint8[size];

		if (!file->ReadBinary(data, size))
		{
			ME_ERROR("Failed to read file! Error: {0}",
				file ? static_cast<uint32>(file->GetFileLastError()) : 0);
			return {};
		}

		if (size < sizeof(TRGHeader))
		{
			ME_ERROR("Invalid TGA file size");
			delete[] data;
			return {};
		}

		TRGHeader header;
		memcpy(&header, data, sizeof(TRGHeader));

		ME::Core::Math::Resolution2D<uint32> resolution{ header.width, header.height };

		if (header.pixelDepth != BIT(5))
		{
			ME_ERROR("Unsupported \"{1}\" file! Error: {0}",
				file ? static_cast<uint32>(file->GetFileLastError()) : 0, CONVERT_TEXT(filePath));
			delete[] data;
			return {};
		}

		SIZE_T imageSize = size - sizeof(TRGHeader);
		uint8* targaData = new uint8[imageSize];
		memcpy(targaData, data + sizeof(TRGHeader), imageSize);

		uint8* imageData = new uint8[imageSize];
		delete[] data;

		int32 index = 0;
		int32 k = (resolution.x * resolution.y * 4) - (resolution.x * 4);

		for (uint32 j = 0; j < resolution.y; j++)
		{
			for (uint32 i = 0; i < resolution.x; i++)
			{
				imageData[index + 0] = targaData[k + 2];
				imageData[index + 1] = targaData[k + 1];
				imageData[index + 2] = targaData[k + 0];
				imageData[index + 3] = targaData[k + 3];

				k += 4;
				index += 4;
			}
			k -= resolution.x * 8;
		}

		delete[] targaData;

		AssetLoadResult result;
		result.Images.PushBack(ME::Core::Memory::Reference<Assets::Image>(new Assets::Image(imageData, imageSize, resolution, file->GetFileInfo().Name)));

		file->Close();
		return result;
	}

    AssetLoadResult AssetLoader::LoadWAV(const char8* filePath)
    {
		if (!ME::Core::IO::PFileExists(filePath))
		{
			ME_ERROR("File {0} not found!", CONVERT_TEXT(filePath));
			return {};
		}

		auto file = ME::Core::IO::POpenFile(filePath);
		if (!file || !file->IsOpen())
		{
			ME_ERROR("File is not opened! Error: {0}",
				file ? static_cast<uint32>(file->GetFileLastError()) : 0);
			return {};
		}

		WAVHeader header = {};
		Assets::AudioFile audioFile = {};

		uint32 chunk = 0;
		uint32 chunkSize = 0;
		uint8 dump[8];
	    while (!file->Eof())
		{
			if (!file->ReadBinary(&chunk, 4))
			    break;
	        switch (chunk)
            {
				case FourCC('R', 'I', 'F', 'F'):
                {
                    // File size
			        file->ReadBinary(&header.FileSize, sizeof(uint32));
			        file->ReadBinary(&header.FileFormat, sizeof(uint32));
					break;
                }
				case FourCC('f', 'm', 't', 0x20):
				{
                    // Chunk size
			        file->ReadBinary(&chunkSize, sizeof(uint32));

				    // fmt chunk data
			        file->ReadBinary(&header.AudioFormat,	sizeof(uint16));
			        file->ReadBinary(&header.NbrChannels,	sizeof(uint16));
			        file->ReadBinary(&header.Frequency,		sizeof(uint32));
			        file->ReadBinary(&header.BytePerSec,	sizeof(uint32));
			        file->ReadBinary(&header.BytePerBloc,	sizeof(uint16));
			        file->ReadBinary(&header.BitsPerSample,	sizeof(uint16));
					if (header.AudioFormat != WAVE_FORMAT_PCM)
					{
						file->ReadBinary(&header.cbSize,	sizeof(uint16));
						if (header.cbSize == 22)
						{
							file->ReadBinary(&header.wValidBitsPerSample, sizeof(uint16));
							file->ReadBinary(&header.dwChannelMask, sizeof(uint32));
							file->ReadBinary(header.SubFormat, sizeof(uint8) * 16);
						}
					}
                    break;
				}
				case FourCC('d', 'a', 't', 'a'):
				{
					file->ReadBinary(&header.DataSize, sizeof(uint32));
					header.Data = new uint8[header.DataSize];
					file->ReadBinary(header.Data, header.DataSize);
					if (header.DataSize % 2 != 0)
					    file->ReadBinary(dump, sizeof(uint8));
					break;
				}
				case FourCC('f', 'a', 'c', 't'):
				{
					file->ReadBinary(&chunkSize, sizeof(uint32));
					break;
				}
				default:
                {
					file->ReadBinary(&chunkSize, sizeof(uint32));
					file->Seek(file->Tell() + chunkSize);
                }   
            }
		}

		audioFile.Format = Assets::AudioFormat::WAV;
		audioFile.AudioInfo.Frequency = header.Frequency;
		audioFile.AudioInfo.BitPerSample = header.BitsPerSample;
		audioFile.AudioInfo.ChannelCount = header.NbrChannels;
		audioFile.AudioInfo.Length = static_cast<float32>(header.DataSize) / static_cast<float32>(header.BytePerSec);
		audioFile.AudioInfo.SampleCount = header.DataSize / (header.NbrChannels * header.BitsPerSample / 8);
		audioFile.Data = header.Data;
		audioFile.Size = header.DataSize;

		AssetLoadResult result = {};
		result.Audio = audioFile;

        return result;
    }


    ME::Core::Math::Vector3D32 AssetLoader::CalculateNormal(const ME::Core::Array<Assets::Vertex>& vertices)
	{
		ME::Core::Math::Vector3D32 normal(0.0f, 0.0f, 0.0f);

		for (int32 i = 0; i < static_cast<int32>(vertices.Size()); ++i)
		{
			int32 j = (i + 1) % vertices.Size();
			const auto& pi = vertices[i].Position;
			const auto& pj = vertices[j].Position;
			normal.X += (pi.Y - pj.Y) * (pi.Z + pj.Z);
			normal.Y += (pi.Z - pj.Z) * (pi.X + pj.X);
			normal.Z += (pi.X - pj.X) * (pi.Y + pj.Y);
		}

		float32 len = normal.Length();
		if (len > 1e-6f)
		{
			normal /= len;
		}
		else
		{
			normal = ME::Core::Math::Vector3D32(0.0f, 0.0f, 1.0f);
		}
		return normal;
	}

	void AssetLoader::ProjectTo2D(const ME::Core::Array<Assets::Vertex>& vertices,
	                              ME::Core::Array<ME::Core::Math::Vector2D32>& out2D)
	{
		Core::Math::Vector3D32 normal = CalculateNormal(vertices).Abs();

		int32 dropAxis = 0;
		if (normal.y > normal.x && normal.y > normal.z) dropAxis = 1;
		else if (normal.z > normal.x && normal.z > normal.y) dropAxis = 2;

		out2D.Clear();
		out2D.Reserve(vertices.Size());
		for (auto& v : vertices)
		{
			if (dropAxis == 0) out2D.EmplaceBack(ME::Core::Math::Vector2D32{ v.Position.y, v.Position.z });
			else if (dropAxis == 1) out2D.EmplaceBack(ME::Core::Math::Vector2D32{ v.Position.x, v.Position.z });
			else out2D.EmplaceBack(ME::Core::Math::Vector2D32{ v.Position.x, v.Position.y });
		}
	}

	bool AssetLoader::IsPointInTriangle(const ME::Core::Math::Vector2D32& pt, const ME::Core::Math::Vector2D32& a,
		const ME::Core::Math::Vector2D32& b, const ME::Core::Math::Vector2D32& c)
	{
		bool b1 = SIGN(pt, a, b) < 0.0f;
		bool b2 = SIGN(pt, b, c) < 0.0f;
		bool b3 = SIGN(pt, c, a) < 0.0f;

		return (b1 == b2) && (b2 == b3);
	}

	bool AssetLoader::IsEar(int i, const ME::Core::Array<ME::Core::Math::Vector2D32>& poly,
		const ME::Core::Array<uint32>& idx)
	{
		int prev = idx[(i + idx.Size() - 1) % idx.Size()];
		int curr = idx[i];
		int next = idx[(i + 1) % idx.Size()];

		const ME::Core::Math::Vector2D32& a = poly[prev];
		const ME::Core::Math::Vector2D32& b = poly[curr];
		const ME::Core::Math::Vector2D32& c = poly[next];

		float cross = (b.x - a.x) * (c.y - a.y) -
			(b.y - a.y) * (c.x - a.x);
		if (cross <= 0) return false;

		for (uint32 j = 0; j < idx.Size(); j++)
		{
			if (j == static_cast<uint32>(i) || j == static_cast<uint32>((i + 1) % idx.Size()) || j == static_cast<uint32>((i + idx.Size() - 1) % idx.Size()))
				continue;
			if (IsPointInTriangle(poly[idx[j]], a, b, c))
				return false;
		}

		return true;
	}

	void AssetLoader::Triangulate(const ME::Core::Array<Assets::Vertex>& vertices,
		ME::Core::Array<uint32>& indicesOut)
	{
		if (vertices.Size() == 4)
		{
			indicesOut.EmplaceBack(0);
			indicesOut.EmplaceBack(1);
			indicesOut.EmplaceBack(2);

			indicesOut.EmplaceBack(0);
			indicesOut.EmplaceBack(2);
			indicesOut.EmplaceBack(3);
			return;
		}

		Core::Array<ME::Core::Math::Vector2D32> projected;
		ProjectTo2D(vertices, projected);

		Core::Array<uint32> idx;
		for (uint32 i = 0; i < vertices.Size(); i++)
			idx.EmplaceBack(i);

		while (idx.Size() > 3)
		{
			bool earFound = false;
			for (uint32 i = 0; i < idx.Size(); i++)
			{
				if (IsEar(i, projected, idx))
				{
					uint32 prev = idx[(i + idx.Size() - 1) % idx.Size()];
					uint32 curr = idx[i];
					uint32 next = idx[(i + 1) % idx.Size()];

					indicesOut.EmplaceBack(prev);
					indicesOut.EmplaceBack(curr);
					indicesOut.EmplaceBack(next);

					idx.Erase(idx.Begin() + i);
					earFound = true;
					break;
				}
			}

			if (!earFound)
				break;
		}

		if (idx.Size() == 3)
		{
			indicesOut.EmplaceBack(idx[0]);
			indicesOut.EmplaceBack(idx[1]);
			indicesOut.EmplaceBack(idx[2]);
		}
	}
}
