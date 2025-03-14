#pragma once

#include "Renderer/Base/Buffer.h"
#include "Platform/Windows/Win32Platform.h"

namespace Pawn::Render
{
	class DirectX11VertexBuffer : public VertexBuffer
	{
	public:
		DirectX11VertexBuffer(void* data, SIZE_T size, Usage usage);
		virtual ~DirectX11VertexBuffer() override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void SetData(void* data) override;
		virtual void* GetData() override;

	private:
		void Init(void* data, SIZE_T size, Usage usage);

	private:
		ID3D11Buffer* m_Buffer;

	};

	class DirectX11IndexBuffer : public IndexBuffer
	{
	public:
		DirectX11IndexBuffer(void* data, uint32 count, Usage usage);
		virtual ~DirectX11IndexBuffer() override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void SetData(void* data) override;
		virtual void* GetData() override;

		virtual uint32 GetCount() override;

	private:
		void Init(void* data, uint32 count, Usage usage);

	private:
		ID3D11Buffer* m_Buffer;
		uint32 m_Count = 0;

	};

}