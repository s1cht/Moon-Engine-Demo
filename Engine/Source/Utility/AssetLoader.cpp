#include "AssetLoader.h"

#include <Core/Containers/Tables/UnorderedMap.hpp>
#include <Core/Platform/Base/IO.hpp>

#define DEBUG_MESH
//#define DEBUG_INDICES

namespace Pawn::Utility
{
	namespace LocalFunctions
	{
		void Split(const Pawn::Core::Containers::String& str, Pawn::Core::Containers::Array<Pawn::Core::Containers::String>& out, Pawn::Core::Containers::String splitStr)
		{
			out.Clear();

			Pawn::Core::Containers::String temp;

			for (int32 i = 0; i < (int32)str.GetSize(); i++)
			{
				Pawn::Core::Containers::String test = str.Substring(i, splitStr.GetSize());

				if (test == splitStr)
				{
					if (!temp.Empty())
					{
						out.EmplaceBack(temp);
						temp.Clear();
					}
					i += (int32)splitStr.GetSize() - 1;
				}
				else
				{
					temp += str[i];
				}
			}

			if (!temp.Empty())
			{
				out.EmplaceBack(temp);
			}
		}

		bool IsOnSameSide(Pawn::Core::Math::Vector3D32 p1, Pawn::Core::Math::Vector3D32 p2, 
			Pawn::Core::Math::Vector3D32 a, Pawn::Core::Math::Vector3D32 b)
		{
			Pawn::Core::Math::Vector3D32 cp1 = (b - a).Cross(p1 - a);
			Pawn::Core::Math::Vector3D32 cp2 = (b - a).Cross(p2 - a);

			if (cp1.Dot(cp2) >= 0)
				return true;
			else
				return false;
		}

		Pawn::Core::Math::Vector3D32 GenerateTriangleNormal(Pawn::Core::Math::Vector3D32 t1, 
			Pawn::Core::Math::Vector3D32 t2, 
			Pawn::Core::Math::Vector3D32 t3)
		{
			Pawn::Core::Math::Vector3D32 u = t2 - t1;
			Pawn::Core::Math::Vector3D32 v = t3 - t1;

			Pawn::Core::Math::Vector3D32 normal = u.Cross(v);

			return normal;
		}

		bool IsInTriangle(Pawn::Core::Math::Vector3D32 point, 
			Pawn::Core::Math::Vector3D32 tri1, Pawn::Core::Math::Vector3D32 tri2, Pawn::Core::Math::Vector3D32 tri3)
		{
			bool within_tri_prisim = IsOnSameSide(point, tri1, tri2, tri3) && IsOnSameSide(point, tri2, tri1, tri3)
				&& IsOnSameSide(point, tri3, tri1, tri2);

			if (!within_tri_prisim)
				return false;

			Pawn::Core::Math::Vector3D32 n = GenerateTriangleNormal(tri1, tri2, tri3);

			Pawn::Core::Math::Vector3D32 proj = point.Project(n);

			if (proj.Length() == 0)
				return true;
			else
				return false;
		}

		Pawn::Core::Containers::String GetTokenValues(const Pawn::Core::Containers::String& in)
		{
			if (!in.Empty())
			{
				SIZE_T tokStart = in.FindFirstNot(TEXT(" \t"));
				SIZE_T spaceStart = in.FindFirst(TEXT(" \t"), tokStart);
				SIZE_T valStart = in.FindFirstNot(TEXT(" \t"), spaceStart);
				SIZE_T valEnd = in.FindLastNot(TEXT(" \t"));

				if (valStart != -1 && valEnd != -1)
				{
					return in.Substring(valStart, valEnd - valStart + 1);
				}
				else if (valStart != -1)
				{
					return in.Substring(valEnd);
				}
			}
			return TEXT("");
		}

		inline Pawn::Core::Containers::String GetToken(const Pawn::Core::Containers::String& in)
		{
			if (!in.Empty())
			{
				SIZE_T tokStart = in.FindFirstNot(TEXT(" \t"));
				SIZE_T tokEnd = in.FindFirst(TEXT(" \t"));

				if (tokStart != -1 && tokEnd != -1)
					return in.Substring(tokStart, tokEnd - tokStart);
				else
					return in.Substring(tokStart);
			}
			return TEXT("");
		}

		template <class T>
		inline const T& FindElement(const Pawn::Core::Containers::Array<T>& elements, Pawn::Core::Containers::String& index)
		{
			int32 idx = (int32)Pawn::Core::Containers::StringToInt(index, nullptr);
			if (idx < 0)
				idx = (int32)elements.GetSize() + idx;
			else
				idx--;
			return elements[idx];
		}

		inline void GenVerticesFromRawOBJ(
			Pawn::Core::Containers::Array<Pawn::Assets::Vertex>& vertices,
			Pawn::Core::Containers::Array<int32>& indices,
			const Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32>& positions,
			const Pawn::Core::Containers::Array<Pawn::Core::Math::Vector2D32>& textureCoords,
			const Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32>& normals,
			Pawn::Core::Containers::String currentLine)
		{
			vertices.Clear();
			indices.Clear();

			Pawn::Core::Containers::UnorderedMap<Pawn::Core::Containers::String, int32> vertexMap;

			Pawn::Core::Containers::Array<Pawn::Core::Containers::String> sface;
			LocalFunctions::Split(LocalFunctions::GetTokenValues(currentLine), sface, Pawn::Core::Containers::String(TEXT(" ")));

			bool noNormal = false;

			for (int32 i = 0; i < (int32)sface.GetSize(); i++)
			{
				auto it = vertexMap.Find(sface[i]);
				if (it != vertexMap.End())
				{
					indices.PushBack(it->Value2);
					continue;
				}

				Pawn::Core::Containers::Array<Pawn::Core::Containers::String> svert;
				LocalFunctions::Split(sface[i], svert, Pawn::Core::Containers::String(TEXT("/")));

				int32 vtype;
				if (svert.GetSize() == 1)
					vtype = 1;
				else if (svert.GetSize() == 2)
					vtype = 2;
				else if (svert.GetSize() == 3)
					vtype = svert[1] != TEXT("") ? 4 : 3;
				else
					continue;

				Pawn::Assets::Vertex vertexTemp;
				switch (vtype)
				{
				case 1:
				{
					vertexTemp.Position = LocalFunctions::FindElement(positions, svert[0]);
					vertexTemp.TextureCoordinate = Pawn::Core::Math::Vector2D32(0, 0);
					noNormal = true;
					break;
				}
				case 2:
				{
					vertexTemp.Position = LocalFunctions::FindElement(positions, svert[0]);
					vertexTemp.TextureCoordinate = LocalFunctions::FindElement(textureCoords, svert[1]);
					noNormal = true;
					break;
				}
				case 3:
				{
					vertexTemp.Position = LocalFunctions::FindElement(positions, svert[0]);
					vertexTemp.TextureCoordinate = Pawn::Core::Math::Vector2D32(0, 0);
					vertexTemp.Normal = LocalFunctions::FindElement(normals, svert[2]);
					break;
				}
				case 4:
				{
					vertexTemp.Position = LocalFunctions::FindElement(positions, svert[0]);
					vertexTemp.TextureCoordinate = LocalFunctions::FindElement(textureCoords, svert[1]);
					vertexTemp.Normal = LocalFunctions::FindElement(normals, svert[2]);
					break;
				}
				default:
					continue;
				}

				int32 newIndex = vertices.GetSize();
				vertices.EmplaceBack(vertexTemp);
				vertexMap.Insert(sface[i], newIndex);
				indices.PushBack(newIndex);
			}

			if (noNormal && vertices.GetSize() >= 3)
			{
				Pawn::Core::Math::Vector3D32 edge1 = vertices[1].Position - vertices[0].Position;
				Pawn::Core::Math::Vector3D32 edge2 = vertices[2].Position - vertices[1].Position;
				Pawn::Core::Math::Vector3D32 normal = edge1.Cross(edge2);
				if (normal.LengthSquared() > 1e-6)
				{
					normal.Normalize();
					for (int32 i = 0; i < (int32)vertices.GetSize(); i++)
						vertices[i].Normal = normal;
				}
				else
				{
					for (int32 i = 0; i < (int32)vertices.GetSize(); i++)
						vertices[i].Normal = Pawn::Core::Math::Vector3D32(0, 0, 1);
				}
			}
		}

		inline void VertexTriangluation(
			Pawn::Core::Containers::Array<int32>& oIndices,
			const Pawn::Core::Containers::Array<int32>& iIndices,
			const Pawn::Core::Containers::Array<Pawn::Assets::Vertex>& iVerts)
		{
			oIndices.Clear();
			if (iIndices.GetSize() < 3)
				return;

			for (int32 i = 1; i < (int32)iIndices.GetSize() - 1; i++)
			{
				int32 idx0 = iIndices[0];
				int32 idx1 = iIndices[i];
				int32 idx2 = iIndices[i + 1];

				if (idx0 == idx1 || idx1 == idx2 || idx2 == idx0)
					continue;

				oIndices.EmplaceBack(idx0);
				oIndices.EmplaceBack(idx1);
				oIndices.EmplaceBack(idx2);
			}
		}

		inline void CenterMesh(Pawn::Core::Containers::Array<Pawn::Assets::Vertex>& vertices)
		{
			if (vertices.GetSize() == 0)
				return;

			Pawn::Core::Math::Vector3D32 centroid(0.0f, 0.0f, 0.0f);
			for (const auto& vertex : vertices)
			{
				centroid += vertex.Position;
			}
			centroid /= (float32)vertices.GetSize();

			for (auto& vertex : vertices)
			{
				vertex.Position -= centroid;
			}
		}

		inline void CenterMeshGroup(Pawn::Core::Containers::Array<Pawn::Core::Memory::Reference<Pawn::Assets::Mesh>>& meshes)
		{
			if (meshes.Empty())
				return;

			int32 totalVertices = 0;
			Pawn::Core::Math::Vector3D32 centroid(0.0f, 0.0f, 0.0f);
			for (const auto& mesh : meshes)
			{
				totalVertices += mesh->GetVertices().GetSize();
				for (const auto& vertex : mesh->GetVertices())
				{
					centroid += vertex.Position;
				}
			}

			if (totalVertices == 0)
				return;

			centroid /= (float32)totalVertices;

			for (auto& mesh : meshes)
			{
				for (auto& vertex : mesh->GetVertices())
				{
					vertex.Position -= centroid;
				}
			}

			Pawn::Core::Math::Vector3D32 newCentroid(0.0f, 0.0f, 0.0f);
			int32 newTotalVertices = 0;
			for (const auto& mesh : meshes)
			{
				newTotalVertices += mesh->GetVertices().GetSize();
				for (const auto& vertex : mesh->GetVertices())
				{
					newCentroid += vertex.Position;
				}
			}
			newCentroid /= (float32)newTotalVertices;
		}
	}

	AssetLoadResult AssetLoader::Load(const Pawn::Core::Containers::String& filePath, bool centered, AssetFileFormats format)
	{
		switch (format)
		{
		case AssetFileFormats::OBJ:
			return LoadOBJ(filePath.GetString(), centered);
		default:
			PE_ERROR("Unsupported asset format!: {}", (uint32)(format));
			return AssetLoadResult();
		}
	}

	AssetLoadResult AssetLoader::LoadOBJ(const uchar* filePath, bool centered)
	{
		if (!Pawn::Core::IO::PFileExists(filePath))
		{
			PE_ERROR(TEXT("File {0} not found!"), filePath);
			return AssetLoadResult();
		}
	
		Pawn::Core::Memory::Reference <Pawn::Core::IO::File> file;

		file = Pawn::Core::IO::POpenFile(filePath);
		if (!file || !file->IsOpen())
		{
			PE_ERROR(TEXT("File is not opened! Error: {0}"),
				file ? (uint32)file->GetFileLastError() : 0);
			return AssetLoadResult();
		}

		bool readingStarted = false;
		AssetLoadResult result;

		result.Meshes = Pawn::Core::Containers::Array<Pawn::Core::Memory::Reference<Assets::Mesh>>();

		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32> positions(1000);
		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector2D32> uvCoords(1000);
		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32> normals(1000);

		Pawn::Assets::Mesh* mesh;
		Pawn::Core::Containers::String meshName;
		Pawn::Core::Containers::Array<Assets::Vertex> vertices(1000);
		Pawn::Core::Containers::Array<int32> indices(1000);

		Pawn::Core::Containers::String readStr;
		while (file->Read(readStr, Pawn::Core::IO::StringReadMode::Line))
		{
			if (LocalFunctions::GetToken(readStr) == TEXT("o") || LocalFunctions::GetToken(readStr) == TEXT("g") || LocalFunctions::GetToken(readStr)[0] == TEXT('g'))
			{
				if (!readingStarted)
				{
					readingStarted = true;

					if (LocalFunctions::GetToken(readStr) == TEXT("o") || LocalFunctions::GetToken(readStr) == TEXT("g"))
						meshName = LocalFunctions::GetTokenValues(readStr);
					else
						meshName = TEXT("unnamed");
				}
				else
				{
					if (!indices.Empty() && !vertices.Empty())
					{
						mesh = new Pawn::Assets::Mesh();

#ifdef DEBUG_MESH
						PE_INFO(TEXT("MESH {0}"), meshName.GetString());
#ifdef DEBUG_VERTICES
						for (const auto& vert : vertices)
						{
							PE_INFO(TEXT("Vertex position: {0}, {1}, {2}; UV: {3}, {4}; Normal: {5}, {6}, {7}"),
								vert.Position.X, vert.Position.Y, vert.Position.Z,
								vert.TextureCoordinate.X, vert.TextureCoordinate.Y,
								vert.Normal.X, vert.Normal.Y, vert.Normal.Z
							);
						}
#endif
#ifdef DEBUG_INDICES
						int32 i = 0;
						Pawn::Core::Containers::String temp = TEXT("");
						for (const auto& ind : indices)
						{
							i++;
							temp += TEXT(" ") + Pawn::Core::Containers::ToString(ind);
							if (i == 3)
							{
								PE_INFO(TEXT("Vertex indices: {0}"), temp.GetString());
								temp = TEXT(" ");
								i = 0;
							}
						}
#endif
#endif
						mesh->SetVertexes(vertices);
						mesh->SetIndexes(indices);
						mesh->SetGroupName(meshName);
						result.Meshes.EmplaceBack(Pawn::Core::Memory::Reference<Pawn::Assets::Mesh>(mesh));

						vertices.Clear();
						indices.Clear();
						meshName.Clear();

						meshName = LocalFunctions::GetTokenValues(readStr);
					}
					else
					{
						if (LocalFunctions::GetToken(readStr) == TEXT("o") || LocalFunctions::GetToken(readStr) == TEXT("g"))
							meshName = LocalFunctions::GetTokenValues(readStr);
						else
							meshName = TEXT("unnamed");
					}
				}
			}

			if (LocalFunctions::GetToken(readStr) == TEXT("v"))
			{
				Pawn::Core::Containers::Array<Pawn::Core::Containers::String> spos;
				Pawn::Core::Math::Vector3D32 vpos;
				LocalFunctions::Split(LocalFunctions::GetTokenValues(readStr), spos, Pawn::Core::Containers::String(TEXT(" ")));

				vpos.X = (float32)Pawn::Core::Containers::StringToFloat(spos[0], nullptr);
				vpos.Y = (float32)Pawn::Core::Containers::StringToFloat(spos[1], nullptr);
				vpos.Z = (float32)Pawn::Core::Containers::StringToFloat(spos[2], nullptr);

				positions.EmplaceBack(vpos);
			}
			if (LocalFunctions::GetToken(readStr) == TEXT("vt"))
			{
				Pawn::Core::Containers::Array<Pawn::Core::Containers::String> stex;
				Pawn::Core::Math::Vector2D32 vtex;
				LocalFunctions::Split(LocalFunctions::GetTokenValues(readStr), stex, Pawn::Core::Containers::String(TEXT(" ")));

				vtex.X = (float32)Pawn::Core::Containers::StringToFloat(stex[0], nullptr);
				vtex.Y = (float32)Pawn::Core::Containers::StringToFloat(stex[1], nullptr);

				uvCoords.EmplaceBack(vtex);
			}
			if (LocalFunctions::GetToken(readStr) == TEXT("vn"))
			{
				Pawn::Core::Containers::Array<Pawn::Core::Containers::String> snor;
				Pawn::Core::Math::Vector3D32 vnor;
				LocalFunctions::Split(LocalFunctions::GetTokenValues(readStr), snor, Pawn::Core::Containers::String(TEXT(" ")));

				vnor.X = (float32)Pawn::Core::Containers::StringToFloat(snor[0], nullptr);
				vnor.Y = (float32)Pawn::Core::Containers::StringToFloat(snor[1], nullptr);
				vnor.Z = (float32)Pawn::Core::Containers::StringToFloat(snor[2], nullptr);

				normals.EmplaceBack(vnor);
			}
			if (LocalFunctions::GetToken(readStr) == TEXT("f"))
			{
				Pawn::Core::Containers::Array<Assets::Vertex> tempVertices;
				Pawn::Core::Containers::Array<int32> tempIndices;
				Pawn::Core::Containers::Array<int32> triangulatedIndices;
				LocalFunctions::GenVerticesFromRawOBJ(tempVertices, tempIndices, positions, uvCoords, normals, readStr);
				
				for (int32 i = 0; i < (int32)tempVertices.GetSize(); i++)
					vertices.PushBack(tempVertices[i]);

				LocalFunctions::VertexTriangluation(triangulatedIndices, tempIndices, tempVertices);

				for (int32 i = 0; i < (int32)triangulatedIndices.GetSize(); i++)
				{
					indices.EmplaceBack((int32)((vertices.GetSize()) - tempVertices.GetSize()) + triangulatedIndices[i]);
				}
			}
		}
		if (!indices.Empty() && !vertices.Empty())
		{
			mesh = new Pawn::Assets::Mesh();
#ifdef DEBUG_MESH
			PE_INFO(TEXT("MESH {0}"), meshName.GetString());
#ifdef DEBUG_VERTICES
			for (const auto& vert : vertices)
			{
				PE_INFO(TEXT("Vertex position: {0}, {1}, {2}; UV: {3}, {4}; Normal: {5}, {6}, {7}"),
					vert.Position.X, vert.Position.Y, vert.Position.Z,
					vert.TextureCoordinate.X, vert.TextureCoordinate.Y,
					vert.Normal.X, vert.Normal.Y, vert.Normal.Z
				);
			}
#endif
#ifdef DEBUG_INDICES
			int32 i = 0;
			Pawn::Core::Containers::String temp = TEXT("");
			for (const auto& ind : indices)
			{
				i++;
				temp += TEXT(" ") + Pawn::Core::Containers::ToString(ind);
				if (i == 3)
				{
					PE_INFO(TEXT("Vertex indices: {0}"), temp.GetString());
					temp = TEXT(" ");
					i = 0;
				}
			}
#endif
#endif
			mesh->SetVertexes(vertices);
			mesh->SetIndexes(indices);
			mesh->SetGroupName(meshName);
			result.Meshes.EmplaceBack(Pawn::Core::Memory::Reference<Pawn::Assets::Mesh>(mesh));
		}

		LocalFunctions::CenterMeshGroup(result.Meshes);

		file->Close();

		return result;
	}
}