#pragma once

#include "Renderer/Base/Buffer.h"
#include "Platform/Windows/Win32Platform.h"

namespace Pawn::Render
{
	class PAWN_API DirectX11VertexBuffer : public VertexBuffer
	{
	public:
		DirectX11VertexBuffer(void* data, SIZE_T size, Usage usage);
		~DirectX11VertexBuffer() override;

		void Bind(BufferLayout& layout) override;
		void Unbind() override;

		void SetData(void* data, SIZE_T size) override;
		void* GetData() override;

	private:
		void Init(void* data, SIZE_T size, Usage usage);

	private:
		Memory::Reference<ID3D11Buffer> m_Buffer;

	};

	class PAWN_API DirectX11IndexBuffer : public IndexBuffer
	{
	public:
		DirectX11IndexBuffer(void* data, uint32 count, Usage usage);
		~DirectX11IndexBuffer() override;

		void Bind() override;
		void Unbind() override;

		void SetData(void* data, uint32 count) override;
		void* GetData() override;

		uint32 GetCount() override;

	private:
		void Init(void* data, uint32 count, Usage usage);

	private:
		Memory::Reference<ID3D11Buffer> m_Buffer;
		uint32 m_Count = 0;

	};

	class PAWN_API DirectX11Uniform : public Uniform
	{
	public:
		DirectX11Uniform(SIZE_T size, Usage usage);
		~DirectX11Uniform() override;

	public:
		void Bind(uint32 index, Shader::Type stage) override;
		void Unbind(uint32 index, Shader::Type stage) override;

		void SetData(void* data, SIZE_T size) override;
		void* GetData() override;

	private:
		void Init(SIZE_T size, Usage usage);

	private:
		uint32 m_Size = 0;
		Memory::Reference<ID3D11Buffer> m_Buffer;

	};

};