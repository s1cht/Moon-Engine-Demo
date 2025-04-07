#include "AssetLoader.h"

import Pawn.Core.Math;
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

		file = Pawn::Core::IO::POpenFile(filePath);
		result = file->IsOpen();
		if (!result)
		{
			PE_ERROR(TEXT("File is not opened! Error: {0}"), (uint32)file->GetFileLastError());
			return AssetLoadResult();
		}

		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32> positions;
		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector2D32> uvCoords;
		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32> normals;

		positions.Reserve(1000);
		uvCoords.Reserve(1000);
		normals.Reserve(1000);

		Pawn::Core::Containers::UnorderedMap<VertexKey, uint32, VertexKeyHash> vertexMap;
		Pawn::Core::Containers::Array<Assets::Vertex> vertices;

		vertexMap.Reserve(1000); 
		vertices.Reserve(1000);

		struct GroupEntry 
		{
			Pawn::Core::Containers::String name;
			Pawn::Core::Containers::Array<int32> groupIndices;
			Pawn::Core::Containers::String material;

			GroupEntry(const Pawn::Core::Containers::String& n) : name(n) {}
		};
		Pawn::Core::Containers::Array<GroupEntry> groups;

		SIZE_T currentGroupIdx = 0;

		Pawn::Core::Containers::String buf;
		SIZE_T pos = 0;

		while (!file->Eof())
		{
			result = file->Read(buf, Pawn::Core::IO::StringReadMode::Line);
			if (!result)
			{
				if (!file->Eof())
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
					float32 x = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + 2, &pos);
					float32 y = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + pos, &pos);
					float32 z = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + pos, &pos);
					if (pos == 0)
					{
						PE_ERROR(TEXT("Invalid position data in line: {0}"), buf.GetString());
						continue;
					}
					positions.EmplaceBack(x, y, z);
				}
				// Texture
				else if (subStr[1] == TEXT('t'))
				{
					pos = 0;
					float32 u = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + 2, &pos);
					float32 v = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + pos, &pos);
					if (pos == 0)
					{
						PE_ERROR(TEXT("Invalid texture coordinate data in line: {0}"), buf.GetString());
						continue;
					}
					uvCoords.EmplaceBack(u, v);
				}
				// Normals
				else if (subStr[1] == TEXT('n'))
				{
					pos = 0;
					float32 x = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + 2, &pos);
					float32 y = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + pos, &pos);
					float32 z = (float32)Pawn::Core::Containers::StringToFloat(buf.GetString() + pos, &pos);
					if (pos == 0)
					{
						PE_ERROR(TEXT("Invalid normal data in line: {0}"), buf.GetString());
						continue;
					}
					normals.EmplaceBack(x, y, z);
				}
			}
			else if (subStr[0] == TEXT('g'))
			{
				pos = 2;
				Pawn::Core::Containers::String groupName = Pawn::Core::Containers::String(buf.GetString() + pos);
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

					if (pos == 0)
					{
						PE_ERROR(TEXT("Invalid face data in line: {0}"), buf.GetString());
						continue;
					}

					VertexKey key(p, t, n);
					uint32 index = 0;
					bool found = false;

					auto it = vertexMap.Find(key);
					if (it != vertexMap.End())
					{
						index = it->Value2;
						found = true;
					}

					if (!found)
					{
						Assets::Vertex face;
						face.x = positions[p].X;
						face.y = positions[p].Y;
						face.z = positions[p].Z;
						face.uvU = uvCoords[t].X;
						face.uvV = uvCoords[t].Y;
						face.uvW = 0.0f;
						face.normalX = normals[n].X;
						face.normalY = normals[n].Y;
						face.normalZ = normals[n].Z;

						index = static_cast<uint32>(vertices.GetSize());
						vertices.PushBack(face);
						vertexMap.Insert(key, index);
					}

					groups[currentGroupIdx].groupIndices.EmplaceBack(index);
				}
			}
		}

		AssetLoadResult resultLoad;
		for (SIZE_T i = 0; i < groups.GetSize(); ++i)
		{
			auto mesh = Pawn::Core::Memory::Reference<Assets::Mesh>(new Assets::Mesh(std::move(groups[i].name)));
			mesh->SetVertexes(std::move(vertices));
			mesh->SetIndexes(std::move(groups[i].groupIndices));
			resultLoad.Meshes.EmplaceBack(std::move(mesh));
		}

		for (auto mesh = resultLoad.Meshes.Begin(); mesh != resultLoad.Meshes.End(); mesh++)
			PE_INFO(TEXT("Mesh {0} read!"), (*mesh)->GetGroupName().GetData());

		return resultLoad;
	}
}