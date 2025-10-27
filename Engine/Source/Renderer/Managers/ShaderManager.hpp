#pragma once

#include <Core.hpp>
#include <Core/Containers/String/String.hpp>
#include <Core/Containers/Tables/UnorderedMap.hpp>

#include "Renderer/Base/Shader.h"
#include "Utility/ShaderCompiler.hpp"

#define ME_COMPILED_SHADER_EXT ".cso"

#define ME_SHADER_VERTEX_ENTRY_POINT		"VSMain"
#define ME_SHADER_HULL_ENTRY_POINT			"HSMain"
#define ME_SHADER_DOMAIN_ENTRY_POINT		"DSMain"
#define ME_SHADER_GEOMETRY_ENTRY_POINT		"GSMain"
#define ME_SHADER_TASK_ENTRY_POINT			"TSMain"
#define ME_SHADER_MESH_ENTRY_POINT			"MSMain"
#define ME_SHADER_PIXEL_ENTRY_POINT			"PSMain"
#define ME_SHADER_COMPUTE_ENTRY_POINT		"CSMain"

#define ME_SHADER_VERTEX_ENTRY_POINT_W		TEXT_HELPERW(ME_SHADER_VERTEX_ENTRY_POINT)
#define ME_SHADER_HULL_ENTRY_POINT_W		TEXT_HELPERW(ME_SHADER_HULL_ENTRY_POINT)
#define ME_SHADER_DOMAIN_ENTRY_POINT_W		TEXT_HELPERW(ME_SHADER_DOMAIN_ENTRY_POINT)
#define ME_SHADER_GEOMETRY_ENTRY_POINT_W	TEXT_HELPERW(ME_SHADER_GEOMETRY_ENTRY_POINT)
#define ME_SHADER_TASK_ENTRY_POINT_W		TEXT_HELPERW(ME_SHADER_TASK_ENTRY_POINT)
#define ME_SHADER_MESH_ENTRY_POINT_W		TEXT_HELPERW(ME_SHADER_MESH_ENTRY_POINT)
#define ME_SHADER_PIXEL_ENTRY_POINT_W		TEXT_HELPERW(ME_SHADER_PIXEL_ENTRY_POINT)
#define ME_SHADER_COMPUTE_ENTRY_POINT_W		TEXT_HELPERW(ME_SHADER_COMPUTE_ENTRY_POINT)

namespace ME::Render::Manager
{
	struct VertexPipelineShaderPaths
	{
		ME::Core::String Vertex;
		ME::Core::String Hull;
		ME::Core::String Domain;
		ME::Core::String Geometry;
	};

	struct MeshPipelineShaderPaths
	{
		ME::Core::String Task;
		ME::Core::String Mesh;
	};

	struct ShaderGroupSpecification
	{
		VertexPipelineShaderPaths Vertex;
		MeshPipelineShaderPaths Mesh;
		ME::Core::String Pixel;
		Render::ResourceLayoutPack Layout;
		ME::Core::String ShaderGroupName;
	};

	class MEAPI ShaderManager
	{
	public:
		struct ShaderGroup
		{
		    bool UsesMeshPipeline = false;
			// Vertex pipeline
			ME::Core::Memory::Reference<ME::Render::Shader> Vertex;
			ME::Core::Memory::Reference<ME::Render::Shader> Hull;
			ME::Core::Memory::Reference<ME::Render::Shader> Domain;
			ME::Core::Memory::Reference<ME::Render::Shader> Geometry;
			// Mesh pipeline
			ME::Core::Memory::Reference<ME::Render::Shader> Task;
			ME::Core::Memory::Reference<ME::Render::Shader> Mesh;
			// Pixel shader
		    ME::Core::Memory::Reference<ME::Render::Shader> Pixel;

			Render::ResourceLayoutPack ResourceLayout;
		};

        struct ComputeShaderGroup
        {
			ME::Core::Memory::Reference<ME::Render::Shader> Shader;
			Render::ResourceLayoutPack ResourceLayout;
        };

	public:
		ShaderManager();
		ShaderManager(const ShaderManager&) = delete;
		ShaderManager(ShaderManager&&) = delete;
		~ShaderManager() = default;

	public:
		static ShaderManager& Get() { static ShaderManager instance; return instance; }

	public:
		void Shutdown();

	public:
		bool LoadVertexPipelineShadersFromFiles(const ShaderGroupSpecification& specification);
		bool LoadMeshPipelineShadersFromFiles(const ShaderGroupSpecification& specification);
		bool LoadComputeFile(const ME::Core::String& groupName, const ME::Core::String& shaderPath, const Render::ResourceLayoutPack& resourceLayout);

	public:
		void SetCompiledShaderPath(const ME::Core::StringView& path) { m_CompiledShaderPath = path.ToString<ME::Core::String::AllocatorType>(); }
		void SetShaderSourcePath(const ME::Core::StringView& path) { m_ShaderSourcePath = path.ToString<ME::Core::String::AllocatorType>(); }

	public:
		inline ShaderGroup GetShaderGroupM(const char8* name) { return m_GraphicsShaders[name]; }
		inline ShaderGroup GetShaderGroupM(const ME::Core::String& name) { return m_GraphicsShaders[name.String()]; }
		inline ComputeShaderGroup GetComputeShaderM(const char8* name) { return m_ComputeShaders[name]; }
		inline ComputeShaderGroup GetComputeShaderM(const ME::Core::String& name) { return m_ComputeShaders[name.String()]; }
		inline ME::Core::StringView GetCompiledShadersPathM() const { return m_CompiledShaderPath; }
		inline ME::Core::StringView GetShadersSourcePathM() const { return m_ShaderSourcePath; }

	public:
		static bool LoadCompiler() { return Utility::ShaderCompiler::Get().InitCompiler(); }
		static void UnloadCompiler() { Utility::ShaderCompiler::Get().ShutdownCompiler(); }

		inline static ShaderGroup GetShaderGroup(const char8* name) { return Get().GetShaderGroupM(name); }
		inline static ShaderGroup GetShaderGroup(const ME::Core::String& name) { return Get().GetShaderGroupM(name); }

	private:
		ME::Core::Memory::Reference<ME::Render::Shader> LoadShader(const ME::Core::StringView& shaderName, const Render::ResourceLayoutPack& layouts, const ME::Render::ShaderStage& shaderStage) const;
		ME::Core::Memory::Reference<ME::Render::Shader> LoadCompiledShader(const ME::Core::StringView& shaderName, const Render::ResourceLayoutPack& layouts, const ME::Render::ShaderStage& shaderStage) const;
		ME::Core::Memory::Reference<ME::Render::Shader> CompileShader(const ME::Core::StringView& shaderName, const Render::ResourceLayoutPack& layouts, const ME::Render::ShaderStage& shaderStage) const;

	private:
		ME::Core::Containers::UnorderedMap<ME::Core::String, ShaderGroup> m_GraphicsShaders;
		ME::Core::Containers::UnorderedMap<ME::Core::String, ComputeShaderGroup> m_ComputeShaders;

		ME::Core::String m_CompiledShaderPath;
		ME::Core::String m_ShaderSourcePath;

	};

}
