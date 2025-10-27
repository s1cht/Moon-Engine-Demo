#pragma once

#include <Core.hpp>
#include <Core/Containers/String/String.hpp>

#include <dxc/dxcapi.h>
#include <spirv-tools/libspirv.hpp>

#include "Core/Containers/Array.hpp"
#include "Renderer/Base/Shader.h"

#define SHADER_MODEL "_6_9"

namespace ME::Utility
{
	enum class ShaderCompilationError : uint8
	{
		Success = 0,
		Failed,
	};

	enum class ShaderOptimizationParameter : uint8
	{
		Disabled = 0,
		Level0, Level1, Level2, Level3
	};

	struct ShaderCompilationSpecification
	{
		// Path to shader
		ME::Core::WideStringView Path;

		// Bytecode format
		ME::Render::ShaderFormat Format;

		// Shader type
		ME::Render::ShaderStage ShaderType;

		// Output path (optional, if global output path is set up)
		ME::Core::WideStringView OutputPath;

		// Entry-point (optional)
		ME::Core::WideStringView EntryPoint;

		// Specific session-only defines (optional)
		ME::Core::Containers::Array<ME::Core::WideString> Defines;

		// Specific session-only includes (optional)
		ME::Core::Containers::Array<ME::Core::WideString> Includes;

		// Compiler optimizations level (default - Level3, if debug then disabled (ShaderOptimizationParameter::Disabled))
		ME::Utility::ShaderOptimizationParameter Optimization;
	};

	struct CompilationResult
	{
		ShaderCompilationError Result;
		ME::Core::String ErrorMsg;

		ME::Render::CompiledShader Shader;
	};

	struct DisassemblyResult
	{
		ME::Core::String Dissassebly;
	};

	class MEAPI ShaderCompiler
	{
		struct ComDeleter
		{
			constexpr void operator()(IUnknown* ptr) const noexcept
			{
				ptr->Release();
				ptr = nullptr;
			}
		};

		template <typename T>
		using COMPtr = ME::Core::Memory::Scope<T, ComDeleter>;

	public:
		ShaderCompiler();
		ShaderCompiler(const ShaderCompiler&) = delete;
		~ShaderCompiler();

	public:
		inline static ShaderCompiler& Get()
		{
			static ShaderCompiler Instance;
			return Instance;
		}

	public:
		bool InitCompiler();
		void ShutdownCompiler();

	public:
		CompilationResult Compile(const ShaderCompilationSpecification& specification);
		DisassemblyResult Disassemble(ME::Render::CompiledShader& shader);

	public:
		inline void SetGlobalDefines(ME::Core::Containers::Array<ME::Core::WideString>& defines) { m_Defines = defines; }
		inline void SetGlobalIncludePaths(ME::Core::Containers::Array<ME::Core::WideString>& paths) { m_IncludePaths = paths; };
		inline void SetDebugMode(bool mode) { m_DebugInfo = true; }

	public:
		inline ME::Core::Containers::Array<ME::Core::WideString>& GetGlobalDefines() { return m_Defines; }
		inline ME::Core::Containers::Array<ME::Core::WideString>& GetGlobalIncludePaths() { return m_IncludePaths; }
		inline bool DebugEnabled() const { return m_DebugInfo; }
		inline bool CompilerWorks() const { return m_CompilerWorks; }

	private:
		ME::Core::WideStringView SelectTargetProfile(ME::Render::ShaderStage type) const;
		ME::Core::WideStringView SelectOptimization(ME::Utility::ShaderOptimizationParameter parameter) const;

	private:
		DisassemblyResult DisassembleSPIRV(ME::Render::CompiledShader& shader);
		DisassemblyResult DisassembleDXIL(ME::Render::CompiledShader& shader);

	private:
		COMPtr<IDxcCompiler3> m_Compiler;
		COMPtr<IDxcUtils> m_Utils;
		COMPtr<IDxcIncludeHandler> m_IncludeHandler;
		spv_context m_SpvContext;

		bool m_CompilerWorks = false;

	private:
		bool m_DebugInfo;
		ME::Core::Containers::Array<ME::Core::WideString> m_Defines;
		ME::Core::Containers::Array<ME::Core::WideString> m_IncludePaths;

	};
}