#include "DirectX11Buffer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/API/DirectX11/DirectX11Renderer.h"

namespace Pawn::Render
{

    static VertexBuffer* CreateDirectX11VertexBuffer(void* data, SIZE_T size, Usage usage)
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

    void DirectX11VertexBuffer::Bind()
    {
        DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
        render->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_Buffer, nullptr, nullptr);
    }

    void DirectX11VertexBuffer::Unbind()
    {
        DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
        render->GetDeviceContext()->IASetVertexBuffers(0, 1, nullptr, nullptr, nullptr);
    }

    void DirectX11VertexBuffer::SetData(void* data)
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

        result = render->GetDevice()->CreateBuffer(&bufferDesc, &subresData, &m_Buffer);
        if (FAILED(result)) 
        {
            PE_ASSERT(false, TEXT("DirectX11: VertexBuffer: Creation failed!"));
            return;
        };
    }
    
    /////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////-  IndexBuffer  -////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    static IndexBuffer* CreateDirectX11IndexBuffer(void* data, uint32 count, Usage usage)
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
        render->GetDeviceContext()->IASetIndexBuffer(m_Buffer, DXGI_FORMAT_R32_UINT, 0);
    }
    
    void DirectX11IndexBuffer::Unbind()
    {
        DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
        render->GetDeviceContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
    }
    
    void DirectX11IndexBuffer::SetData(void* data)
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

		result = render->GetDevice()->CreateBuffer(&bufferDesc, &subresData, &m_Buffer);
		if (FAILED(result))
		{
			PE_ASSERT(false, TEXT("DirectX11: IndexBuffer: Creation failed!"));
			return;
		};
    }


    static uint32 ConvertShaderTypeDirectX11(ShaderType type)
    {
        switch (type)
		{
        case Pawn::Render::ShaderType::None:
            break;
        case Pawn::Render::ShaderType::Float:
            break;
        case Pawn::Render::ShaderType::Float2:
            break;
        case Pawn::Render::ShaderType::Float3:
            break;
        case Pawn::Render::ShaderType::Float4:
            break;
        case Pawn::Render::ShaderType::Int:
            break;
        case Pawn::Render::ShaderType::Int2:
            break;
        case Pawn::Render::ShaderType::Int3:
            break;
        case Pawn::Render::ShaderType::Int4:
            break;
        case Pawn::Render::ShaderType::Mat3x3:
            break;
        case Pawn::Render::ShaderType::Mat4x4:
            break;
        case Pawn::Render::ShaderType::Bool:
            break;
        default:
            break;
        }
    }
}