//
//		ShaderManager.cpp
//
//----------------------------
// To do list:
// 1. Add hot reload
// 2. Add serialization
// 3. Recompile only with new source hash.

#include "ShaderManager.hpp"
#include <Core/Platform/Base/IO.hpp>

#include "EngineDefines.hpp"
#include "Renderer/RenderCommand.hpp"
#include "Renderer/Base/RenderAPI.hpp"

namespace ME::Render::Manager
{
	ShaderManager::ShaderManager()
	{
#ifdef ME_SHADER_DEBUG
		Utility::ShaderCompiler::Get().SetDebugMode(true);
#else
		Utility::ShaderCompiler::Get().SetDebugMode(false);
#endif
	}

	void ShaderManager::Shutdown()
	{
		m_Shaders.Clear();
		m_ComputeShaders.Clear();
	}

    bool ShaderManager::LoadShaders(const ShaderGroupSpecification& specification)
    {
        switch (specification.Type)
        {
			case ShaderGroupType::Vertex: return LoadVertexPipelineShaders(specification);
			case ShaderGroupType::Mesh: return LoadMeshPipelineShaders(specification);
			case ShaderGroupType::Compute: return LoadComputeShader(specification);
        }

        return false;
    }

    bool ShaderManager::LoadVertexPipelineShaders(const ShaderGroupSpecification& specification)
	{
		if (!Utility::ShaderCompiler::Get().CompilerWorks())
			return false;

		ShaderGroup shaders = {
			.UsesMeshPipeline = false,
			// Vertex pipeline
			.Vertex = nullptr, .Hull = nullptr, .Domain = nullptr, .Geometry = nullptr,
			// Mesh pipeline
			.Task = nullptr, .Mesh = nullptr,
			// Pixel shader
			.Pixel = nullptr,
		    .ResourceLayout = specification.Layout
		};

		// Pixel shader
		if (!specification.Pixel.Empty())
			shaders.Pixel = LoadShader(specification.Pixel, specification.Layout, ME::Render::ShaderStage::Pixel, specification.Defines);
		if (shaders.Pixel == nullptr)
			return false;

		// Vertex shader
		shaders.Vertex = LoadShader(specification.Vertex.Vertex, specification.Layout, ME::Render::ShaderStage::Vertex, specification.Defines);
		if (shaders.Vertex == nullptr)
			return false;

		// Hull shader
		if (!specification.Vertex.Hull.Empty())
			shaders.Hull = LoadShader(specification.Vertex.Hull, specification.Layout, ME::Render::ShaderStage::Hull, specification.Defines);

		// Domain shader
		if (!specification.Vertex.Domain.Empty())
			shaders.Domain = LoadShader(specification.Vertex.Domain, specification.Layout, ME::Render::ShaderStage::Domain, specification.Defines);

		// Geometry shader
		if (!specification.Vertex.Geometry.Empty())
			shaders.Geometry = LoadShader(specification.Vertex.Geometry, specification.Layout, ME::Render::ShaderStage::Geometry, specification.Defines);

		m_Shaders.Insert(specification.ShaderGroupName, shaders);

		return true;
	}

    bool ShaderManager::LoadMeshPipelineShaders(const ShaderGroupSpecification& specification)
    {
		if (!Utility::ShaderCompiler::Get().CompilerWorks())
			return false;

		ShaderGroup shaders = {
			.UsesMeshPipeline = true,
			// Vertex pipeline
			.Vertex = nullptr, .Hull = nullptr, .Domain = nullptr, .Geometry = nullptr,
			// Mesh pipeline
			.Task = nullptr, .Mesh = nullptr,
			// Pixel shader
			.Pixel = nullptr,
			.ResourceLayout = specification.Layout
		};

		// Pixel shader
		if (!specification.Pixel.Empty())
			shaders.Pixel = LoadShader(specification.Pixel, specification.Layout, ME::Render::ShaderStage::Pixel, specification.Defines);
		if (shaders.Pixel == nullptr)
			return false;

		// Mesh shader
		shaders.Mesh = LoadShader(specification.Mesh.Mesh, specification.Layout, ME::Render::ShaderStage::Mesh, specification.Defines);
		if (shaders.Mesh == nullptr)
			return false;

		// Task shader
		if (!specification.Mesh.Task.Empty())
			shaders.Task = LoadShader(specification.Mesh.Task, specification.Layout, ME::Render::ShaderStage::Task, specification.Defines);

		m_Shaders.Insert(specification.ShaderGroupName, shaders);

		return true;
    }

    bool ShaderManager::LoadComputeShader(const ShaderGroupSpecification& specification)
	{
		if (!Utility::ShaderCompiler::Get().CompilerWorks())
			return false;
		ComputeShaderGroup shaders = {.Shader = nullptr, .ResourceLayout = specification.Layout };

		shaders.Shader = LoadShader(
			specification.Compute, specification.Layout, ME::Render::ShaderStage::Compute, specification.Defines);
		if (shaders.Shader == nullptr)
			return false;

		m_ComputeShaders.Insert(specification.ShaderGroupName, shaders);

		return true;
	}

	ME::Core::Memory::Reference<ME::Render::Shader> ShaderManager::LoadShader(
		const ME::Core::StringView& shaderName, const Render::ResourceLayoutPack& layouts, 
		const ME::Render::ShaderStage& shaderStage,
		const ME::Core::Array<ME::Core::String>& defines) const
	{
		bool result = ME::Core::IO::PFileExists(
			(m_CompiledShaderPath + TEXT("/") + shaderName).String());

		if (result)
			return LoadCompiledShader(shaderName, layouts, shaderStage, defines);

		return CompileShader(shaderName, layouts, shaderStage, defines);
	}

	ME::Core::Memory::Reference<ME::Render::Shader> ShaderManager::LoadCompiledShader(
		const ME::Core::StringView& shaderName, 
		const Render::ResourceLayoutPack& layouts, const ME::Render::ShaderStage& shaderStage,
		const ME::Core::Array<ME::Core::String>& defines) const
	{
		Render::CompiledShader compiledShader{ .Bytecode = nullptr, .Size = 0 };

		Core::Memory::Reference<Core::IO::File> shaderFile = Core::IO::POpenFile(
			(m_CompiledShaderPath + TEXT("/") + shaderName + TEXT(ME_COMPILED_SHADER_EXT)).
			String());

		if (!shaderFile->IsOpen())
			return CompileShader(shaderName, layouts, shaderStage, defines);

		compiledShader.Size = shaderFile->GetFileSize();
		compiledShader.Bytecode = ::operator new(compiledShader.Size);

		bool result = shaderFile->ReadBinary(compiledShader.Bytecode, compiledShader.Size);
		if (!result)
			return CompileShader(shaderName, layouts, shaderStage, defines);

		Render::ShaderSpecification specification = {};
		specification.CompiledShader = compiledShader;
		specification.Stage = shaderStage;
		specification.Layouts = layouts;

		return ME::Render::Shader::Create(specification);
	}

	ME::Core::Memory::Reference<ME::Render::Shader> ShaderManager::CompileShader(const ME::Core::StringView& shaderName, 
		const Render::ResourceLayoutPack& layouts, 
		const ME::Render::ShaderStage& shaderStage, 
		const ME::Core::Array<ME::Core::String>& defines) const
	{
		Utility::CompilationResult result;
		Utility::ShaderCompilationSpecification specs = {};
		ME::Core::Memory::Reference<ME::Render::Shader> shader;
		ME::Core::WideString src = Core::StringToWideString(m_ShaderSourcePath) + Core::StringToWideString(shaderName.ToString());
		ME::Core::WideString output = Core::StringToWideString(m_CompiledShaderPath) + Core::StringToWideString(shaderName.ToString());

		specs.Path = src;
		specs.OutputPath = output;

		switch (ME::Render::RenderCommand::Get()->GetRendererAPI())
		{
			case Render::RenderAPI::API::Vulkan:
			{
				specs.Format = Render::ShaderFormat::SPIRV;
				break;
			}
			case Render::RenderAPI::API::Metal:
			{
				specs.Format = Render::ShaderFormat::Metal;
				break;
			}
			case Render::RenderAPI::API::DirectX12:
			{
				specs.Format = Render::ShaderFormat::DXIL;
				break;
			}
			default: ;
		}

		switch (shaderStage)
		{
		    case Render::ShaderStage::Task:
		    {
				specs.EntryPoint = ME_SHADER_TASK_ENTRY_POINT_W;
				break;
		    }
			case Render::ShaderStage::Mesh:
			{
				specs.EntryPoint = ME_SHADER_MESH_ENTRY_POINT_W;
				break;
			}
		    case Render::ShaderStage::Vertex:
			{
				specs.EntryPoint = ME_SHADER_VERTEX_ENTRY_POINT_W;
				break;
			}
			case Render::ShaderStage::Hull:
			{
				specs.EntryPoint = ME_SHADER_HULL_ENTRY_POINT_W;
				break;
			}
			case Render::ShaderStage::Domain:
			{
				specs.EntryPoint = ME_SHADER_DOMAIN_ENTRY_POINT_W;
				break;
			}
			case Render::ShaderStage::Geometry:
			{
				specs.EntryPoint = ME_SHADER_GEOMETRY_ENTRY_POINT_W;
				break;
			}
			case Render::ShaderStage::Pixel:
			{
				specs.EntryPoint = ME_SHADER_PIXEL_ENTRY_POINT_W;
				break;
			}
			case Render::ShaderStage::Compute:
			{
				specs.EntryPoint = ME_SHADER_COMPUTE_ENTRY_POINT_W;
				break;
			}
			case Render::ShaderStage::None:
			{
				ME_ERROR("No default entry point available for ShaderStage::None!");
				break;
			}
        }

#		ifdef ME_SHADER_DEBUG
			specs.Optimization = Utility::ShaderOptimizationParameter::Disabled;
#		else
			specs.Optimization = Utility::ShaderOptimizationParameter::Level3;
#		endif

		specs.ShaderType = shaderStage;

		specs.Defines = ConvertDefines(defines);

		result = Utility::ShaderCompiler::Get().Compile(specs);
		if (result.Result != Utility::ShaderCompilationError::Success)
		{
			ME_ERROR("Shader compilation error! Error message: {0}", result.ErrorMsg);
			return nullptr;
		}

	    Render::ShaderSpecification specification = {};
		specification.CompiledShader = result.Shader;
		specification.Stage = shaderStage;
		specification.Layouts = layouts;

		return ME::Render::Shader::Create(specification);
	}

    ME::Core::Array<ME::Core::WideString> ShaderManager::ConvertDefines(
        const ME::Core::Array<ME::Core::String>& defines) const
    {
		ME::Core::Array<ME::Core::WideString> newDefines;

		for (const auto& def : defines)
			newDefines.EmplaceBack(Core::StringToWideString(def));

        return newDefines;
    }
}
