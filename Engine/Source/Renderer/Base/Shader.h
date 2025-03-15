#pragma once

#include <Core/Containers/String.h>

namespace Pawn::Render
{
	class Shader
	{
	public:
		enum class Type
		{
			None = 0,
			Vertex, Pixel, Compute, Geometry, Hull, Domain
		};

	public:
		virtual ~Shader() {};

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual Shader::Type GetShaderType() const = 0;

	public:
		static Shader* CreateShader(const String& path, Shader::Type type, bool compiled);

	public:
		static Shader* CreateCompiledShader(const String& path, Shader::Type type);
		static Shader* CreateAndCompileShader(const String& path, Shader::Type type);

	// Compiled
	public:
		static Shader* CreateCompiledDirectX11Shader(const String& path, Shader::Type type);

	// Read then compile
	public:
		static Shader* CreateAndCompileDirectX11Shader(const String& path, Shader::Type type);


	};
}

