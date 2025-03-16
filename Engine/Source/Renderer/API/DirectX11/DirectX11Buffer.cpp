#include "DirectX11Buffer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/API/DirectX11/DirectX11Renderer.h"

namespace Pawn::Render
{

    VertexBuffer* VertexBuffer::CreateDirectX11VertexBuffer(void* data, SIZE_T size, Usage usage)
    {
        return new DirectX11VertexBuffer(data, size, usage);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////-  VertexBuffer  -///////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    DirectX11VertexBuffer::DirectX11VertexBuffer(void* data, SIZE_T size, Usage usage)
    {
        Init(data, size, usage);
    }

    DirectX11VertexBuffer::~DirectX11VertexBuffer()
    {
        PE_D3D11_RELEASE(m_Buffer);
    }

    void DirectX11VertexBuffer::Bind(BufferLayout& layout)
    {
        DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
        ID3D11Buffer* temp = m_Buffer.get();
        uint32 stride = layout.GetStride();
        uint32 offset = 0;
        render->GetDeviceContext()->IASetVertexBuffers(0, 1, &temp, &stride, &offset);
    }

    void DirectX11VertexBuffer::Unbind()
    {
        DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
        render->GetDeviceContext()->IASetVertexBuffers(0, 1, nullptr, nullptr, nullptr);
    }

    void DirectX11VertexBuffer::SetData(void* data, SIZE_T size)
    {
        PE_INFO(TEXT("DirectX11 VertexBuffer: SetData unsupported!"));
    }
    
    void* DirectX11VertexBuffer::GetData()
    {
        PE_INFO(TEXT("DirectX11 VertexBuffer: GetData unsupported!"));
        return nullptr;
    }

    void DirectX11VertexBuffer::Init(void* data, SIZE_T size, Usage usage)
    {
        HRESULT result;
        D3D11_BUFFER_DESC bufferDesc = {};
        D3D11_SUBRESOURCE_DATA subresData = {};
        ID3D11Buffer* temp;

        DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

        bufferDesc.ByteWidth = (uint32)size;
        switch (usage)
        {
            case Pawn::Render::Usage::Default:          bufferDesc.Usage = D3D11_USAGE_DEFAULT; break;
            case Pawn::Render::Usage::Immutable:        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; break;
            case Pawn::Render::Usage::Dynamic:          bufferDesc.Usage = D3D11_USAGE_DYNAMIC; break;
            case Pawn::Render::Usage::Staging:          bufferDesc.Usage = D3D11_USAGE_STAGING; break;
            default: PE_ASSERT(false, TEXT("Wtf is a kilometer!!!"));
        }
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        subresData.pSysMem = data;
        subresData.SysMemPitch = 0;
        subresData.SysMemSlicePitch = 0;

        result = render->GetDevice()->CreateBuffer(&bufferDesc, &subresData, &temp);
        if (FAILED(result)) 
        {
            PE_ASSERT(false, TEXT("DirectX11: VertexBuffer: Creation failed!"));
            return;
        };

        m_Buffer = Memory::Reference<ID3D11Buffer>(temp);
    }
    
    /////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////-  IndexBuffer  -////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    IndexBuffer* IndexBuffer::CreateDirectX11IndexBuffer(void* data, uint32 count, Usage usage)
    {
        return new DirectX11IndexBuffer(data, count, usage);
    }

    DirectX11IndexBuffer::DirectX11IndexBuffer(void* data, uint32 count, Usage usage)
    {
        Init(data, count, usage);
    }
    
    DirectX11IndexBuffer::~DirectX11IndexBuffer()
    {
        PE_D3D11_RELEASE(m_Buffer);
    }
    
    void DirectX11IndexBuffer::Bind()
    {
        DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
        render->GetDeviceContext()->IASetIndexBuffer(m_Buffer.get(), DXGI_FORMAT_R32_UINT, 0);
    }
    
    void DirectX11IndexBuffer::Unbind()
    {
        DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
        render->GetDeviceContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
    }
    
    void DirectX11IndexBuffer::SetData(void* data, uint32 count)
    {
        PE_INFO(TEXT("DirectX11 IndexBuffer: SetData unsupported!"));
    }
    
    void* DirectX11IndexBuffer::GetData()
    {
        PE_INFO(TEXT("DirectX11 IndexBuffer: GetData unsupported!"));
        return nullptr;
    }
    
    uint32 DirectX11IndexBuffer::GetCount()
    {
        return m_Count;
    }

    void DirectX11IndexBuffer::Init(void* data, uint32 count, Usage usage)
    {
		HRESULT result;
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresData = {};
        ID3D11Buffer* temp;

		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

		bufferDesc.ByteWidth = count * sizeof(uint32);
		switch (usage)
		{
		    case Pawn::Render::Usage::Default:          bufferDesc.Usage = D3D11_USAGE_DEFAULT; break;
		    case Pawn::Render::Usage::Immutable:        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; break;
		    case Pawn::Render::Usage::Dynamic:          bufferDesc.Usage = D3D11_USAGE_DYNAMIC; break;
		    case Pawn::Render::Usage::Staging:          bufferDesc.Usage = D3D11_USAGE_STAGING; break;
		    default: PE_ASSERT(false, TEXT("Wtf is a kilometer!!!"));
		}
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		subresData.pSysMem = data;
		subresData.SysMemPitch = 0;
		subresData.SysMemSlicePitch = 0;

		result = render->GetDevice()->CreateBuffer(&bufferDesc, &subresData, &temp);
		if (FAILED(result))
		{
			PE_ASSERT(false, TEXT("DirectX11: IndexBuffer: Creation failed!"));
			return;
		};

        m_Buffer = Memory::Reference<ID3D11Buffer>(temp);
        m_Count = count;
    }


    uint32 ConvertShaderTypeDirectX11(ShaderType type)
    {
        switch (type)
		{
            case Pawn::Render::ShaderType::Float:
            {
                return DXGI_FORMAT_R32_FLOAT;
                break;
            }
            case Pawn::Render::ShaderType::Float2:
    		{
    			return DXGI_FORMAT_R32G32_FLOAT;
    			break;
    		}
            case Pawn::Render::ShaderType::Float3:
    		{
    			return DXGI_FORMAT_R32G32B32_FLOAT;
    			break;
    		}
            case Pawn::Render::ShaderType::Float4:
    		{
    			return DXGI_FORMAT_R32G32B32A32_FLOAT;
    			break;
    		}
            case Pawn::Render::ShaderType::Int:
    		{
    			return DXGI_FORMAT_R32_SINT;
    			break;
    		}
            case Pawn::Render::ShaderType::Int2:
    		{
    			return DXGI_FORMAT_R32G32_SINT;
    			break;
    		}
            case Pawn::Render::ShaderType::Int3:
    		{
    			return DXGI_FORMAT_R32G32B32_SINT;
    			break;
    		}
            case Pawn::Render::ShaderType::Int4:
    		{
    			return DXGI_FORMAT_R32G32B32A32_SINT;
    			break;
    		}
            case Pawn::Render::ShaderType::Bool:
            {
                return DXGI_FORMAT_R32_SINT;
                break;
            }
            case Pawn::Render::ShaderType::Uint:
            {
                return DXGI_FORMAT_R32_UINT;
                break;
            }
			case Pawn::Render::ShaderType::Uint2:
			{
				return DXGI_FORMAT_R32G32_UINT;
				break;
			}
			case Pawn::Render::ShaderType::Uint3:
			{
				return DXGI_FORMAT_R32G32B32_UINT;
				break;
			}
			case Pawn::Render::ShaderType::Uint4:
			{
				return DXGI_FORMAT_R32G32B32A32_UINT;
				break;
			}
			case Pawn::Render::ShaderType::None:
			{
				PE_ASSERT(false, TEXT("DirectX11: ShaderTypeConversion: Shader type ::None can't be converted!"));
				return 0;
				break;
			}
			case Pawn::Render::ShaderType::Mat3x3:
			{
				PE_ASSERT(false, TEXT("DirectX11: ShaderType conversion failed! Mat3x3 unsupported in DirectX11!"));
				return 0;
				break;
			}
			case Pawn::Render::ShaderType::Mat4x4:
			{
				PE_ASSERT(false, TEXT("DirectX11: ShaderType conversion failed! Mat4x4 unsupported in DirectX11!"));
				return 0;
				break;
			}
        }

        PE_ASSERT(false, TEXT("DirectX11: ShaderType conversion failed! Unknown type!"));
        return 0;
    }

	/////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////-  Uniform  -//////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    Uniform* Uniform::CreateDirectX11Uniform(SIZE_T size, Usage usage)
    {
        return new DirectX11Uniform(size, usage);
    }

	DirectX11Uniform::DirectX11Uniform(SIZE_T size, Usage usage)
{
        Init(size, usage);
	}

	DirectX11Uniform::~DirectX11Uniform()
	{
        PE_D3D11_RELEASE(m_Buffer);
	}

	void DirectX11Uniform::Bind(uint32 index, Shader::Type stage)
    {
        DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
        ID3D11Buffer* temp = m_Buffer.get();
        switch (stage)
        {
            case Pawn::Render::Shader::Type::Vertex:        render->GetDeviceContext()->VSSetConstantBuffers(index, 1, &temp); return; break;
            case Pawn::Render::Shader::Type::Pixel:         render->GetDeviceContext()->PSSetConstantBuffers(index, 1, &temp); return; break;
            case Pawn::Render::Shader::Type::Compute:       render->GetDeviceContext()->CSSetConstantBuffers(index, 1, &temp); return; break;
            case Pawn::Render::Shader::Type::Geometry:      render->GetDeviceContext()->GSSetConstantBuffers(index, 1, &temp); return; break;
            case Pawn::Render::Shader::Type::Hull:          render->GetDeviceContext()->HSSetConstantBuffers(index, 1, &temp); return; break;
            case Pawn::Render::Shader::Type::Domain:        render->GetDeviceContext()->DSSetConstantBuffers(index, 1, &temp); return; break;
            case Pawn::Render::Shader::Type::None:
            {
                PE_ASSERT(false, TEXT("Stage None is unsupported!"));
                return;
                break;
            }
        }
        PE_ASSERT(false, TEXT("Stage None is unsupported!"));
	}

	void DirectX11Uniform::Unbind(uint32 index, Shader::Type stage)
    {
		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());  
		switch (stage)
		{
		case Pawn::Render::Shader::Type::Vertex:        render->GetDeviceContext()->VSSetConstantBuffers(index, 1, nullptr); return; break;
		case Pawn::Render::Shader::Type::Pixel:         render->GetDeviceContext()->PSSetConstantBuffers(index, 1, nullptr); return; break;
		case Pawn::Render::Shader::Type::Compute:       render->GetDeviceContext()->CSSetConstantBuffers(index, 1, nullptr); return; break;
		case Pawn::Render::Shader::Type::Geometry:      render->GetDeviceContext()->GSSetConstantBuffers(index, 1, nullptr); return; break;
		case Pawn::Render::Shader::Type::Hull:          render->GetDeviceContext()->HSSetConstantBuffers(index, 1, nullptr); return; break;
		case Pawn::Render::Shader::Type::Domain:        render->GetDeviceContext()->DSSetConstantBuffers(index, 1, nullptr); return; break;
		case Pawn::Render::Shader::Type::None:
		{
			PE_ASSERT(false, TEXT("Stage None is unsupported!"));
			return;
			break;
		}
		}
		PE_ASSERT(false, TEXT("Stage None is unsupported!"));
	}

    void DirectX11Uniform::SetData(void* data, SIZE_T size)
    {
		if (size > m_Size)
		{
			PE_ERROR(TEXT("Data size ({0}) exceeds uniform buffer size ({1})!"), size, m_Size);
			return;
		}

        DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

        HRESULT result;
        D3D11_MAPPED_SUBRESOURCE resource;

        result = render->GetDeviceContext()->Map(m_Buffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
        if (FAILED(result))
        {
            PE_ERROR(TEXT("Failed to map buffer! Error: {0}"), result);
            return;
        }
        
        memcpy(resource.pData, data, size);

        render->GetDeviceContext()->Unmap(m_Buffer.get(), 0);
	}

	void* DirectX11Uniform::GetData()
	{
        PE_ERROR(TEXT("GetData is unsupoorted"));
        return nullptr;
	}

	void DirectX11Uniform::Init(SIZE_T size, Usage usage)
    {
		HRESULT result;
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresData = {};
		ID3D11Buffer* temp;

		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

        m_Size = (uint32)size;

		bufferDesc.ByteWidth = (uint32)size;

		switch (usage)
		{
		    case Pawn::Render::Usage::Default:          bufferDesc.Usage = D3D11_USAGE_DEFAULT; break;
		    case Pawn::Render::Usage::Immutable:        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; break;
		    case Pawn::Render::Usage::Dynamic:          bufferDesc.Usage = D3D11_USAGE_DYNAMIC; break;
		    case Pawn::Render::Usage::Staging:          bufferDesc.Usage = D3D11_USAGE_STAGING; break;
		    default: PE_ASSERT(false, TEXT("Wtf is a kilometer!!!"));
		}

		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		result = render->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &temp);
		if (FAILED(result))
		{
			PE_ASSERT(false, TEXT("DirectX11: IndexBuffer: Creation failed!"));
			return;
		};

		m_Buffer = Memory::Reference<ID3D11Buffer>(temp);
	}

}