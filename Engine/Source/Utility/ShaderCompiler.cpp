#include "ShaderCompiler.hpp"

#include <xxhash.h>
#include <Core/Utils/Logging/Logger.hpp>

#define CONCAT_TARGET_PROFILE(profile, version) profile version
#define SC_ARG(x) L"-" ## x
#define SC_VALUE(x) L ## x


namespace ME::Utility
{
	ShaderCompiler::ShaderCompiler()
	{
		m_Utils = nullptr;
		m_Compiler = nullptr;
		m_IncludeHandler = nullptr;
		m_DebugInfo = false;
		m_CompilerWorks = false;
	}

	ShaderCompiler::~ShaderCompiler()
	{
		ShutdownCompiler();
	}

	bool ShaderCompiler::InitCompiler()
	{
		if (m_CompilerWorks) return true;
        IDxcUtils* utils = nullptr;
		IDxcCompiler3* compiler = nullptr;
		IDxcIncludeHandler* includeHandler = nullptr;

		HRESULT result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
		if (FAILED(result))
		{
            ME_ASSERT(false, "Shader compiler: failed to create DXC utils! DXC error: {0}", static_cast<uint32>(result));
			ShutdownCompiler();
			return false;
		}
		m_Utils.reset(utils);

		result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
		if (FAILED(result))
		{
            ME_ASSERT(false, "Shader compiler: failed to create DXC compiler! DXC error: {0}", static_cast<uint32>(result));
			ShutdownCompiler();
			return false;
		}
		m_Compiler.reset(compiler);

		result = m_Utils->CreateDefaultIncludeHandler(&includeHandler);
		if (FAILED(result))
		{
            ME_ASSERT(false, "Shader compiler: failed to create DXC include handler! DXC error: {0}", static_cast<uint32>(result));
			ShutdownCompiler();
			return false;
		}
		m_IncludeHandler.reset(includeHandler);

		m_SpvContext = spvContextCreate(SPV_ENV_UNIVERSAL_1_5);
		
		m_CompilerWorks = true;

		return true;
	}

	void ShaderCompiler::ShutdownCompiler()
	{
		if (!m_CompilerWorks) return;
		m_Utils.reset(nullptr);
		m_IncludeHandler.reset(nullptr);
		m_Compiler.reset(nullptr);
		spvContextDestroy(m_SpvContext);
		m_CompilerWorks = false;
	}

	CompilationResult ShaderCompiler::Compile(const ShaderCompilationSpecification& specification)
	{
		if (!m_CompilerWorks) return CompilationResult{ ShaderCompilationError::Failed, TEXT("Compiler doesn't work"), {nullptr, 0} };
        CompilationResult compResult = {};

		uint32 codePage;
		DxcBuffer buffer;
		IDxcBlobEncoding* sourceBlob = nullptr;
		IDxcResult* dxcResult = nullptr;
		IDxcBlob* code = nullptr;

		ME::Core::Containers::Array<LPCWSTR> arguments = {};

		codePage = DXC_CP_ACP;
		HRESULT result = m_Utils->LoadFile(specification.Path.String(), &codePage, &sourceBlob);
		if (FAILED(result))
		{
			compResult.Result = ShaderCompilationError::Failed;
			compResult.ErrorMsg = TEXT("Failed to load shader file");
			compResult.Shader.Bytecode = nullptr;
			compResult.Shader.Size = 0;
			return compResult;
		}

		// DXC Compile arguments
		// Argument format:
		// <path_to_shader> 
		// -E <entry_point (default: "main")> 
		// -T <target_profile> (<nothing> | -spirv | -metal) 
		// if debug: -Zi -Qsource_in_debug_module -Qembed_debug 
		// -Fo <output_file> 
		// -Zpr 
		// -fdiagnostics-format=msvc 
		// -D <value>... 
		// -I <path>...

		arguments.EmplaceBack(specification.Path.String());

		arguments.EmplaceBack(SC_ARG("E"));
		arguments.EmplaceBack(specification.EntryPoint.Size() > 0 ? specification.EntryPoint.String() : SC_VALUE("main"));

		arguments.EmplaceBack(SC_ARG("T"));
		arguments.EmplaceBack(SelectTargetProfile(specification.ShaderType).String());

		arguments.EmplaceBack(SC_ARG("Fo"));
		arguments.EmplaceBack(specification.OutputPath.String());

		arguments.EmplaceBack(SC_ARG("Zpr"));

		if (specification.Format == Render::ShaderFormat::SPIRV)
		{
			arguments.EmplaceBack(SC_ARG("spirv"));
			arguments.EmplaceBack(SC_ARG("fspv-target-env=universal1.5"));
			arguments.EmplaceBack(SC_ARG("fvk-use-dx-layout"));
			arguments.EmplaceBack(SC_ARG("fspv-reflect"));
		}
		else if (specification.Format == Render::ShaderFormat::Metal)
			arguments.EmplaceBack(SC_ARG("metal"));

		arguments.EmplaceBack(SelectOptimization(specification.Optimization).String());

		if (m_DebugInfo)
		{
			arguments.EmplaceBack(SC_ARG("Zi"));
			arguments.EmplaceBack(SC_ARG("Qsource_in_debug_module"));
			arguments.EmplaceBack(SC_ARG("Qembed_debug"));
		}

		arguments.EmplaceBack(SC_VALUE("-fdiagnostics-format=msvc"));

		for (const auto& define : specification.Defines)
		{
			arguments.EmplaceBack(SC_ARG("D"));
			arguments.EmplaceBack(define.String());
		}

		for (const auto& define : m_Defines)
		{
			arguments.EmplaceBack(SC_ARG("D"));
			arguments.EmplaceBack(define.String());
		}

		for (const auto& include : m_Defines)
		{
			arguments.EmplaceBack(SC_ARG("I"));
			arguments.EmplaceBack(include.String());
		}

		for (const auto& include : m_IncludePaths)
		{
			arguments.EmplaceBack(SC_ARG("I"));
			arguments.EmplaceBack(include.String());
		}

		buffer.Encoding = DXC_CP_ACP;
		buffer.Ptr = sourceBlob->GetBufferPointer();
		buffer.Size = sourceBlob->GetBufferSize();

		result = m_Compiler->Compile(&buffer, arguments.Data(), arguments.Size(), m_IncludeHandler.get(), IID_PPV_ARGS(&dxcResult));
		if (SUCCEEDED(result))
			dxcResult->GetStatus(&result);


		if (FAILED(result) && dxcResult)
		{
			IDxcBlobEncoding* errorBlob;
			result = dxcResult->GetErrorBuffer(&errorBlob);
			if (SUCCEEDED(result) && errorBlob)
			{
				compResult.Result = ShaderCompilationError::Failed;
				compResult.ErrorMsg = static_cast<const char8*>(errorBlob->GetBufferPointer());
				compResult.Shader.Bytecode = nullptr;
				compResult.Shader.Size = 0;
				compResult.Shader.Format = Render::ShaderFormat::None;

				return compResult;
			}
		}
		else if (FAILED(result))
		{
			compResult.Result = ShaderCompilationError::Failed;
			compResult.ErrorMsg = TEXT("Unknown");
			compResult.Shader.Bytecode = nullptr;
			compResult.Shader.Size = 0;
			compResult.Shader.Format = Render::ShaderFormat::None;
			
			return compResult;
		}

		dxcResult->GetResult(&code);
		compResult.ErrorMsg = TEXT("");
		compResult.Result = ShaderCompilationError::Success;
		compResult.Shader.Format = specification.Format;
		compResult.Shader.Size = code->GetBufferSize();
		compResult.Shader.Bytecode = new int8[compResult.Shader.Size];
		memcpy(compResult.Shader.Bytecode, code->GetBufferPointer(), compResult.Shader.Size);

		sourceBlob->Release();
		dxcResult->Release();
		code->Release();

		return compResult;
	}

	DisassemblyResult ShaderCompiler::Disassemble(Render::CompiledShader& shader)
	{
		switch (shader.Format)
		{
		case Render::ShaderFormat::DXIL:
			return DisassembleDXIL(shader);
		case Render::ShaderFormat::SPIRV:
			return DisassembleSPIRV(shader);
		default:
			return { "" };
		}
	}

	ME::Core::WideStringView ShaderCompiler::SelectTargetProfile(ME::Render::ShaderStage type) const
	{
		switch (type)
		{
			case ME::Render::ShaderStage::Pixel: return CONCAT_TARGET_PROFILE(L"ps", SHADER_MODEL);
			case ME::Render::ShaderStage::Vertex: return CONCAT_TARGET_PROFILE(L"vs", SHADER_MODEL);
			case ME::Render::ShaderStage::Geometry: return CONCAT_TARGET_PROFILE(L"gs", SHADER_MODEL);
			case ME::Render::ShaderStage::Hull: return CONCAT_TARGET_PROFILE(L"hs", SHADER_MODEL);
			case ME::Render::ShaderStage::Domain: return CONCAT_TARGET_PROFILE(L"ds", SHADER_MODEL);
			case ME::Render::ShaderStage::Compute: return CONCAT_TARGET_PROFILE(L"cs", SHADER_MODEL);
			case ME::Render::ShaderStage::Task: return CONCAT_TARGET_PROFILE(L"as", SHADER_MODEL);
			case ME::Render::ShaderStage::Mesh: return CONCAT_TARGET_PROFILE(L"ms", SHADER_MODEL);
			case ME::Render::ShaderStage::None: ME_ASSERT(false, "Shader compiler: can't select target profile for None!"); return TEXTW("");
		}
		return TEXTW("");
	}

	ME::Core::WideStringView ShaderCompiler::SelectOptimization(ME::Utility::ShaderOptimizationParameter parameter) const
	{
		switch (parameter)
		{
			case ShaderOptimizationParameter::Disabled: return SC_ARG("Od");
			case ShaderOptimizationParameter::Level0: return SC_ARG("O0");
			case ShaderOptimizationParameter::Level1: return SC_ARG("O1");
			case ShaderOptimizationParameter::Level2: return SC_ARG("O2");
			case ShaderOptimizationParameter::Level3: return SC_ARG("O3");
		}
	}

	DisassemblyResult ShaderCompiler::DisassembleSPIRV(Render::CompiledShader& shader)
	{
        DisassemblyResult disResult;
		spv_text disassembly;
		spv_diagnostic diagnostics;

		spv_result_t result = spvBinaryToText(m_SpvContext,
                                              static_cast<uint32*>(shader.Bytecode),
                                              shader.Size / 4,
                                              SPV_BINARY_TO_TEXT_OPTION_COMMENT |
                                              SPV_BINARY_TO_TEXT_OPTION_FRIENDLY_NAMES |
                                              SPV_BINARY_TO_TEXT_OPTION_SHOW_BYTE_OFFSET |
                                              SPV_BINARY_TO_TEXT_OPTION_NESTED_INDENT,
                                              &disassembly, &diagnostics);
		if (result != SPV_SUCCESS || disassembly->length <= 0)
		{
			ME_ERROR("Shader compiler: shader disassembly failed");
			disResult.Dissassebly = "";
			return disResult;
		}

		disResult.Dissassebly = ME::Core::String(disassembly->str, disassembly->length);

		spvTextDestroy(disassembly);
		spvDiagnosticDestroy(diagnostics);

		return disResult;
	}

	DisassemblyResult ShaderCompiler::DisassembleDXIL(Render::CompiledShader& shader)
	{
		HRESULT result;
		DxcBuffer buffer;
		IDxcResult* dxcResult = nullptr;
		IDxcBlob* disassembly = nullptr;
		DisassemblyResult disResult;

		buffer.Encoding = DXC_CP_ACP;
		buffer.Ptr = shader.Bytecode;
		buffer.Size = shader.Size;

		result = m_Compiler->Disassemble(&buffer, IID_PPV_ARGS(&dxcResult));
		if (SUCCEEDED(result))
			dxcResult->GetStatus(&result);

		if (FAILED(result) && dxcResult)
		{
			IDxcBlobEncoding* errorBlob;
			result = dxcResult->GetErrorBuffer(&errorBlob);
			if (SUCCEEDED(result) && errorBlob)
			{
				ME_ERROR("Shader compiler: shader disassembly failed: {0}", static_cast<const asciichar*>(errorBlob->GetBufferPointer()));
				disResult.Dissassebly = "";
				return disResult;
			}
		}
		else if (FAILED(result))
		{
			disResult.Dissassebly = "";
			return disResult;
		}

		dxcResult->GetResult(&disassembly);

		disResult.Dissassebly = ME::Core::String(static_cast<const char8*>(disassembly->GetBufferPointer()), disassembly->GetBufferSize());

		disassembly->Release();
		dxcResult->Release();

		return disResult;
	}

}
