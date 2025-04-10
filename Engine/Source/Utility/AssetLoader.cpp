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
		if (!Pawn::Core::IO::PFileExists(filePath))
		{
			PE_ERROR(TEXT("File {0} not found!"), filePath);
			return AssetLoadResult();
		}

		auto file = Pawn::Core::IO::POpenFile(filePath);
		if (!file || !file->IsOpen())
		{
			PE_ERROR(TEXT("File is not opened! Error: {0}"),
				file ? (uint32)file->GetFileLastError() : 0);
			return AssetLoadResult();
		}

		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32> positions(1000);
		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector2D32> uvCoords(1000);
		Pawn::Core::Containers::Array<Pawn::Core::Math::Vector3D32> normals(1000);

		Pawn::Core::Containers::UnorderedMap<VertexKey, uint32, VertexKeyHash> vertexMap(1000);
		Pawn::Core::Containers::Array<Assets::Vertex> vertices(1000);

		struct GroupEntry
		{
			Pawn::Core::Containers::String name;
			Pawn::Core::Containers::Array<int32> groupIndices;
			Pawn::Core::Containers::String material;

			GroupEntry(const Pawn::Core::Containers::String& n)
				: name(n), groupIndices(1000) {
			}
		};
		Pawn::Core::Containers::Array<GroupEntry> groups;
		groups.Reserve(10);

		SIZE_T currentGroupIdx = 0;
		Pawn::Core::Containers::String buf;

		while (!file->Eof())
		{
			if (!file->Read(buf, Pawn::Core::IO::StringReadMode::Line))
			{
				if (!file->Eof())
					PE_ERROR(TEXT("Failed file reading! Error: {0}"),
						(int32)file->GetFileLastError());
				break;
			}

			if (buf.Empty() || buf[0] == TEXT('#'))
				continue;

			const uchar* line = buf.GetString();
			SIZE_T pos = 0;

			if (line[0] == TEXT('v'))
			{
				const uchar* data = line + 2;
				if (line[1] == TEXT(' ') || line[1] == TEXT('\0'))
				{
					float32 x = (float32)Pawn::Core::Containers::StringToFloat(data, &pos);
					float32 y = (float32)Pawn::Core::Containers::StringToFloat(data + pos, &pos);
					float32 z = (float32)Pawn::Core::Containers::StringToFloat(data + pos, &pos);
					if (pos == 0)
					{
						PE_ERROR(TEXT("Invalid position data in line: {0}"), line);
						continue;
					}
					positions.EmplaceBack(x, y, z);
				}
				else if (line[1] == TEXT('t'))
				{
					float32 u = (float32)Pawn::Core::Containers::StringToFloat(data, &pos);
					float32 v = (float32)Pawn::Core::Containers::StringToFloat(data + pos, &pos);
					if (pos == 0)
					{
						PE_ERROR(TEXT("Invalid texture coordinate data in line: {0}"), line);
						continue;
					}
					uvCoords.EmplaceBack(u, v);
				}
				else if (line[1] == TEXT('n'))
				{
					float32 x = (float32)Pawn::Core::Containers::StringToFloat(data, &pos);
					float32 y = (float32)Pawn::Core::Containers::StringToFloat(data + pos, &pos);
					float32 z = (float32)Pawn::Core::Containers::StringToFloat(data + pos, &pos);
					if (pos == 0)
					{
						PE_ERROR(TEXT("Invalid normal data in line: {0}"), line);
						continue;
					}
					normals.EmplaceBack(x, y, z);
				}
			}
			else if (line[0] == TEXT('g'))
			{
				const uchar* groupStart = line + 2;
				Pawn::Core::Containers::String groupName(groupStart);
				if (!groupName.Empty())
				{
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
			}
			else if (line[0] == TEXT('f'))
			{
				const uchar* posB = line + 1;
				bool validFace = true;

				for (int i = 0; i < 3 && validFace; ++i)
				{
					pos = 0;
					int32 p = (int32)Pawn::Core::Containers::StringToUint(posB, &pos) - 1;
					posB += pos + 1;
					int32 t = (int32)Pawn::Core::Containers::StringToUint(posB, &pos) - 1;
					posB += pos + 1;
					int32 n = (int32)Pawn::Core::Containers::StringToUint(posB, &pos) - 1;
					posB += pos;

					if (pos == 0 || p < 0 || t < 0 || n < 0 ||
						p >= positions.GetSize() || t >= uvCoords.GetSize() || n >= normals.GetSize())
					{
						PE_ERROR(TEXT("Invalid face data in line: {0}"), line);
						validFace = false;
						break;
					}

					VertexKey key(p, t, n);
					uint32 index;
					auto it = vertexMap.Find(key);

					if (it != vertexMap.End())
					{
						index = it->Value2;
					}
					else
					{
						Assets::Vertex vertex;
						vertex.x = positions[p].X;
						vertex.y = positions[p].Y;
						vertex.z = positions[p].Z;
						vertex.uvU = uvCoords[t].X;
						vertex.uvV = uvCoords[t].Y;
						vertex.normalX = normals[n].X;
						vertex.normalY = normals[n].Y;
						vertex.normalZ = normals[n].Z;

						index = static_cast<uint32>(vertices.GetSize());
						vertices.PushBack(vertex);
						vertexMap.Insert(key, index);
					}

					groups[currentGroupIdx].groupIndices.EmplaceBack(index);
				}
			}
		}

		AssetLoadResult resultLoad;
		resultLoad.Meshes.Reserve(groups.GetSize());

		for (auto& group : groups)
		{
			if (!group.groupIndices.Empty())
			{
				auto mesh = Pawn::Core::Memory::Reference<Assets::Mesh>(
					new Assets::Mesh(group.name));

				Pawn::Core::Containers::Array<Assets::Vertex> groupVertices;
				groupVertices.Reserve(group.groupIndices.GetSize());

				Pawn::Core::Containers::UnorderedMap<uint32, uint32> indexMap;
				indexMap.Reserve(group.groupIndices.GetSize());

				for (SIZE_T i = 0; i < group.groupIndices.GetSize(); ++i)
				{
					uint32 oldIndex = group.groupIndices[i];
					uint32 newIndex;

					auto it = indexMap.Find(oldIndex);
					if (it == indexMap.End())
					{
						newIndex = static_cast<uint32>(groupVertices.GetSize());
						groupVertices.PushBack(vertices[oldIndex]);
						indexMap.Insert(oldIndex, newIndex);
					}
					else
					{
						newIndex = it->Value2;
					}

					group.groupIndices[i] = newIndex;
				}

				mesh->SetVertexes(std::move(groupVertices));
				mesh->SetIndexes(std::move(group.groupIndices));
				resultLoad.Meshes.EmplaceBack(std::move(mesh));
			}
		}

		for (const auto& mesh : resultLoad.Meshes)
		{
			PE_INFO(TEXT("Mesh {0} read!"), mesh->GetGroupName().GetData());
		}

		return resultLoad;
	}
}