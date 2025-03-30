#include "AssetLoader.h"
#include <Core/Utils/Logging/Logger.h>

import Pawn.Core.IO;
import Pawn.Core.Container.UnorderedMap;

namespace Pawn::Utility
{
	AssetLoadResult AssetLoader::Load(const Pawn::Core::Containers::String& filePath, AssetFileFormats format)
	{
		switch (format)
		{
		case AssetFileFormats::OBJ:
			return LoadOBJ(filePath.GetString());
		default:
			PE_ERROR("Unsupported asset format!: {}", (uint32)(format));
			return AssetLoadResult();
		}
	}

	AssetLoadResult AssetLoader::LoadOBJ(const uchar* filePath)
	{
		bool result;
		Pawn::Core::Memory::Reference<Pawn::Core::IO::File> file;

		result = Pawn::Core::IO::PFileExists(filePath);
		if (!result)
		{
			PE_ERROR(TEXT("File {0} not found!"), filePath);
			return AssetLoadResult();
		}

		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32> positions;
		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32> uvCoords;
		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32> normals;

		struct VertexKey {
			int32 p, t, n;
			VertexKey(int32 pos, int32 tex, int32 norm) : p(pos), t(tex), n(norm) {}
			bool operator==(const VertexKey& other) const {
				return p == other.p && t == other.t && n == other.n;
			}
		};

		struct VertexEntry {
			VertexKey key;
			uint32 index;
			VertexEntry(const VertexKey& k, uint32 idx) : key(k), index(idx) {}
		};

		Pawn::Core::Containers::UnorderedMap<VertexKey, Assets::Vertex> vert;
		Pawn::Core::Containers::Array<VertexEntry> vertexMap;
		Pawn::Core::Containers::Array<Assets::Vertex> vertices;
		Pawn::Core::Containers::Array<int32> indices;

		struct GroupEntry {
			Pawn::Core::Containers::String name;
			Pawn::Core::Containers::Array<Assets::Vertex> groupVertices;
			Pawn::Core::Containers::Array<int32> groupIndices;
			Pawn::Core::Containers::String material;

			GroupEntry(const Pawn::Core::Containers::String& n) : name(n) {}
		};
		Pawn::Core::Containers::Array<GroupEntry> groups;

		groups.EmplaceBack(TEXT("default"));
		SIZE_T currentGroupIdx = 0;

		file = Pawn::Core::IO::POpenFile(filePath);
		result = file->IsOpen();
		if (!result)
		{
			PE_ERROR(TEXT("File is not opened! Error: {0}"), (uint32)file->GetFileLastError());
			return AssetLoadResult();
		}

		Pawn::Core::Containers::String buf;
		SIZE_T pos = 0;

		while (!file->Eof())
		{
			result = file->Read(buf, Pawn::Core::IO::StringReadMode::Line);
			if (!result)
			{
				PE_ERROR(TEXT("Failed file reading! Error: {0}"), (int32)file->GetFileLastError());
				break;
			}

			Pawn::Core::Containers::StringView subStr = Pawn::Core::Containers::StringView(buf).Substring(0, 2);

			if (subStr[0] == TEXT('v'))
			{
				// Position
				if (subStr[1] == TEXT('\0') || subStr[1] == TEXT(' '))
				{
					pos = 0;
					positions.EmplaceBack((float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + 2, &pos), 0.f, 0.f);
					positions[positions.GetSize() - 1].Y = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + pos, &pos);
					positions[positions.GetSize() - 1].Z = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + pos, &pos);
				}
				// Texture
				else if (subStr[1] == TEXT('t'))
				{
					pos = 0;
					uvCoords.EmplaceBack((float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + 2, &pos), 0.f, 0.f);
					uvCoords[uvCoords.GetSize() - 1].Y = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + pos, &pos);
					uvCoords[uvCoords.GetSize() - 1].Z = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + pos, &pos);
				}
				// Normals
				else if (subStr[1] == TEXT('n'))
				{
					pos = 0;
					normals.EmplaceBack((float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + 2, &pos), 0.f, 0.f);
					normals[normals.GetSize() - 1].Y = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + pos, &pos);
					normals[normals.GetSize() - 1].Z = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + pos, &pos);
				}
			}
			else if (subStr[0] == TEXT('g'))
			{
				pos = 2;
				//Pawn::Core::Containers::String groupName = Pawn::Core::Containers::String(buf.GetString() + pos).Trim();
				bool found = false;
				for (SIZE_T i = 0; i < groups.GetSize(); ++i)
				{
					if (groups[i].name == groupName)
					{
						currentGroupIdx = i;
						found = true;
						break;
					}
				}
				if (!found)
				{
					groups.EmplaceBack(groupName);
					currentGroupIdx = groups.GetSize() - 1;
				}
			}
			else if (subStr[0] == TEXT('f'))
			{
				pos = 0;
				const uchar* posB = buf.GetString() + 1;

				for (int i = 0; i < 3; ++i)
				{
					int32 p = (int32)Pawn::Core::Containers::StringToUint(posB, &pos) - 1;
					posB += pos + 1;
					int32 t = (int32)Pawn::Core::Containers::StringToUint(posB, &pos) - 1;
					posB += pos + 1;
					int32 n = (int32)Pawn::Core::Containers::StringToUint(posB, &pos) - 1;
					posB += pos + 1;

					VertexKey key(p, t, n);
					uint32 index = 0;
					bool found = false;

					for (SIZE_T j = 0; j < vertexMap.GetSize(); ++j)
					{
						if (vertexMap[j].key == key)
						{
							index = vertexMap[j].index;
							found = true;
							break;
						}
					}

					if (!found)
					{
						Assets::Vertex face;
						face.x = positions[p].X;
						face.y = positions[p].Y;
						face.z = positions[p].Z;
						face.uvU = uvCoords[t].X;
						face.uvV = uvCoords[t].Y;
						face.uvW = uvCoords[t].Z;
						face.normalX = normals[n].X;
						face.normalY = normals[n].Y;
						face.normalZ = normals[n].Z;

						index = static_cast<uint32>(vertices.GetSize());
						vertices.PushBack(face);
						vertexMap.EmplaceBack(key, index);
					}

					groups[currentGroupIdx].groupIndices.EmplaceBack(index);
				}
			}
		}

		AssetLoadResult resultLoad();
		/*resultLoad.vertices = std::move(vertices);
		resultLoad.indices = std::move(indices);

		resultLoad.vertices.Clear();
		resultLoad.indices.Clear();
		for (SIZE_T i = 0; i < groups.GetSize(); ++i)
		{
			SIZE_T offset = resultLoad.vertices.GetSize();
			for (SIZE_T j = 0; j < groups[i].groupVertices.GetSize(); ++j)
			{
				resultLoad.vertices.PushBack(groups[i].groupVertices[j]);
			}
			for (SIZE_T j = 0; j < groups[i].groupIndices.GetSize(); ++j)
			{
				resultLoad.indices.EmplaceBack(groups[i].groupIndices[j] + offset);
			}
		*/}

		return resultLoad;
	}
}