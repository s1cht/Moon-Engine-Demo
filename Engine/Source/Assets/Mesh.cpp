#include "Mesh.h"
#include <Core/Platform/Base/IO.h>

Pawn::Assets::Mesh::Mesh()
{

}

Pawn::Assets::Mesh::Mesh(const uchar* path, MeshFileFormats format)
{
	LoadFromFile(path, format);
}

Pawn::Assets::Mesh::Mesh(const String& path, MeshFileFormats format)
{
	LoadFromFile(path, format);
}

Pawn::Assets::Mesh::Mesh(String&& path, MeshFileFormats format)
{
	LoadFromFile(std::move(path), format);
}

Pawn::Assets::Mesh::~Mesh()
{

}

bool Pawn::Assets::Mesh::LoadFromFile(const uchar* path, MeshFileFormats format)
{
	switch (format)
	{
	case Pawn::Assets::MeshFileFormats::None:
	case Pawn::Assets::MeshFileFormats::Fbx:
	{
		return false;
		break;
	}
	case Pawn::Assets::MeshFileFormats::Obj:
	{
		LoadOBJ(path);
		return true;
		break;
	}
	default:
		return false;
		break;
	}
}

bool Pawn::Assets::Mesh::LoadFromFile(const String& path, MeshFileFormats format)
{
	switch (format)
	{
	case Pawn::Assets::MeshFileFormats::None:
	case Pawn::Assets::MeshFileFormats::Fbx:
	{
		return false;
		break;
	}
	case Pawn::Assets::MeshFileFormats::Obj:
	{
		LoadOBJ(path.GetString());
		return true;
		break;
	}
	default:
		return false;
		break;
	}
}

bool Pawn::Assets::Mesh::LoadFromFile(String&& path, MeshFileFormats format)
{
	switch (format)
	{
	case Pawn::Assets::MeshFileFormats::None:
	case Pawn::Assets::MeshFileFormats::Fbx:
	{
		return false;
		break;
	}
	case Pawn::Assets::MeshFileFormats::Obj:
	{
		LoadOBJ(path.GetString());
		return true;
		break;
	}
	default:
		return false;
		break;
	}
}

Pawn::Memory::Reference<Pawn::Render::VertexBuffer> Pawn::Assets::Mesh::GetVertexBuffer()
{
	return m_VertexBuffer;
}

Pawn::Memory::Reference<Pawn::Render::IndexBuffer> Pawn::Assets::Mesh::GetIndexBuffer()
{
	return m_IndexBuffer;
}

void Pawn::Assets::Mesh::LoadOBJ(const uchar* filePath)
{
	bool result;
	Memory::Reference<IO::File> file;

	result = IO::PFileExists(filePath);
	if (!result)
	{
		PE_ERROR(TEXT("File {0} not found!"), filePath);
		return;
	}

	Array<Math::Vector3D32> positions;
	Array<Math::Vector3D32> uvCoords;
	Array<Math::Vector3D32> normals;

	file = IO::POpenFile(filePath);

	result = file->IsOpen();
	if (!result)
	{
		PE_ERROR(TEXT("File is not opened! Error: {0}"), (uint32)file->GetFileLastError());
		return;
	}

	String buf;
	SIZE_T pos = 0;

	while (!file->Eof())
	{
		result = file->Read(buf, IO::StringReadMode::Line);

		if (!result)
		{
			PE_ERROR(TEXT("Failed file reading! Error: {0}"), (int32)file->GetFileLastError());
			break;
		}

		StringView subStr = StringView(buf).Substring(0, 2);

		if (subStr[0] == TEXT('v'))
		{
			// Position
			if (subStr[1] == TEXT('\0') || subStr[1] == TEXT(' '))
			{
				pos = 0;
				positions.EmplaceBack((float32)StringToFloat(buf.GetString() + 2, &pos), 0.f, 0.f);
				positions[positions.GetSize() - 1].Y = (float32)StringToFloat(buf.GetString() + pos, &pos);
				positions[positions.GetSize() - 1].Z = (float32)StringToFloat(buf.GetString() + pos, &pos);
			}
			// Texture
			else if (subStr[1] == TEXT('t'))
			{
				pos = 0;
				uvCoords.EmplaceBack((float32)StringToFloat(buf.GetString() + 2, &pos), 0.f, 0.f);
				uvCoords[uvCoords.GetSize() - 1].Y = (float32)StringToFloat(buf.GetString() + pos, &pos);
				uvCoords[uvCoords.GetSize() - 1].Z = (float32)StringToFloat(buf.GetString() + pos, &pos);
			}
			// Normals
			else if (subStr[1] == TEXT('n'))
			{
				pos = 0;
				normals.EmplaceBack((float32)StringToFloat(buf.GetString() + 2, &pos), 0.f, 0.f);
				normals[normals.GetSize() - 1].Y = (float32)StringToFloat(buf.GetString() + pos, &pos);
				normals[normals.GetSize() - 1].Z = (float32)StringToFloat(buf.GetString() + pos, &pos);
			}
		}
		// Face
		else if (subStr[0] == TEXT('f'))
		{
			pos = 0;

			int32 f1, f2, f3;
			Vertex face;
			uint32 p, t, n;
			const uchar* posB = buf.GetString() + 1;

			p = (int32)StringToUint(posB, &pos) - 1;
			posB += pos + 1;
			t = (int32)StringToUint(posB, &pos) - 1;
			posB += pos + 1;
			n = (int32)StringToUint(posB, &pos) - 1;
			
			face.x = positions[p].X;
			face.y = positions[p].Y;
			face.z = positions[p].Z;

			face.uvU = uvCoords[t].X;
			face.uvV = uvCoords[t].Y;
			face.uvW = uvCoords[t].Z;

			face.normalX = normals[n].X;
			face.normalY = normals[n].Y;
			face.normalZ = normals[n].Z;

			m_Vertexes.PushBack(face);
			m_Indexes.EmplaceBack(m_Vertexes.GetSize());

			posB += pos + 1;
			p = (int32)StringToUint(posB, &pos) - 1;
			posB += pos + 1;
			t = (int32)StringToUint(posB, &pos) - 1;
			posB += pos + 1;
			n = (int32)StringToUint(posB, &pos) - 1;

			face.x = positions[p].X;
			face.y = positions[p].Y;
			face.z = positions[p].Z;

			face.uvU = uvCoords[t].X;
			face.uvV = uvCoords[t].Y;
			face.uvW = uvCoords[t].Z;

			face.normalX = normals[n].X;
			face.normalY = normals[n].Y;
			face.normalZ = normals[n].Z;

			m_Vertexes.PushBack(face);
			m_Indexes.EmplaceBack(m_Vertexes.GetSize());

			posB += pos + 1;
			p = (int32)StringToUint(posB, &pos) - 1;
			posB += pos + 1;
			t = (int32)StringToUint(posB, &pos) - 1;
			posB += pos + 1;
			n = (int32)StringToUint(posB, &pos) - 1;

			face.x = positions[p].X;
			face.y = positions[p].Y;
			face.z = positions[p].Z;

			face.uvU = uvCoords[t].X;
			face.uvV = uvCoords[t].Y;
			face.uvW = uvCoords[t].Z;

			face.normalX = normals[n].X;
			face.normalY = normals[n].Y;
			face.normalZ = normals[n].Z;

			m_Vertexes.PushBack(face);
			m_Indexes.EmplaceBack(m_Vertexes.GetSize());
		}
	}

	return;
}
