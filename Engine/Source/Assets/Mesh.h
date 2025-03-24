#pragma once

#include "Renderer/Base/Buffer.h"

#include <Core.h>
#include <Core/Math/Math.h>
#include <Core/Memory/PawnMemory.h>
#include <Core/Containers/Array.h>

namespace Pawn::Assets
{
	struct Vertex
	{
		float32 x, y, z;
		float32 uvU, uvV, uvW;
		float32 normalX, normalY, normalZ;
	};

	enum class MeshFileFormats
	{
		None = 0,
		Obj, Fbx
	};

	class PAWN_API Mesh
	{
	public: 
		Mesh();
		Mesh(const uchar* path, MeshFileFormats format = MeshFileFormats::Obj);
		Mesh(const String& path, MeshFileFormats format = MeshFileFormats::Obj);
		Mesh(String&& path, MeshFileFormats format = MeshFileFormats::Obj);

		~Mesh();

	public:
		bool LoadFromFile(const uchar* path, MeshFileFormats format = MeshFileFormats::Obj);
		bool LoadFromFile(const String& path, MeshFileFormats format = MeshFileFormats::Obj);
		bool LoadFromFile(String&& path, MeshFileFormats format = MeshFileFormats::Obj);

	public:
		Memory::Reference<Render::VertexBuffer> GetVertexBuffer();
		Memory::Reference<Render::IndexBuffer> GetIndexBuffer();

	public:


	private:
		void LoadOBJ(const uchar* filePath);

	private:
		Array<Vertex> m_Vertexes;
		Array<int32> m_Indexes;

		Memory::Reference<Render::VertexBuffer> m_VertexBuffer;
		Memory::Reference<Render::IndexBuffer> m_IndexBuffer;

	};
}

