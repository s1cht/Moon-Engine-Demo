#include "AssetLoader.h"

#include <Core/Math/MathMacros.h>

import Pawn.Core.Math;
import Pawn.Core.IO;
import Pawn.Core.Algorithm;
import Pawn.Core.Container.String;
import Pawn.Core.Container.UnorderedMap;

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
						i += (int32)splitStr.GetSize() - 1;
					}
					else
					{
						if constexpr (sizeof(Pawn::Core::Containers::String::DataType) == 1)
							out.EmplaceBack("");
						else if constexpr (sizeof(Pawn::Core::Containers::String::DataType) == 2)
							out.EmplaceBack(TEXT(""));
						else
							PE_ASSERT(false, TEXT("Unsupported string!"));
					}
				}
				else if (i + splitStr.GetSize() >= str.GetSize())
				{
					temp += str.Substring(i, splitStr.GetSize());
					out.EmplaceBack(temp);
					break;
				}
				else
				{
					temp += str[i];
				}
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

		inline void GenVerticesFromRawOBJ(Pawn::Core::Containers::Array<Pawn::Assets::Vertex>& vertices,
			const Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32>& positions,
			const Pawn::Core::Containers::Array<Pawn::Core::Math::Vector2D32>& textureCoords,
			const Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32>& normals,
			Pawn::Core::Containers::String currentLine)
		{
			Pawn::Core::Containers::Array<Pawn::Core::Containers::String> sface, svert;
			Pawn::Assets::Vertex vertexTemp;
			LocalFunctions::Split(LocalFunctions::GetTokenValues(currentLine), sface, Pawn::Core::Containers::String(TEXT(" ")));

			bool noNormal = false;

			for (int32 i = 0; i < (int)sface.GetSize(); i++)
			{
				int32 vtype;

				LocalFunctions::Split(sface[i], svert, Pawn::Core::Containers::String(TEXT("/")));

				if (svert.GetSize() == 1)
					vtype = 1;

				if (svert.GetSize() == 2)
					vtype = 2;

				if (svert.GetSize() == 3)
				{
					if (svert[1] != TEXT(""))
						vtype = 4;
					else
						vtype = 3;
				}

				switch (vtype)
				{
				case 1:
				{
					vertexTemp.Position = LocalFunctions::FindElement(positions, svert[0]);
					vertexTemp.TextureCoordinate = Pawn::Core::Math::Vector2D32(0, 0);
					noNormal = true;
					vertices.EmplaceBack(vertexTemp);
					break;
				}
				case 2:
				{
					vertexTemp.Position = LocalFunctions::FindElement(positions, svert[0]);
					vertexTemp.TextureCoordinate = LocalFunctions::FindElement(textureCoords, svert[1]);
					noNormal = true;
					vertices.EmplaceBack(vertexTemp);
					break;
				}
				case 3:
				{
					vertexTemp.Position = LocalFunctions::FindElement(positions, svert[0]);
					vertexTemp.TextureCoordinate = Pawn::Core::Math::Vector2D32(0, 0);
					vertexTemp.Normal = LocalFunctions::FindElement(normals, svert[2]);
					vertices.EmplaceBack(vertexTemp);
					break;
				}
				case 4:
				{
					vertexTemp.Position = LocalFunctions::FindElement(positions, svert[0]);
					vertexTemp.TextureCoordinate = LocalFunctions::FindElement(textureCoords, svert[1]);
					vertexTemp.Normal = LocalFunctions::FindElement(normals, svert[2]);
					vertices.EmplaceBack(vertexTemp);
					break;
				}
				default:
					break;
				}
			}

			if (noNormal)
			{
				Pawn::Core::Math::Vector3D32 A = vertices[0].Position - vertices[1].Position;
				Pawn::Core::Math::Vector3D32 B = vertices[2].Position - vertices[1].Position;

				Pawn::Core::Math::Vector3D32 normal = A.Cross(B);

				for (int32 i = 0; i < (int32)vertices.GetSize(); i++)
					vertices[i].Normal = normal;
			}	
		}

		inline void VertexTriangluation(Pawn::Core::Containers::Array<int32>& oIndices,
			const Pawn::Core::Containers::Array<Pawn::Assets::Vertex>& iVerts)
		{
			if (iVerts.GetSize() < 3)
				return;
			
			if (iVerts.GetSize() == 3)
			{
				oIndices.EmplaceBack(0);
				oIndices.EmplaceBack(1);
				oIndices.EmplaceBack(2);
				return;
			}

			Pawn::Core::Containers::Array<Pawn::Assets::Vertex> tVerts = iVerts;

			while (true)
			{
				for (int32 i = 0; i < (int32)tVerts.GetSize(); i++)
				{
					Pawn::Assets::Vertex pPrev;
					Pawn::Assets::Vertex pNext;
					Pawn::Assets::Vertex pCur = tVerts[i];

					if (i == 0)
						pPrev = tVerts[tVerts.GetSize() - 1];
					else
						pPrev = tVerts[i - 1];

					if (i == tVerts.GetSize() - 1)
						pNext = tVerts[0];
					else
						pNext = tVerts[i + 1];

					if (tVerts.GetSize() == 3)
					{
						for (int32 j = 0; j < (int32)tVerts.GetSize(); j++)
						{
							if (iVerts[j].Position == pCur.Position)
								oIndices.EmplaceBack(j);
							if (iVerts[j].Position == pPrev.Position)
								oIndices.EmplaceBack(j);
							if (iVerts[j].Position == pNext.Position)
								oIndices.EmplaceBack(j);
						}

						tVerts.Clear();
						break;
					}

					if (tVerts.GetSize() == 4)
					{
						for (int32 j = 0; j < (int32)iVerts.GetSize(); j++)
						{
							if (iVerts[j].Position == pCur.Position)
								oIndices.EmplaceBack(j);
							if (iVerts[j].Position == pPrev.Position)
								oIndices.EmplaceBack(j);
							if (iVerts[j].Position == pNext.Position)
								oIndices.EmplaceBack(j);
						}

						Pawn::Core::Math::Vector3D32 tempVec;
						for (int32 j = 0; j < (int32)tVerts.GetSize(); j++)
						{
							if (tVerts[j].Position != pCur.Position
								&& tVerts[j].Position != pPrev.Position
								&& tVerts[j].Position != pNext.Position)
							{
								tempVec = tVerts[j].Position;
								break;
							}
						}

						for (int32 j = 0; j < (int32)iVerts.GetSize(); j++)
						{
							if (iVerts[j].Position == pPrev.Position)
								oIndices.EmplaceBack(j);
							if (iVerts[j].Position == pNext.Position)
								oIndices.EmplaceBack(j);
							if (iVerts[j].Position == tempVec)
								oIndices.EmplaceBack(j);
						}

						tVerts.Clear();
						break;
					}

					float32 angle = (float32)((pPrev.Position - pCur.Position).GetAngleBetweenVectors(pNext.Position - pCur.Position) * 180 / PI);
					if (angle <= 0 && angle >= 180)
						continue;

					bool inTri = false;
					for (int32 j = 0; j < (int32)iVerts.GetSize(); j++)
					{
						if (LocalFunctions::IsInTriangle(iVerts[j].Position, pPrev.Position, pCur.Position, pNext.Position)
							&& iVerts[j].Position != pPrev.Position
							&& iVerts[j].Position != pCur.Position
							&& iVerts[j].Position != pNext.Position)
						{
							inTri = true;
							break;
						}
					}
					if (inTri)
						continue;

					for (int32 j = 0; j < (int32)iVerts.GetSize(); j++)
					{
						if (iVerts[j].Position == pCur.Position)
							oIndices.EmplaceBack(j);
						if (iVerts[j].Position == pPrev.Position)
							oIndices.EmplaceBack(j);
						if (iVerts[j].Position == pNext.Position)
							oIndices.EmplaceBack(j);
					}

					for (int32 j = 0; j < (int32)tVerts.GetSize(); j++)
					{
						if (tVerts[j].Position == pCur.Position)
						{
							tVerts.Erase(tVerts.Begin() + j);
							break;
						}
					}

					i = -1;
				}

				if (oIndices.GetSize() == 0)
					break;

				if (tVerts.GetSize() == 0)
					break;
			}
		}
	}

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

		result.Meshes = Pawn::Core::Containers::Array<Pawn::Core::Memory::Reference<Assets::Mesh>>(10);

		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32> positions(1000);
		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector2D32> uvCoords(1000);
		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32> normals(1000);

		Pawn::Core::Memory::Reference<Pawn::Assets::Mesh> mesh;
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
						mesh = Pawn::Core::Memory::Reference<Pawn::Assets::Mesh>(new Pawn::Assets::Mesh());
						mesh->SetVertexes(vertices);
						mesh->SetIndexes(indices);
						mesh->SetGroupName(meshName);
						mesh->CreateBuffers();
						result.Meshes.EmplaceBack(mesh);

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
				LocalFunctions::GenVerticesFromRawOBJ(tempVertices, positions, uvCoords, normals, readStr);
				
				for (int32 i = 0; i < (int32)tempVertices.GetSize(); i++)
					vertices.PushBack(tempVertices[i]);

				LocalFunctions::VertexTriangluation(indices, tempVertices);

				for (int i = 0; i < int(tempIndices.GetSize()); i++)
				{
					indices.EmplaceBack((int32)((vertices.GetSize()) - tempVertices.GetSize()) + tempIndices[i]);
				}
			}
		}
		if (!indices.Empty() && !vertices.Empty())
		{
			mesh = Pawn::Core::Memory::Reference<Pawn::Assets::Mesh>(new Pawn::Assets::Mesh());
			mesh->SetVertexes(vertices);
			mesh->SetIndexes(indices);
			mesh->SetGroupName(meshName);
			mesh->CreateBuffers();
			result.Meshes.EmplaceBack(mesh);
		}

		file->Close();

		return result;
	}
}