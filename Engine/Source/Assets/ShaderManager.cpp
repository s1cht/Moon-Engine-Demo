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

#include "EngineDefines.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Base/RenderAPI.h"

namespace ME::Assets
{
	ShaderManager::ShaderManager()
	{
#ifdef PE_SHADER_DEBUG
		Utility::ShaderCompiler::Get().SetDebugMode(true);
#else
		Utility::ShaderCompiler::Get().SetDebugMode(false);
#endif
	}

	ShaderManager::~ShaderManager() {}

	void ShaderManager::Shutdown()
	{
		m_GraphicsShaders.Clear();
		m_ComputeShaders.Clear();
	}


	bool ShaderManager::LoadShadersFromFiles(const uchar* shaderGroupName, const ShaderGroupSpecification& specification)
	{
		if (!Utility::ShaderCompiler::Get().CompilerWorks())
			return false;

		ME::Core::Containers::WideString path;
		ShaderGroup shaders = {.Vertex = nullptr, .Hull = nullptr, .Domain = nullptr, .Geometry = nullptr,
			.Pixel = nullptr };

		// Vertex shader
		path = ME::Core::Containers::StringToWideString(specification.Paths.Vertex);
		shaders.Vertex = LoadShader(path, specification.Layout, ME::Render::ShaderStage::Vertex);
		if (shaders.Vertex == nullptr)
			return false;

		// Hull shader
		path = ME::Core::Containers::StringToWideString(specification.Paths.Hull);
		if (!path.Empty())
			shaders.Hull = LoadShader(path, specification.Layout, ME::Render::ShaderStage::Hull);

		// Domain shader
		path = ME::Core::Containers::StringToWideString(specification.Paths.Domain);
		if (!path.Empty())
			shaders.Domain = LoadShader(path, specification.Layout, ME::Render::ShaderStage::Domain);

		// Geometry shader
		path = ME::Core::Containers::StringToWideString(specification.Paths.Geometry);
		if (!path.Empty())
			shaders.Geometry = LoadShader(path, specification.Layout, ME::Render::ShaderStage::Geometry);

		// Pixel shader
		path = ME::Core::Containers::StringToWideString(specification.Paths.Pixel);
		if (!path.Empty())
			shaders.Pixel = LoadShader(path, specification.Layout, ME::Render::ShaderStage::Pixel);

		if (shaders.Pixel == nullptr)
			return false;

		m_GraphicsShaders.Insert(shaderGroupName, shaders);

		return true;
	}

	bool ShaderManager::LoadComputeFile(const uchar* name, const ME::Core::Containers::String& shaderPath, const Render::ResourceLayoutPack& resourceLayout)
	{
		if (!Utility::ShaderCompiler::Get().CompilerWorks())
			return false;

		ME::Core::Containers::WideString path = ME::Core::Containers::StringToWideString(shaderPath);
		ME::Core::Memory::Reference<ME::Render::Shader> shader = LoadShader(
			path, resourceLayout, ME::Render::ShaderStage::Compute);
		if (shader == nullptr)
			return false;

		m_ComputeShaders.Insert(name, shader);

		return true;
	}

	ME::Core::Memory::Reference<ME::Render::Shader> ShaderManager::LoadShader(const ME::Core::Containers::WideStringView& shaderName, const Render::ResourceLayoutPack& layouts, const ME::Render::ShaderStage& shaderStage) const
	{
		bool result = ME::Core::IO::PFileExists(
			(ME::Core::Containers::WideStringToString(m_CompiledShaderPath + L"/" + shaderName)).GetString());

		if (result)
			return LoadCompiledShader(shaderName, layouts, shaderStage);

		return CompileShader(shaderName, layouts, shaderStage);
	}

	ME::Core::Memory::Reference<ME::Render::Shader> ShaderManager::LoadCompiledShader(const ME::Core::Containers::WideStringView& shaderName, const Render::ResourceLayoutPack& layouts, const ME::Render::ShaderStage& shaderStage) const
	{
		Render::CompiledShader compiledShader{ nullptr, 0 };

		Core::Memory::Reference<Core::IO::File> shaderFile = Core::IO::POpenFile(
			Core::Containers::WideStringToString(m_CompiledShaderPath + L"/" + shaderName + ME_COMPILED_SHADER_EXT).
			GetString());

		if (!shaderFile->IsOpen())
			return CompileShader(shaderName, layouts, shaderStage);

		compiledShader.Size = shaderFile->GetFileSize();
		compiledShader.Bytecode = ::operator new(compiledShader.Size);

		bool result = shaderFile->ReadBinary(compiledShader.Bytecode, compiledShader.Size);
		if (!result)
			return CompileShader(shaderName, layouts, shaderStage);

		Render::ShaderSpecification specification = {};
		specification.CompiledShader = compiledShader;
		specification.Stage = shaderStage;
		specification.Layouts = layouts;

		return ME::Render::Shader::Create(specification);
	}


	ME::Core::Memory::Reference<ME::Render::Shader> ShaderManager::CompileShader(const ME::Core::Containers::WideStringView& shaderName, const Render::ResourceLayoutPack& layouts, const ME::Render::ShaderStage& shaderStage) const
	{
		Utility::CompilationResult result;
		Utility::ShaderCompilationSpecification specs = {};
		ME::Core::Memory::Reference<ME::Render::Shader> shader;
		ME::Core::Containers::WideString src = m_ShaderSourcePath + L"/" + shaderName;
		ME::Core::Containers::WideString output = m_CompiledShaderPath + L"/" + shaderName;

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
			case Render::ShaderStage::Vertex:
			{
				specs.EntryPoint = PE_SHADER_VERTEX_ENTRY_POINT_W;
				break;
			}
			case Render::ShaderStage::Hull:
			{
				specs.EntryPoint = PE_SHADER_HULL_ENTRY_POINT_W;
				break;
			}
			case Render::ShaderStage::Domain:
			{
				specs.EntryPoint = PE_SHADER_DOMAIN_ENTRY_POINT_W;
				break;
			}
			case Render::ShaderStage::Geometry:
			{
				specs.EntryPoint = PE_SHADER_GEOMETRY_ENTRY_POINT_W;
				break;
			}
			case Render::ShaderStage::Pixel:
			{
				specs.EntryPoint = PE_SHADER_PIXEL_ENTRY_POINT_W;
				break;
			}
			case Render::ShaderStage::Compute:
			{
				specs.EntryPoint = PE_SHADER_COMPUTE_ENTRY_POINT_W;
				break;
			}
			case Render::ShaderStage::None:
			{
				ME_ERROR(TEXT("No default entry point available for ShaderStage::None!"));
				break;
			}
		}


#		ifdef PE_SHADER_DEBUG
			specs.Optimization = Utility::ShaderOptimizationParameter::Disabled;
#		else
			specs.Optimization = Utility::ShaderOptimizationParameter::Level3;
#		endif

		specs.ShaderType = shaderStage;

		result = Utility::ShaderCompiler::Get().Compile(specs);
		if (result.Result != Utility::ShaderCompilationError::Success)
		{
			ME_ERROR(TEXT("Shader compilation error! Error message: {0}"), result.ErrorMsg.GetString());
			return nullptr;
		}

		Render::ShaderSpecification specification = {};
		specification.CompiledShader = result.Shader;
		specification.Stage = shaderStage;
		specification.Layouts = layouts;

		return ME::Render::Shader::Create(specification);
	}
}
