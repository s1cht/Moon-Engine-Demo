#include "DirectX11PipilineState.h"

#include "Renderer/API/DirectX11/DirectX11Renderer.h"
#include "Renderer/RenderCommand.h"


namespace Pawn::Render
{
	PipelineState* PipelineState::CreateDirectX11PipelineState()
	{
		return new DirectX11PipilineState();
	}

	DirectX11PipilineState::DirectX11PipilineState()
	{
		m_DepthStencilState = nullptr;
		m_InputLayout = nullptr;
		m_RasterizerState = nullptr;
		m_BlendState = nullptr;
	}

	DirectX11PipilineState::~DirectX11PipilineState()
	{
		PE_D3D11_RELEASE(m_BlendState);
		PE_D3D11_RELEASE(m_DepthStencilState);
		PE_D3D11_RELEASE(m_RasterizerState);
		PE_D3D11_RELEASE(m_InputLayout);
	}

	void DirectX11PipilineState::SetDepthStencilState(bool depthEnabled, bool stencilEnable, DepthComparison depthFunc)
	{
		PE_ASSERT(m_DepthStencilState.get() == nullptr, TEXT("DirectX11: DepthStencilState exists! Create new PipelineState and setup it!"));
		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

		HRESULT result;
		ID3D11DepthStencilState* temp;
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

		depthStencilDesc.DepthEnable = depthEnabled;
		depthStencilDesc.DepthWriteMask = depthEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

		switch (depthFunc)
		{
			case Pawn::Render::DepthComparison::None:			depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;			break;
			case Pawn::Render::DepthComparison::Less:			depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;			break;
			case Pawn::Render::DepthComparison::Greater:		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER;		break;
			case Pawn::Render::DepthComparison::LessEqual:		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;	break;
			case Pawn::Render::DepthComparison::GreaterEqual:	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;	break;
			case Pawn::Render::DepthComparison::NotEqual:		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NOT_EQUAL;		break;
			case Pawn::Render::DepthComparison::Equal:			depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL;			break;
			case Pawn::Render::DepthComparison::Always:			depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;		break;
			default:
				PE_ASSERT(false, TEXT("DirectX11: Unknown DepthComparison value"));
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;
				break;
		}

		depthStencilDesc.StencilEnable = stencilEnable;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = stencilEnable ? D3D11_STENCIL_OP_INCR : D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = stencilEnable ? D3D11_COMPARISON_ALWAYS : D3D11_COMPARISON_NEVER;
		depthStencilDesc.BackFace = depthStencilDesc.FrontFace;

		result = render->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &temp);
		if (FAILED(result))
		{
			PE_ASSERT(false, TEXT("DirectX11: Pipeline state. Creation failed! Error: {0}"), (uint32)result);
			return;
		}
		m_DepthStencilState = Memory::Scope<ID3D11DepthStencilState>(temp);
	}

	void DirectX11PipilineState::SetBlendState(bool enableBlend, BlendMask mask)
	{
		PE_ASSERT(m_BlendState.get() == nullptr, TEXT("DirectX11: BlendState exists! Create new PipelineState and setup it!"));
		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

		HRESULT result;
		ID3D11BlendState* temp;
		D3D11_BLEND_DESC blendDesc = {};

		blendDesc.RenderTarget[0].BlendEnable = enableBlend;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		switch (mask)
		{
		case Pawn::Render::BlendMask::All:		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;		break;
		case Pawn::Render::BlendMask::Red:		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED;		break;
		case Pawn::Render::BlendMask::Green:	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_GREEN;	break;
		case Pawn::Render::BlendMask::Blue:		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_BLUE;	break;
		case Pawn::Render::BlendMask::Alpha:	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALPHA;	break;
		default: 
			PE_ASSERT(false, TEXT("DirectX11: Unknown DepthComparison value"));
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;
		}

		result = render->GetDevice()->CreateBlendState(&blendDesc, &temp);
		if (FAILED(result))
		{
			PE_ASSERT(false, TEXT("DirectX11: Pipeline state. Creation failed! Error: {0}"), (uint32)result);
			return;
		}

		m_BlendState = Memory::Scope<ID3D11BlendState>(temp);
	}

	void DirectX11PipilineState::SetRasterizerState(RasterizerCull cull, RasterizerFill fill, bool frontCounterClockwise, int32 depthBias, float32 depthBiasClamp)
	{
		PE_ASSERT(m_RasterizerState.get() == nullptr, TEXT("DirectX11: RasterizerState exists! Create new PipelineState and setup it!"));
		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

		HRESULT result;
		ID3D11RasterizerState* temp;
		D3D11_RASTERIZER_DESC  rasterizerDesc = {};


		switch (cull)
		{
			case Pawn::Render::RasterizerCull::None:	rasterizerDesc.CullMode = D3D11_CULL_NONE;		break;
			case Pawn::Render::RasterizerCull::Back:	rasterizerDesc.CullMode = D3D11_CULL_BACK;		break;
			case Pawn::Render::RasterizerCull::Front:	rasterizerDesc.CullMode = D3D11_CULL_FRONT;		break;
			default:
				PE_ASSERT(false, TEXT("DirectX11: Unknown RasterizerCull value"));
				rasterizerDesc.CullMode = D3D11_CULL_BACK;
				break;
		}

		switch (fill)
		{
			case Pawn::Render::RasterizerFill::Fill:		rasterizerDesc.FillMode = D3D11_FILL_SOLID; break;
			case Pawn::Render::RasterizerFill::Wireframe:	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME; break;
			default:
				PE_ASSERT(false, TEXT("DirectX11: Unknown RasterizerFill value"));
				rasterizerDesc.FillMode = D3D11_FILL_SOLID;
				break;
		}

		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.DepthBias = depthBias;
		rasterizerDesc.DepthBiasClamp = depthBiasClamp;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = frontCounterClockwise;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		result = render->GetDevice()->CreateRasterizerState(&rasterizerDesc, &temp);
		if (FAILED(result))
		{
			PE_ASSERT(false, TEXT("DirectX11: Pipeline state. Creation failed! Error: {0}"), (uint32)result);
			return;
		}

		m_RasterizerState = Memory::Scope<ID3D11RasterizerState>(temp);
	}

	void DirectX11PipilineState::SetInputLayout(BufferLayout& layout, InputClassification inputSlotClass, uint32 instanceDataStepRate)
	{
		PE_ASSERT(m_InputLayout.get() == nullptr, TEXT("DirectX11: InputLayout exists! Create new PipelineState and setup it!"));
		PE_ASSERT(m_VertexShader.get() != nullptr, TEXT("DirectX11: VertexShader doesn't exist! Setup shaders before creating InputLayout!"));

		HRESULT result;
		SIZE_T i;
		Array<D3D11_INPUT_ELEMENT_DESC> polygonLayout;
		ID3D11InputLayout* temp;

		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

		polygonLayout.Resize(layout.GetElements().GetSize() * 4);

		i = 0;
		for (BufferElement element : layout.GetElements())
		{
			if (element.Type == ShaderType::Mat3x3)
			{
				for (uint8 row = 0; row < 4; ++row)
				{
					D3D11_INPUT_ELEMENT_DESC desc = {};
					desc.SemanticName = (element.Name + ToAnsiString(row)).GetString();
					desc.SemanticIndex = element.SemanticIndex;
					desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					desc.InputSlot = element.InputSlot;
					desc.AlignedByteOffset = element.Offset + (row * 16); 
					desc.InputSlotClass = (inputSlotClass == InputClassification::PerInstance) ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
					desc.InstanceDataStepRate = (inputSlotClass == InputClassification::PerInstance) ? instanceDataStepRate : 0;
					polygonLayout.PushBack(desc);
				}
			}
			else if (element.Type == ShaderType::Mat3x3)
			{

			}
			else
			{
				D3D11_INPUT_ELEMENT_DESC desc = {};
				desc.SemanticName = element.Name.GetString();
				desc.SemanticIndex = element.SemanticIndex;
				desc.Format = DXGI_FORMAT(ConvertShaderTypeDirectX11(element.Type));
				desc.InputSlot = element.InputSlot;
				desc.AlignedByteOffset = element.Offset;
				desc.InputSlotClass = (inputSlotClass == InputClassification::PerInstance) ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
				desc.InstanceDataStepRate = (inputSlotClass == InputClassification::PerInstance) ? instanceDataStepRate : 0;
				polygonLayout.PushBack(desc);
			}
		}

		result = render->GetDevice()->CreateInputLayout(polygonLayout.Data(), layout.GetElements().GetSize(),
			m_VertexShader->GetBufferPointer(), m_VertexShader->GetBufferSize(), &temp);

		if (FAILED(result))
		{
			PE_ASSERT(false, TEXT("DirectX11: Pipeline state. Creation failed! Error: {0}"), (uint32)result);
			return;
		}

		m_InputLayout = Memory::Scope<ID3D11InputLayout>(temp);
	}

	void DirectX11PipilineState::Bind()
	{
		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

		//m_VertexShader->Bind();
		//m_PixelShader->Bind();

		render->GetDeviceContext()->RSSetState(m_RasterizerState.get());
		render->GetDeviceContext()->OMSetDepthStencilState(m_DepthStencilState.get(), 1);
		render->GetDeviceContext()->OMSetBlendState(m_BlendState.get(), nullptr, 0xFFFFFFFF);
	}

	void DirectX11PipilineState::SetPrimitiveTopology(PrimitiveTopology topology, uint8 patchListPointCount)
	{
		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

		switch (topology)
		{
			case Pawn::Render::PrimitiveTopology::None:
			{
				render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);
				PE_ASSERT(false, TEXT("DirectX11: Primitive topology is none!"));
				break;
			}
			case Pawn::Render::PrimitiveTopology::PointList:
			{
				render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
				break;
			}
			case Pawn::Render::PrimitiveTopology::LineList:
			{
				render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
				break;
			}
			case Pawn::Render::PrimitiveTopology::LineStrip:
			{
				render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
				break;
			}
			case Pawn::Render::PrimitiveTopology::TriangleList:
			{
				render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				break;
			}
			case Pawn::Render::PrimitiveTopology::TriangleStrip:
			{
				render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				break;
			}
			case Pawn::Render::PrimitiveTopology::TriangleFan:
			{
				render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				break;
			}
			case Pawn::Render::PrimitiveTopology::PatchList:
			{
				PE_ASSERT(patchListPointCount > 32 || patchListPointCount == 0, TEXT("DirectX11: PipelineState: PatchList greater than 32!"));
				render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY(32 + patchListPointCount));
				break;
			}
		}
	}

	void DirectX11PipilineState::SetVertexShader(Memory::Reference<Shader> vertexShader)
	{
		m_VertexShader = vertexShader;
	}

	void DirectX11PipilineState::SetPixelShader(Memory::Reference<Shader> pixelShader)
	{
		m_PixelShader = pixelShader;
	}

}