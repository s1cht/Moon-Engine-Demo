#include "DirectX11Shader.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/API/DirectX11/DirectX11Renderer.h"
#include "Core/Platform/Windows/WindowsIO.h"

namespace Pawn::Render
{

	Shader* Shader::CreateCompiledDirectX11Shader(const uchar* fileName, Shader::Type type)
	{
		return new DirectX11Shader(fileName, type, true);
	}

	Shader* Shader::CreateAndCompileDirectX11Shader(const uchar* fileName, Shader::Type type)
	{
		return new DirectX11Shader(fileName, type, false);
	}

	DirectX11Shader::DirectX11Shader(const uchar* fileName, Type type, bool compiled)
		: m_Type(type)
	{
		Init(fileName, compiled);
	}

	DirectX11Shader::~DirectX11Shader()
	{
		if (m_Shader != nullptr)
		{
			switch (m_Type)
			{
			case Pawn::Render::Shader::Type::Vertex:	static_cast<ID3D11VertexShader*>(m_Shader)->Release();		break;
			case Pawn::Render::Shader::Type::Pixel:		static_cast<ID3D11PixelShader*>(m_Shader)->Release();		break;
			case Pawn::Render::Shader::Type::Compute:	static_cast<ID3D11ComputeShader*>(m_Shader)->Release();		break;
			case Pawn::Render::Shader::Type::Geometry:	static_cast<ID3D11GeometryShader*>(m_Shader)->Release();	break;
			case Pawn::Render::Shader::Type::Hull:		static_cast<ID3D11HullShader*>(m_Shader)->Release();		break;
			case Pawn::Render::Shader::Type::Domain:	static_cast<ID3D11DomainShader*>(m_Shader)->Release();		break;
			}
			m_Shader = nullptr;
		}
		if (m_ShaderBuffer.ShaderPtr)
		{
			delete m_ShaderBuffer.ShaderPtr;
			m_ShaderBuffer.ShaderPtr = nullptr;
			m_ShaderBuffer.ShaderSize = 0;
		}
	}

	void DirectX11Shader::Bind()
	{
		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

		switch (m_Type)
		{
		case Pawn::Render::Shader::Type::Vertex:
		{
			render->GetDeviceContext()->VSSetShader(static_cast<ID3D11VertexShader*>(m_Shader), nullptr, 0);
			break;
		}
		case Pawn::Render::Shader::Type::Pixel:
		{
			render->GetDeviceContext()->PSSetShader(static_cast<ID3D11PixelShader*>(m_Shader), nullptr, 0);
			break;
		}
		case Pawn::Render::Shader::Type::Compute:
		{
			render->GetDeviceContext()->CSSetShader(static_cast<ID3D11ComputeShader*>(m_Shader), nullptr, 0);
			break;
		}
		case Pawn::Render::Shader::Type::Geometry:
		{
			render->GetDeviceContext()->GSSetShader(static_cast<ID3D11GeometryShader*>(m_Shader), nullptr, 0);
			break;
		}
		case Pawn::Render::Shader::Type::Hull:
		{
			render->GetDeviceContext()->HSSetShader(static_cast<ID3D11HullShader*>(m_Shader), nullptr, 0);
			break;
		}
		case Pawn::Render::Shader::Type::Domain:
		{
			render->GetDeviceContext()->DSSetShader(static_cast<ID3D11DomainShader*>(m_Shader), nullptr, 0);
			break;
		}
		}
	}

	void DirectX11Shader::Unbind()
	{
		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

		switch (m_Type)
		{
		case Pawn::Render::Shader::Type::Vertex:
		{
			render->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
			break;
		}
		case Pawn::Render::Shader::Type::Pixel:
		{
			render->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);
			break;
		}
		case Pawn::Render::Shader::Type::Compute:
		{
			render->GetDeviceContext()->CSSetShader(nullptr, nullptr, 0);
			break;
		}
		case Pawn::Render::Shader::Type::Geometry:
		{
			render->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
			break;
		}
		case Pawn::Render::Shader::Type::Hull:
		{
			render->GetDeviceContext()->HSSetShader(nullptr, nullptr, 0);
			break;
		}
		case Pawn::Render::Shader::Type::Domain:
		{
			render->GetDeviceContext()->DSSetShader(nullptr, nullptr, 0);
			break;
		}
		}
	}

	void DirectX11Shader::Init(const uchar* fileName, bool compiled)
	{
		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
		HRESULT result;
		bool exists;

		String compiledShaderPath = IO::DirectoryStorage::GetDirectory(TEXT("CompiledShaders")) + StringView(fileName) + Shader::GetCompiledShaderExtension();
		String shaderSourcePath = IO::DirectoryStorage::GetDirectory(TEXT("Shaders")) + StringView(fileName) + Shader::GetShaderSourceExtension();

		if (compiled)
		{
			exists = IO::PFileExists(compiledShaderPath.GetString());
			if (exists)
			{
				m_ShaderBuffer = ReadCompiledShader(compiledShaderPath);
				if (m_ShaderBuffer.ShaderSize <= 0)
				{
					PE_ASSERT(false, TEXT("Compilation failed! Error: {0}"));
					return;
				}
			}
			else
			{
				m_ShaderBuffer = CompileShader(shaderSourcePath, compiledShaderPath);
				if (m_ShaderBuffer.ShaderSize <= 0)
				{
					PE_ASSERT(false, TEXT("Compilation failed! Error: {0}"));
					return;
				}
			}
		}
		else
		{
			m_ShaderBuffer = CompileShader(shaderSourcePath, compiledShaderPath);
			if (m_ShaderBuffer.ShaderSize <= 0)
			{
				PE_ASSERT(false, TEXT("Compilation failed! Error: {0}"));
				return;
			}
		}

		switch (m_Type)
		{
		case Pawn::Render::Shader::Type::None: PE_ASSERT(false, TEXT("Creation of None shader is prohibited!")) break;
		case Pawn::Render::Shader::Type::Vertex:
		{
			ID3D11VertexShader* shader;
			result = render->GetDevice()->CreateVertexShader(m_ShaderBuffer.ShaderPtr, m_ShaderBuffer.ShaderSize, nullptr, &shader);
			if (FAILED(result))
			{
				PE_ASSERT(false, TEXT("VertexShader creation failed! Error: {0}"), (int32)result);
				if (shader)
					shader->Release();
				return;
			}
			m_ShaderSize = sizeof(shader);
			m_Shader = static_cast<void*>(shader);

			break;
		}
		case Pawn::Render::Shader::Type::Pixel:
		{
			ID3D11PixelShader* shader;
			result = render->GetDevice()->CreatePixelShader(m_ShaderBuffer.ShaderPtr, m_ShaderBuffer.ShaderSize, nullptr, &shader);
			if (FAILED(result))
			{
				PE_ASSERT(false, TEXT("PixelShader creation failed! Error: {0}"), (int32)result);
				if (shader)
					shader->Release();
				return;
			}
			m_ShaderSize = sizeof(shader);
			m_Shader = static_cast<void*>(shader);

			break;
		}
		case Pawn::Render::Shader::Type::Compute:
		{
			ID3D11ComputeShader* shader;
			result = render->GetDevice()->CreateComputeShader(m_ShaderBuffer.ShaderPtr, m_ShaderBuffer.ShaderSize, nullptr, &shader);
			if (FAILED(result))
			{
				PE_ASSERT(false, TEXT("ComputeShader creation failed! Error: {0}"), (int32)result);
				if (shader)
					shader->Release();
				return;
			}
			m_ShaderSize = sizeof(shader);
			m_Shader = static_cast<void*>(shader);

			break;
		}
		case Pawn::Render::Shader::Type::Geometry:
		{
			ID3D11GeometryShader* shader;
			result = render->GetDevice()->CreateGeometryShader(m_ShaderBuffer.ShaderPtr, m_ShaderBuffer.ShaderSize, nullptr, &shader);
			if (FAILED(result))
			{
				PE_ASSERT(false, TEXT("GeometryShader creation failed! Error: {0}"), (int32)result);
				if (shader)
					shader->Release();
				return;
			}
			m_ShaderSize = sizeof(shader);
			m_Shader = static_cast<void*>(shader);

			break;
		}
		case Pawn::Render::Shader::Type::Hull:
		{
			ID3D11HullShader* shader;
			result = render->GetDevice()->CreateHullShader(m_ShaderBuffer.ShaderPtr, m_ShaderBuffer.ShaderSize, nullptr, &shader);
			if (FAILED(result))
			{
				PE_ASSERT(false, TEXT("HullShader creation failed! Error: {0}"), (int32)result);
				if (shader)
					shader->Release();
				return;
			}
			m_ShaderSize = sizeof(shader);
			m_Shader = static_cast<void*>(shader);

			break;
		}
		case Pawn::Render::Shader::Type::Domain:
		{
			ID3D11DomainShader* shader;
			result = render->GetDevice()->CreateDomainShader(m_ShaderBuffer.ShaderPtr, m_ShaderBuffer.ShaderSize, nullptr, &shader);
			if (FAILED(result))
			{
				PE_ASSERT(false, TEXT("VertexShader creation failed! Error: {0}"), (int32)result);
				if (shader)
					shader->Release();
				return;
			}
			m_ShaderSize = sizeof(shader);
			m_Shader = static_cast<void*>(shader);

			break;
		}
		}
	}

	DirectX11Shader::CompiledShader DirectX11Shader::CompileShader(const String& path, const String& compiledPath)
	{
		using namespace IO;

		Memory::Reference<File> file = POpenFile(path.GetString());
		Memory::Reference<File> fileWrite = POpenFile(compiledPath.GetString(), IO::FileReadMode::WriteAndRead);

		HRESULT result;
		uint32 flags;
		ID3D10Blob* temp;
		ID3D10Blob* errorBlob;
		const ansichar* entryPoint = nullptr;
		const ansichar* shaderFeatureLevel = nullptr;
		CompiledShader output;

		switch (m_Type)
		{
			case Pawn::Render::Shader::Type::None:
			{
				PE_ASSERT(false, TEXT("DirectX11: ShaderType was None!"));
				return CompiledShader(nullptr, 0);
			}
			case Pawn::Render::Shader::Type::Vertex:
			{
				entryPoint = "VSMain";
				shaderFeatureLevel = "vs_5_0";
				break;
			}
			case Pawn::Render::Shader::Type::Pixel:
			{
				entryPoint = "PSMain";
				shaderFeatureLevel = "ps_5_0";
				break;
			}
			case Pawn::Render::Shader::Type::Compute:
			{
				entryPoint = "CSMain";
				shaderFeatureLevel = "cs_5_0";
				break;
			}
			case Pawn::Render::Shader::Type::Geometry:
			{
				entryPoint = "GSMain";
				shaderFeatureLevel = "gs_5_0";
				break;
			}
			case Pawn::Render::Shader::Type::Hull:
			{
				entryPoint = "HSMain";
				shaderFeatureLevel = "hs_5_0";
				break;
			}
			case Pawn::Render::Shader::Type::Domain:
			{
				entryPoint = "DSMain";
				shaderFeatureLevel = "ds_5_0";
				break;
			}
			default:
			{
				PE_ASSERT(false, TEXT("Unknown shader type!"));
				return CompiledShader(nullptr, 0);
			}
		}

		flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

		result = D3DCompileFromFile(path.GetString(), nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint, shaderFeatureLevel,
			flags, 0,
			&temp, &errorBlob);
		if (FAILED(result))
		{
			if (errorBlob)
			{
				AnsiString str(static_cast<const ansichar*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
				PE_ASSERT(false, "Shader compilation failed! Error: {0}", str.GetString());
				errorBlob->Release();
			}
			if (temp)
				temp->Release();

			return CompiledShader(nullptr, 0);
		}
		
		output.ShaderPtr = nullptr;
		output.ShaderSize = temp->GetBufferSize();
		memcpy(output.ShaderPtr, temp->GetBufferPointer(), output.ShaderSize);
		
		fileWrite->Write(reinterpret_cast<const uchar*>(output.ShaderPtr));
		fileWrite->Close();

		return output;
	}

	DirectX11Shader::CompiledShader DirectX11Shader::ReadCompiledShader(const String& path)
	{
		using namespace IO;

		bool result;
		Memory::Reference<File> file;
		CompiledShader output;

		file = POpenFile(path.GetString());

		output.ShaderSize = file->GetFileInfo().Size;
		output.ShaderPtr = ::operator new(output.ShaderSize);

		result = file->IsOpen();
		if (!result)
		{
			PE_ERROR(TEXT("Can't find shader! Error: {0}, Path {1}"), (uint32)file->GetFileLastError(), path.GetString());
			delete output.ShaderPtr;
			return CompiledShader(nullptr, 0);
		}

		result = file->ReadBinary(output.ShaderPtr, output.ShaderSize);
		if (!result)
		{
			PE_ERROR(TEXT("Can't read shader! Error: {0}, Path {1}"), (uint32)file->GetFileLastError(), path.GetString());
			delete output.ShaderPtr;
			return CompiledShader(nullptr, 0);
		}

		file->Close();

		return output;
	}

}