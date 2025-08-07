#pragma once

#include <Core.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>
#include <Core/Containers/Tables/UnorderedMap.hpp>

#include "Renderer/Base/Shader.h"
#include "Utility/ShaderCompiler.hpp"

#define ME_COMPILED_SHADER_EXT L".cso"

#define ME_SHADER_VERTEX_ENTRY_POINT		"VSMain"
#define ME_SHADER_HULL_ENTRY_POINT			"HSMain"
#define ME_SHADER_DOMAIN_ENTRY_POINT		"DSMain"
#define ME_SHADER_GEOMETRY_ENTRY_POINT		"GSMain"
#define ME_SHADER_PIXEL_ENTRY_POINT			"PSMain"
#define ME_SHADER_COMPUTE_ENTRY_POINT		"CSMain"

#define PE_SHADER_VERTEX_ENTRY_POINT_W		TEXT(ME_SHADER_VERTEX_ENTRY_POINT)
#define PE_SHADER_HULL_ENTRY_POINT_W		TEXT(ME_SHADER_HULL_ENTRY_POINT)
#define PE_SHADER_DOMAIN_ENTRY_POINT_W		TEXT(ME_SHADER_DOMAIN_ENTRY_POINT)
#define PE_SHADER_GEOMETRY_ENTRY_POINT_W	TEXT(ME_SHADER_GEOMETRY_ENTRY_POINT)
#define PE_SHADER_PIXEL_ENTRY_POINT_W		TEXT(ME_SHADER_PIXEL_ENTRY_POINT)
#define PE_SHADER_COMPUTE_ENTRY_POINT_W		TEXT(ME_SHADER_COMPUTE_ENTRY_POINT)

namespace ME::Assets
{
	struct ShaderGroupPaths
	{
		ME::Core::Containers::String Vertex;
		ME::Core::Containers::String Hull;
		ME::Core::Containers::String Domain;
		ME::Core::Containers::String Geometry;
		ME::Core::Containers::String Pixel;
	};

	class MOON_API ShaderManager
	{
	public:
		struct ShaderGroup
		{
			ME::Core::Memory::Reference<ME::Render::Shader> Vertex;
			ME::Core::Memory::Reference<ME::Render::Shader> Hull;
			ME::Core::Memory::Reference<ME::Render::Shader> Domain;
			ME::Core::Memory::Reference<ME::Render::Shader> Geometry;
			ME::Core::Memory::Reference<ME::Render::Shader> Pixel;
		};

	public:
		ShaderManager();
		ShaderManager(const ShaderManager&) = delete;
		ShaderManager(ShaderManager&&) = delete;
		~ShaderManager();

	public:
		static ShaderManager& Get() { static ShaderManager instance; return instance; }

	public:
		void Shutdown();

	public:
		bool LoadShadersFromFiles(const uchar* shaderGroupName, ShaderGroupPaths& shaderGroupPath);
		bool LoadComputeFile(const uchar* name, const ME::Core::Containers::String& shaderPath);

		static bool LoadCompiler() { return Utility::ShaderCompiler::Get().InitCompiler(); }
		static void UnloadCompiler() { Utility::ShaderCompiler::Get().ShutdownCompiler(); }

	public:
		void SetCompiledShaderPath(const ME::Core::Containers::StringView& path) { m_CompiledShaderPath = path.ToString<ME::Core::Containers::String::AllocatorType>(); }
		void SetShaderSourcePath(const ME::Core::Containers::StringView& path) { m_ShaderSourcePath = path.ToString<ME::Core::Containers::String::AllocatorType>(); }

	public:
		inline ShaderGroup& GetShaderGroup(const uchar* name) { return m_GraphicsShaders[name]; }
		inline ShaderGroup& GetShaderGroup(const ME::Core::Containers::String& name) { return m_GraphicsShaders[name.GetString()]; }
		inline ME::Core::Memory::Reference<ME::Render::Shader> GetComputeShader(const uchar* name) { return m_ComputeShaders[name]; }
		inline ME::Core::Memory::Reference<ME::Render::Shader> GetComputeShader(const ME::Core::Containers::String& name) { return m_ComputeShaders[name.GetString()]; }
		inline ME::Core::Containers::StringView GetCompiledShadersPath() const { return m_CompiledShaderPath; }
		inline ME::Core::Containers::StringView GetShadersSourcePath() const { return m_ShaderSourcePath; }

	private:
		ME::Core::Memory::Reference<ME::Render::Shader> LoadShader(const ME::Core::Containers::WideStringView& shaderName, const ME::Render::Shader::Type& shaderType);
		ME::Core::Memory::Reference<ME::Render::Shader> LoadCompiledShader(const ME::Core::Containers::WideStringView& shaderName, const ME::Render::Shader::Type& shaderType) const;
		ME::Core::Memory::Reference<ME::Render::Shader> CompileShader(const ME::Core::Containers::WideStringView& shaderName, const ME::Render::Shader::Type& shaderType) const;

	private:
		ME::Core::Containers::UnorderedMap<const uchar*, ShaderGroup> m_GraphicsShaders;
		ME::Core::Containers::UnorderedMap<const uchar*, ME::Core::Memory::Reference<ME::Render::Shader>> m_ComputeShaders;

		ME::Core::Containers::String m_CompiledShaderPath;
		ME::Core::Containers::String m_ShaderSourcePath;

	};

}
