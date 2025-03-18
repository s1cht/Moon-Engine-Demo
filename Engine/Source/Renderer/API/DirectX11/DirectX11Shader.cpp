#include "DirectX11Shader.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/API/DirectX11/DirectX11Renderer.h"
#include "Core/Platform/Windows/WindowsIO.h"

namespace Pawn::Render
{

	Shader* Shader::CreateCompiledDirectX11Shader(const String& path, Shader::Type type)
	{
		return new DirectX11Shader(path, type, true);
	}

	Shader* Shader::CreateAndCompileDirectX11Shader(const String& path, Shader::Type type)
	{
		return new DirectX11Shader(path, type, false);
	}

	DirectX11Shader::DirectX11Shader(String path, Type type, bool compiled)
		: m_Type(type)
	{
		Init(path, compiled);
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
		if (m_Buffer)
		{
			m_Buffer->Release();
			m_Buffer = nullptr;
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

	void DirectX11Shader::Init(String path, bool compiled)
	{
		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
		HRESULT result;
		uint32 flags;
		ID3D10Blob* temp;
		ID3D10Blob* errorBlob;
		const ansichar* entryPoint = nullptr;
		const ansichar* shaderFeatureLevel = nullptr;

		switch (m_Type)
		{
		case Pawn::Render::Shader::Type::None:
		{
			PE_ASSERT(false, TEXT("DirectX11: ShaderType was None!"));
			return;
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
			return;
		}
		}

		flags = 0;

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
		}
		else
		{
			m_Buffer = Memory::Scope<ID3D10Blob>(temp);
			m_BufferSize = m_Buffer->GetBufferSize();
		}

		switch (m_Type)
		{
		case Pawn::Render::Shader::Type::None: PE_ASSERT(false, TEXT("Creation of None shader is prohibited!")) break;
		case Pawn::Render::Shader::Type::Vertex:
		{
			ID3D11VertexShader* shader;
			result = render->GetDevice()->CreateVertexShader(m_Buffer->GetBufferPointer(), m_BufferSize, nullptr, &shader);
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
			result = render->GetDevice()->CreatePixelShader(m_Buffer->GetBufferPointer(), m_BufferSize, nullptr, &shader);
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
			result = render->GetDevice()->CreateComputeShader(m_Buffer->GetBufferPointer(), m_BufferSize, nullptr, &shader);
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
			result = render->GetDevice()->CreateGeometryShader(m_Buffer->GetBufferPointer(), m_BufferSize, nullptr, &shader);
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
			result = render->GetDevice()->CreateHullShader(m_Buffer->GetBufferPointer(), m_BufferSize, nullptr, &shader);
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
			result = render->GetDevice()->CreateDomainShader(m_Buffer->GetBufferPointer(), m_BufferSize, nullptr, &shader);
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

	DirectX11Shader::CompiledShader DirectX11Shader::CompileShader(String path)
	{
		using namespace IO;

		//Memory::Reference<File> file = POpenFile()
		CompiledShader output;

		return output;
	}

	DirectX11Shader::CompiledShader DirectX11Shader::ReadCompiledShader(String path)
	{
		CompiledShader output;


		return output;
	}

}