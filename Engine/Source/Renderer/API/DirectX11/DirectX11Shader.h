#pragma once

#include "Platform/Windows/Win32Platform.h"
#include "Renderer/Base/Shader.h"

namespace Pawn::Render
{
	class PAWN_API DirectX11Shader : public Shader
	{
	public:
		DirectX11Shader(String path, Type type, bool compiled);
		~DirectX11Shader();

	public:
		void Bind() override;
		void Unbind() override;

	public:
		inline Memory::Reference<ID3D10Blob> GetBuffer() { return m_Buffer; };
		inline SIZE_T GetBufferSize() const { return m_BufferSize; }

	private:
		void Init(String path);

	private:
		Memory::Reference<ID3D10Blob> m_Buffer;
		SIZE_T m_BufferSize;
		Shader::Type m_Type;

		void* m_Shader;
		SIZE_T m_ShaderSize;

	};

}
