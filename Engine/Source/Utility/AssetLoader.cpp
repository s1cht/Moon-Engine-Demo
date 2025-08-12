#include "AssetLoader.h"

#include <Core/Containers/Tables/UnorderedMap.hpp>
#include <Core/Platform/Base/IO.hpp>

#define DEBUG_MESH
//#define DEBUG_INDICES

namespace ME::Utility
{
	namespace LocalFunctions
	{
		void Split(const ME::Core::Containers::String& str,
		           ME::Core::Containers::Array<ME::Core::Containers::String>& out,
		           ME::Core::Containers::String splitStr)
		{
			out.Clear();

			ME::Core::Containers::String temp;

			for (int32 i = 0; i < static_cast<int32>(str.GetSize()); i++)
			{
				ME::Core::Containers::String test = str.Substring(i, splitStr.GetSize());

				if (test == splitStr)
				{
					if (!temp.Empty())
					{
						out.EmplaceBack(temp);
						temp.Clear();
					}
					i += static_cast<int32>(splitStr.GetSize()) - 1;
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

		bool IsOnSameSide(ME::Core::Math::Vector3D32 p1, ME::Core::Math::Vector3D32 p2,
		                  ME::Core::Math::Vector3D32 a, ME::Core::Math::Vector3D32 b)
		{
			ME::Core::Math::Vector3D32 cp1 = (b - a).Cross(p1 - a);
			ME::Core::Math::Vector3D32 cp2 = (b - a).Cross(p2 - a);

			if (cp1.Dot(cp2) >= 0)
				return true;
			else
				return false;
		}

		ME::Core::Math::Vector3D32 GenerateTriangleNormal(ME::Core::Math::Vector3D32 t1,
		                                                  ME::Core::Math::Vector3D32 t2,
		                                                  ME::Core::Math::Vector3D32 t3)
		{
			ME::Core::Math::Vector3D32 u = t2 - t1;
			ME::Core::Math::Vector3D32 v = t3 - t1;

			ME::Core::Math::Vector3D32 normal = u.Cross(v);

			return normal;
		}

		bool IsInTriangle(ME::Core::Math::Vector3D32 point,
		                  ME::Core::Math::Vector3D32 tri1, ME::Core::Math::Vector3D32 tri2,
		                  ME::Core::Math::Vector3D32 tri3)
		{
			bool within_tri_prisim = IsOnSameSide(point, tri1, tri2, tri3) && IsOnSameSide(point, tri2, tri1, tri3)
				&& IsOnSameSide(point, tri3, tri1, tri2);

			if (!within_tri_prisim)
				return false;

			ME::Core::Math::Vector3D32 n = GenerateTriangleNormal(tri1, tri2, tri3);

			ME::Core::Math::Vector3D32 proj = point.Project(n);

			if (proj.Length() == 0)
				return true;
			else
				return false;
		}

		ME::Core::Containers::String GetTokenValues(const ME::Core::Containers::String& in)
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

		inline ME::Core::Containers::String GetToken(const ME::Core::Containers::String& in)
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
		inline const T& FindElement(const ME::Core::Containers::Array<T>& elements, ME::Core::Containers::String& index)
		{
			int32 idx = static_cast<int32>(ME::Core::Containers::StringToInt(index, nullptr));
			if (idx == 0)
			{
				// Invalid in OBJ (1-based)
				ME_ERROR(TEXT("Invalid vertex index 0 in OBJ file."));
				static T defaultVal{}; // Return default or throw
				return defaultVal;
			}
			if (idx < 0)
				idx = static_cast<int32>(elements.GetSize()) + idx;
			else
				idx--;
			if (idx < 0 || idx >= static_cast<int32>(elements.GetSize()))
			{
				ME_ERROR(TEXT("Vertex index out of bounds: {} (array size: {})"), idx + 1, elements.GetSize());
				static T defaultVal{}; // Return default or throw
				return defaultVal;
			}
			return elements[idx];
		}

		inline void GenVerticesFromRawOBJ(
			ME::Core::Containers::Array<ME::Assets::Vertex>& vertices,
			ME::Core::Containers::Array<int32>& indices,
			const ME::Core::Containers::Array<ME::Core::Math::Vector3D32>& positions,
			const ME::Core::Containers::Array<ME::Core::Math::Vector2D32>& textureCoords,
			const ME::Core::Containers::Array<ME::Core::Math::Vector3D32>& normals,
			ME::Core::Containers::String currentLine,
			ME::Core::Containers::UnorderedMap<ME::Core::Containers::String, int32>& vertexMap,
			// Passed by ref for global use
			bool& noNormal)
		{
			indices.Clear(); // Only clear indices, vertices are accumulated

			ME::Core::Containers::Array<ME::Core::Containers::String> sface;
			LocalFunctions::Split(LocalFunctions::GetTokenValues(currentLine), sface,
			                      ME::Core::Containers::String(TEXT(" ")));

			noNormal = false;

			for (int32 i = 0; i < static_cast<int32>(sface.GetSize()); i++)
			{
				auto it = vertexMap.Find(sface[i]);
				if (it != vertexMap.End())
				{
					indices.PushBack(it->Value2);
					continue;
				}

				ME::Core::Containers::Array<ME::Core::Containers::String> svert;
				LocalFunctions::Split(sface[i], svert, ME::Core::Containers::String(TEXT("/")));

				int32 vtype;
				if (svert.GetSize() == 1)
					vtype = 1;
				else if (svert.GetSize() == 2)
					vtype = 2;
				else if (svert.GetSize() == 3)
					vtype = svert[1] != TEXT("") ? 4 : 3;
				else
					continue;

				ME::Assets::Vertex vertexTemp;
				switch (vtype)
				{
				case 1:
					{
						vertexTemp.Position = LocalFunctions::FindElement(positions, svert[0]);
						vertexTemp.TextureCoordinate = ME::Core::Math::Vector2D32(0, 0);
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
						vertexTemp.TextureCoordinate = ME::Core::Math::Vector2D32(0, 0);
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
		}

		// Ear clipping triangulation for handling concave polygons
		inline void VertexTriangluation(
			ME::Core::Containers::Array<int32>& oIndices,
			const ME::Core::Containers::Array<int32>& iIndices,
			const ME::Core::Containers::Array<ME::Assets::Vertex>& iVerts)
		{
			oIndices.Clear();
			if (iIndices.GetSize() < 3)
				return;

			// Simple fan for triangles
			if (iIndices.GetSize() == 3)
			{
				oIndices.EmplaceBack(iIndices[0]);
				oIndices.EmplaceBack(iIndices[1]);
				oIndices.EmplaceBack(iIndices[2]);
				return;
			}

			// Ear clipping for polygons
			ME::Core::Containers::Array<int32> remainingIndices = iIndices; // Copy
			while (remainingIndices.GetSize() > 3)
			{
				bool earFound = false;
				for (int32 i = 0; i < static_cast<int32>(remainingIndices.GetSize()); ++i)
				{
					int32 prev = (i == 0) ? remainingIndices.GetSize() - 1 : i - 1;
					int32 next = (i + 1) % remainingIndices.GetSize();

					ME::Core::Math::Vector3D32 vPrev = iVerts[remainingIndices[prev]].Position;
					ME::Core::Math::Vector3D32 vCurr = iVerts[remainingIndices[i]].Position;
					ME::Core::Math::Vector3D32 vNext = iVerts[remainingIndices[next]].Position;

					// Check if ear (convex and no points inside)
					ME::Core::Math::Vector3D32 edge1 = vCurr - vPrev;
					ME::Core::Math::Vector3D32 edge2 = vNext - vCurr;
					if (edge1.Cross(edge2).Z < 0) continue; // Concave vertex, skip

					bool isEar = true;
					for (int32 j = 0; j < static_cast<int32>(remainingIndices.GetSize()); ++j)
					{
						if (j == prev || j == i || j == next) continue;
						ME::Core::Math::Vector3D32 point = iVerts[remainingIndices[j]].Position;
						if (IsInTriangle(point, vPrev, vCurr, vNext))
						{
							isEar = false;
							break;
						}
					}

					if (isEar)
					{
						// Add triangle
						oIndices.EmplaceBack(remainingIndices[prev]);
						oIndices.EmplaceBack(remainingIndices[i]);
						oIndices.EmplaceBack(remainingIndices[next]);

						// Remove ear vertex
						remainingIndices.Erase(remainingIndices.Begin() + i);
						earFound = true;
						break;
					}
				}
				if (!earFound)
				{
					// Fallback to fan if ear clipping fails (e.g., degenerate)
					for (int32 k = 1; k < static_cast<int32>(iIndices.GetSize()) - 1; ++k)
					{
						oIndices.EmplaceBack(iIndices[0]);
						oIndices.EmplaceBack(iIndices[k]);
						oIndices.EmplaceBack(iIndices[k + 1]);
					}
					return;
				}
			}
			// Last triangle
			oIndices.EmplaceBack(remainingIndices[0]);
			oIndices.EmplaceBack(remainingIndices[1]);
			oIndices.EmplaceBack(remainingIndices[2]);
		}

		inline void CenterMesh(ME::Core::Containers::Array<ME::Assets::Vertex>& vertices)
		{
			if (vertices.GetSize() == 0)
				return;

			ME::Core::Math::Vector3D32 centroid(0.0f, 0.0f, 0.0f);
			for (const auto& vertex : vertices)
			{
				centroid += vertex.Position;
			}
			centroid /= static_cast<float32>(vertices.GetSize());

			for (auto& vertex : vertices)
			{
				vertex.Position -= centroid;
			}
		}

		inline void CenterMeshGroup(ME::Core::Containers::Array<ME::Core::Memory::Reference<ME::Assets::Mesh>>& meshes)
		{
			if (meshes.Empty())
				return;

			int32 totalVertices = 0;
			ME::Core::Math::Vector3D32 centroid(0.0f, 0.0f, 0.0f);
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

			centroid /= static_cast<float32>(totalVertices);

			for (auto& mesh : meshes)
			{
				for (auto& vertex : mesh->GetVertices())
				{
					vertex.Position -= centroid;
				}
			}

			// Optional verification (can remove for production)
			ME::Core::Math::Vector3D32 newCentroid(0.0f, 0.0f, 0.0f);
			int32 newTotalVertices = 0;
			for (const auto& mesh : meshes)
			{
				newTotalVertices += mesh->GetVertices().GetSize();
				for (const auto& vertex : mesh->GetVertices())
				{
					newCentroid += vertex.Position;
				}
			}
			newCentroid /= static_cast<float32>(newTotalVertices);
		}
	}

	AssetLoadResult AssetLoader::Load(const ME::Core::Containers::String& filePath, bool centered,
	                                  AssetFileFormats format)
	{
		switch (format)
		{
		case AssetFileFormats::OBJ:
			return LoadOBJ(filePath.GetString(), centered);
		default:
			ME_ERROR("Unsupported asset format!: {}", static_cast<uint32>(format));
			return AssetLoadResult();
		}
	}

	AssetLoadResult AssetLoader::LoadOBJ(const uchar* filePath, bool centered)
	{
		if (!ME::Core::IO::PFileExists(filePath))
		{
			ME_ERROR(TEXT("File {0} not found!"), filePath);
			return AssetLoadResult();
		}

		ME::Core::Memory::Reference<ME::Core::IO::File> file;

		file = ME::Core::IO::POpenFile(filePath);
		if (!file || !file->IsOpen())
		{
			ME_ERROR(TEXT("File is not opened! Error: {0}"),
			         file ? static_cast<uint32>(file->GetFileLastError()) : 0);
			return AssetLoadResult();
		}

		bool readingStarted = false;
		AssetLoadResult result;

		result.Meshes = ME::Core::Containers::Array<ME::Core::Memory::Reference<Assets::Mesh>>();

		ME::Core::Containers::Array<ME::Core::Math::Vector3D32> positions(1000);
		ME::Core::Containers::Array<ME::Core::Math::Vector2D32> uvCoords(1000);
		ME::Core::Containers::Array<ME::Core::Math::Vector3D32> normals(1000);

		ME::Assets::Mesh* mesh;
		ME::Core::Containers::String meshName;
		ME::Core::Containers::Array<Assets::Vertex> vertices(1000);
		ME::Core::Containers::Array<int32> indices(1000);
		ME::Core::Containers::UnorderedMap<ME::Core::Containers::String, int32> vertexMap;

		ME::Core::Containers::String readStr;
		while (file->Read(readStr, ME::Core::IO::StringReadMode::Line))
		{
			ME::Core::Containers::String token = LocalFunctions::GetToken(readStr);
			if (token == TEXT("o") || token == TEXT("g") || token[0] == TEXT('g'))
			{
				if (!readingStarted)
				{
					readingStarted = true;

					if (token == TEXT("o") || token == TEXT("g"))
					{
						meshName = LocalFunctions::GetTokenValues(readStr);
						meshName = meshName.Substring(0, meshName.GetSize() - 1);
					}
					else
					{
						meshName = TEXT("unnamed");
					}
				}
				else
				{
					if (!indices.Empty() && !vertices.Empty())
					{
						mesh = new ME::Assets::Mesh();

#ifdef DEBUG_MESH
						ME_INFO(TEXT("MESH {0}"), meshName.GetString());
#ifdef DEBUG_VERTICES
                        for (const auto& vert : vertices)
                        {
                            ME_INFO(TEXT("Vertex position: {0}, {1}, {2}; UV: {3}, {4}; Normal: {5}, {6}, {7}"),
                                vert.Position.X, vert.Position.Y, vert.Position.Z,
                                vert.TextureCoordinate.X, vert.TextureCoordinate.Y,
                                vert.Normal.X, vert.Normal.Y, vert.Normal.Z
                            );
                        }
#endif
#ifdef DEBUG_INDICES
                        int32 i = 0;
                        ME::Core::Containers::String temp = TEXT("");
                        for (const auto& ind : indices)
                        {
                            i++;
                            temp += TEXT(" ") + ME::Core::Containers::ToString(ind);
                            if (i == 3)
                            {
                                ME_INFO(TEXT("Vertex indices: {0}"), temp.GetString());
                                temp = TEXT(" ");
                                i = 0;
                            }
                        }
#endif
#endif
						mesh->SetVertices(vertices);
						mesh->SetIndices(indices);
						mesh->SetGroupName(meshName);
						result.Meshes.EmplaceBack(ME::Core::Memory::Reference<ME::Assets::Mesh>(mesh));

						vertices.Clear();
						indices.Clear();
						vertexMap.Clear(); // Reset per mesh
						meshName.Clear();
					}

					if (token == TEXT("o") || token == TEXT("g"))
					{
						meshName = LocalFunctions::GetTokenValues(readStr);
						meshName = meshName.Substring(0, meshName.GetSize() - 1);
					}
					else
					{
						meshName = TEXT("unnamed");
					}
				}
			}

			if (token == TEXT("v"))
			{
				ME::Core::Containers::Array<ME::Core::Containers::String> spos;
				ME::Core::Math::Vector3D32 vpos;
				LocalFunctions::Split(LocalFunctions::GetTokenValues(readStr), spos,
				                      ME::Core::Containers::String(TEXT(" ")));

				vpos.X = static_cast<float32>(ME::Core::Containers::StringToFloat(spos[0], nullptr));
				vpos.Y = static_cast<float32>(ME::Core::Containers::StringToFloat(spos[1], nullptr));
				vpos.Z = static_cast<float32>(ME::Core::Containers::StringToFloat(spos[2], nullptr));

				positions.EmplaceBack(vpos);
			}
			if (token == TEXT("vt"))
			{
				ME::Core::Containers::Array<ME::Core::Containers::String> stex;
				ME::Core::Math::Vector2D32 vtex;
				LocalFunctions::Split(LocalFunctions::GetTokenValues(readStr), stex,
				                      ME::Core::Containers::String(TEXT(" ")));

				vtex.X = static_cast<float32>(ME::Core::Containers::StringToFloat(stex[0], nullptr));
				vtex.Y = static_cast<float32>(ME::Core::Containers::StringToFloat(stex[1], nullptr));

				uvCoords.EmplaceBack(vtex);
			}
			if (token == TEXT("vn"))
			{
				ME::Core::Containers::Array<ME::Core::Containers::String> snor;
				ME::Core::Math::Vector3D32 vnor;
				LocalFunctions::Split(LocalFunctions::GetTokenValues(readStr), snor,
				                      ME::Core::Containers::String(TEXT(" ")));

				vnor.X = static_cast<float32>(ME::Core::Containers::StringToFloat(snor[0], nullptr));
				vnor.Y = static_cast<float32>(ME::Core::Containers::StringToFloat(snor[1], nullptr));
				vnor.Z = static_cast<float32>(ME::Core::Containers::StringToFloat(snor[2], nullptr));

				normals.EmplaceBack(vnor);
			}
			if (token == TEXT("f"))
			{
				ME::Core::Containers::Array<int32> faceIndices;
				bool faceNoNormal = false;
				LocalFunctions::GenVerticesFromRawOBJ(vertices, faceIndices, positions, uvCoords, normals, readStr,
				                                      vertexMap, faceNoNormal);

				ME::Core::Containers::Array<int32> triangulatedIndices;
				LocalFunctions::VertexTriangluation(triangulatedIndices, faceIndices, vertices);

				for (int32 i = 0; i < static_cast<int32>(triangulatedIndices.GetSize()); i++)
				{
					indices.EmplaceBack(triangulatedIndices[i]);
				}

				// If no normal for this face and generated per mesh, but now handle per triangle if needed
				if (faceNoNormal && triangulatedIndices.GetSize() >= 3)
				{
					for (int32 i = 0; i < static_cast<int32>(triangulatedIndices.GetSize()); i += 3)
					{
						int32 idx0 = triangulatedIndices[i];
						int32 idx1 = triangulatedIndices[i + 1];
						int32 idx2 = triangulatedIndices[i + 2];

						ME::Core::Math::Vector3D32 edge1 = vertices[idx1].Position - vertices[idx0].Position;
						ME::Core::Math::Vector3D32 edge2 = vertices[idx2].Position - vertices[idx0].Position;
						ME::Core::Math::Vector3D32 normal = edge1.Cross(edge2);
						if (normal.LengthSquared() > 1e-6f)
						{
							normal.Normalize();
							vertices[idx0].Normal = normal;
							vertices[idx1].Normal = normal;
							vertices[idx2].Normal = normal;
						}
						else
						{
							vertices[idx0].Normal = ME::Core::Math::Vector3D32(0, 0, 1);
							vertices[idx1].Normal = ME::Core::Math::Vector3D32(0, 0, 1);
							vertices[idx2].Normal = ME::Core::Math::Vector3D32(0, 0, 1);
						}
					}
				}
			}
		}
		if (!indices.Empty() && !vertices.Empty())
		{
			mesh = new ME::Assets::Mesh();
#ifdef DEBUG_MESH
			ME_INFO(TEXT("MESH {0}"), meshName.GetString());
#ifdef DEBUG_VERTICES
            for (const auto& vert : vertices)
            {
                ME_INFO(TEXT("Vertex position: {0}, {1}, {2}; UV: {3}, {4}; Normal: {5}, {6}, {7}"),
                    vert.Position.X, vert.Position.Y, vert.Position.Z,
                    vert.TextureCoordinate.X, vert.TextureCoordinate.Y,
                    vert.Normal.X, vert.Normal.Y, vert.Normal.Z
                );
            }
#endif
#ifdef DEBUG_INDICES
            int32 i = 0;
            ME::Core::Containers::String temp = TEXT("");
            for (const auto& ind : indices)
            {
                i++;
                temp += TEXT(" ") + ME::Core::Containers::ToString(ind);
                if (i == 3)
                {
                    ME_INFO(TEXT("Vertex indices: {0}"), temp.GetString());
                    temp = TEXT(" ");
                    i = 0;
                }
            }
#endif
#endif
			mesh->SetVertices(vertices);
			mesh->SetIndices(indices);
			mesh->SetGroupName(meshName);
			result.Meshes.EmplaceBack(ME::Core::Memory::Reference<ME::Assets::Mesh>(mesh));
		}

		if (centered)
		{
			LocalFunctions::CenterMeshGroup(result.Meshes);
		}

		file->Close();

		return result;
	}
}
