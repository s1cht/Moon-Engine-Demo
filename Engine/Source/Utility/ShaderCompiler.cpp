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
		HRESULT result;
		IDxcUtils* utils = nullptr;
		IDxcCompiler3* compiler = nullptr;
		IDxcIncludeHandler* includeHandler = nullptr;

		result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
		if (FAILED(result))
		{
			ME_ASSERT(false, TEXT("Shader compiler: failed to create DXC utils! DXC error: {0}"), (uint32)result);
			ShutdownCompiler();
			return false;
		}
		m_Utils.reset(utils);

		result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
		if (FAILED(result))
		{
			ME_ASSERT(false, TEXT("Shader compiler: failed to create DXC compiler! DXC error: {0}"), (uint32)result);
			ShutdownCompiler();
			return false;
		}
		m_Compiler.reset(compiler);

		result = m_Utils->CreateDefaultIncludeHandler(&includeHandler);
		if (FAILED(result))
		{
			ME_ASSERT(false, TEXT("Shader compiler: failed to create DXC include handler! DXC error: {0}"), (uint32)result);
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
		HRESULT result;

		m_Utils.reset(nullptr);
		m_IncludeHandler.reset(nullptr);
		m_Compiler.reset(nullptr);
		spvContextDestroy(m_SpvContext);
		m_CompilerWorks = false;
	}

	CompilationResult ShaderCompiler::Compile(const ShaderCompilationSpecification& specification)
	{
		if (!m_CompilerWorks) return CompilationResult{ ShaderCompilationError::Failed, TEXT("Compiler doesn't work"), {nullptr, 0} };
		HRESULT result;
		CompilationResult compResult = {};

		uint32 codePage;
		DxcBuffer buffer;
		IDxcBlobEncoding* sourceBlob = nullptr;
		IDxcResult* dxcResult = nullptr;
		IDxcBlob* code = nullptr;

		ME::Core::Containers::Array<LPCWSTR> arguments = {};

		codePage = DXC_CP_ACP;
		result = m_Utils->LoadFile(specification.Path.GetData(), &codePage, &sourceBlob);
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
		// <path_to_shader> -E <entry_point (default: "main")> -Fo <output_file> -T <target_profile> (<nothing> | -spirv | -metal) (-Zi -Qsource_in_debug_module -Qembed_debug) -fdiagnostics-format=msvc -D <value>... -I <path>...

		arguments.EmplaceBack(specification.Path.GetData());

		arguments.EmplaceBack(SC_ARG("E"));
		arguments.EmplaceBack(specification.EntryPoint.GetSize() > 0 ? specification.EntryPoint.GetData() : SC_VALUE("main"));

		arguments.EmplaceBack(SC_ARG("T"));
		arguments.EmplaceBack(SelectTargetProfile(specification.ShaderType).GetData());

		arguments.EmplaceBack(SC_ARG("Fo"));
		arguments.EmplaceBack(specification.OutputPath.GetData());

		if (specification.Format == Render::ShaderFormat::SPIRV)
		{
			arguments.EmplaceBack(SC_ARG("spirv"));
			arguments.EmplaceBack(SC_ARG("fspv-target-env=universal1.5"));
			arguments.EmplaceBack(SC_ARG("fvk-use-dx-layout"));
			arguments.EmplaceBack(SC_ARG("fspv-reflect"));
		}
		else if (specification.Format == Render::ShaderFormat::Metal)
			arguments.EmplaceBack(SC_ARG("metal"));

		arguments.EmplaceBack(SelectOptimization(specification.Optimization).GetData());

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
			arguments.EmplaceBack(define.GetString());
		}

		for (const auto& define : m_Defines)
		{
			arguments.EmplaceBack(SC_ARG("D"));
			arguments.EmplaceBack(define.GetString());
		}

		for (const auto& include : m_Defines)
		{
			arguments.EmplaceBack(SC_ARG("I"));
			arguments.EmplaceBack(include.GetString());
		}

		for (const auto& include : m_IncludePaths)
		{
			arguments.EmplaceBack(SC_ARG("I"));
			arguments.EmplaceBack(include.GetString());
		}

		buffer.Encoding = DXC_CP_ACP;
		buffer.Ptr = sourceBlob->GetBufferPointer();
		buffer.Size = sourceBlob->GetBufferSize();

		result = m_Compiler->Compile(&buffer, arguments.Data(), arguments.GetSize(), m_IncludeHandler.get(), IID_PPV_ARGS(&dxcResult));
		if (SUCCEEDED(result))
			dxcResult->GetStatus(&result);


		if (FAILED(result) && dxcResult)
		{
			IDxcBlobEncoding* errorBlob;
			result = dxcResult->GetErrorBuffer(&errorBlob);
			if (SUCCEEDED(result) && errorBlob)
			{
				compResult.Result = ShaderCompilationError::Failed;
				compResult.ErrorMsg = ME::Core::Containers::AnsiStringToString(Core::Containers::AnsiString(static_cast<const ansichar*>(errorBlob->GetBufferPointer())));
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

	ME::Core::Containers::WideStringView ShaderCompiler::SelectTargetProfile(ME::Render::Shader::Type type) const
	{
		switch (type)
		{
			case ME::Render::Shader::Type::Pixel: return CONCAT_TARGET_PROFILE(L"ps", SHADER_MODEL);
			case ME::Render::Shader::Type::Vertex: return CONCAT_TARGET_PROFILE(L"vs", SHADER_MODEL);
			case ME::Render::Shader::Type::Geometry: return CONCAT_TARGET_PROFILE(L"gs", SHADER_MODEL);
			case ME::Render::Shader::Type::Hull: return CONCAT_TARGET_PROFILE(L"hs", SHADER_MODEL);
			case ME::Render::Shader::Type::Domain: return CONCAT_TARGET_PROFILE(L"ds", SHADER_MODEL);
			case ME::Render::Shader::Type::Compute: return CONCAT_TARGET_PROFILE(L"cs", SHADER_MODEL);
			//case Pawn::Render::Shader::Type::Vertex: return CONCAT_TARGET_PROFILE(L"lib", SHADER_MODEL);
			//case Pawn::Render::Shader::Type::Vertex: return CONCAT_TARGET_PROFILE(L"ms", SHADER_MODEL);
			//case Pawn::Render::Shader::Type::Vertex: return CONCAT_TARGET_PROFILE(L"as", SHADER_MODEL);
			case ME::Render::Shader::Type::None: ME_ASSERT(false, TEXT("Shader compiler: can't select target profile for None!")); return L"";
		}
	}

	ME::Core::Containers::WideStringView ShaderCompiler::SelectOptimization(ME::Utility::ShaderOptimizationParameter parameter) const
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
		spv_result_t result;
		DisassemblyResult disResult;
		spv_text disassembly;
		spv_diagnostic diagnostics;

		result = spvBinaryToText(m_SpvContext, 
			static_cast<uint32*>(shader.Bytecode), 
			shader.Size / 4, 
			SPV_BINARY_TO_TEXT_OPTION_COMMENT | 
			SPV_BINARY_TO_TEXT_OPTION_FRIENDLY_NAMES | 
			SPV_BINARY_TO_TEXT_OPTION_SHOW_BYTE_OFFSET | 
			SPV_BINARY_TO_TEXT_OPTION_NESTED_INDENT, 
			&disassembly, &diagnostics);
		if (result != SPV_SUCCESS || disassembly->length <= 0)
		{
			ME_ERROR(TEXT("Shader compiler: shader disassembly failed"));
			disResult.Dissassebly = "";

			return disResult;
		}

		disResult.Dissassebly = ME::Core::Containers::AnsiString(disassembly->str, disassembly->length);

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
				ME_ERROR(TEXT("Shader compiler: shader disassembly failed: {0}"), ME::Core::Containers::AnsiStringToString(Core::Containers::AnsiString(static_cast<const ansichar*>(errorBlob->GetBufferPointer()))));
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

		disResult.Dissassebly = ME::Core::Containers::AnsiString(static_cast<const ansichar*>(disassembly->GetBufferPointer()), disassembly->GetBufferSize());

		disassembly->Release();
		dxcResult->Release();

		return disResult;
	}

}
