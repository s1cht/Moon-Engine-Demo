#pragma once

#include "Renderer/Base/Shader.h"
#include "Platform/Windows/Win32Platform.h"

#include <Core/Platform/Base/IO.hpp>
#include <Core/Containers/String/String.hpp>

namespace Pawn::Render
{
	class PAWN_API DirectX11Shader : public Shader
	{
	public:
		DirectX11Shader(const uchar* fileName, Type type, bool compiled);
		~DirectX11Shader();


	public:
		void Bind() override;
		void Unbind() override;

	public:
		inline const void* GetBuffer() { return m_ShaderBuffer.ShaderPtr; }
		inline SIZE_T GetBufferSize() const { return m_ShaderBuffer.ShaderSize; }
		inline CompiledShader GetShaderBuffer() const { return m_ShaderBuffer; }

		inline Shader::Type GetShaderType() const override { return m_Type; }

	private:
		void Init(const uchar* fileName, bool compiled);

		CompiledShader CompileShader(const Pawn::Core::Containers::String& path, const Pawn::Core::Containers::String& compiledPath);
		CompiledShader ReadCompiledShader(const Pawn::Core::Containers::String& path);

	private:
		Shader::Type m_Type;

		CompiledShader m_ShaderBuffer;

		void* m_Shader;
		SIZE_T m_ShaderSize;

	};

}
