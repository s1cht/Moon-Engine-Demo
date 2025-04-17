#include "DirectX11Macros.h"
#include "DirectX11PipilineState.h"
#include "Renderer/API/DirectX11/DirectX11Renderer.h"
#include "Renderer/RenderCommand.h"

#include <Core/Misc/Assertion.h>


namespace Pawn::Render
{
	//PipelineState* PipelineState::CreateDirectX11PipelineState()
	//{
	//	return new DirectX11PipelineState();
	//}

	//DirectX11PipelineState::DirectX11PipelineState()
	//{
	//	m_DepthStencilState		= nullptr;
	//	m_InputLayout			= nullptr;
	//	m_RasterizerState		= nullptr;
	//	m_BlendState			= nullptr;
	//	m_VertexShader			= nullptr;
	//	m_PixelShader			= nullptr;
	//	m_ComputeShader			= nullptr;
	//	m_GeometryShader		= nullptr;
	//	m_HullShader			= nullptr;
	//	m_DomainShader			= nullptr;
	//	m_Viewport.TopLeftX = 0.0f;
	//	m_Viewport.TopLeftY = 0.0f;
	//	m_Viewport.MinDepth = 0.0f;
	//	m_Viewport.MaxDepth = 1.0f;
	//	m_Viewport.Width = 0.f;
	//	m_Viewport.Height = 0.f;
	//}

	//DirectX11PipelineState::~DirectX11PipelineState()
	//{
	//	PE_D3D11_RELEASE(m_BlendState);
	//	PE_D3D11_RELEASE(m_DepthStencilState);
	//	PE_D3D11_RELEASE(m_RasterizerState);
	//	PE_D3D11_RELEASE(m_InputLayout);
	//}

	//void DirectX11PipelineState::SetDepthStencilState(bool depthEnabled, bool stencilEnable, DepthComparison depthFunc)
	//{
	//	PE_ASSERT(m_DepthStencilState.get() == nullptr, TEXT("DirectX11: DepthStencilState exists! Create new PipelineState and setup it!"));
	//	DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

	//	HRESULT result;
	//	ID3D11DepthStencilState* temp;
	//	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	//	depthStencilDesc.DepthEnable = depthEnabled;
	//	depthStencilDesc.DepthWriteMask = depthEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

	//	switch (depthFunc)
	//	{
	//		case Pawn::Render::DepthComparison::None:			depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;			break;
	//		case Pawn::Render::DepthComparison::Less:			depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;			break;
	//		case Pawn::Render::DepthComparison::Greater:		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER;		break;
	//		case Pawn::Render::DepthComparison::LessEqual:		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;	break;
	//		case Pawn::Render::DepthComparison::GreaterEqual:	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;	break;
	//		case Pawn::Render::DepthComparison::NotEqual:		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NOT_EQUAL;		break;
	//		case Pawn::Render::DepthComparison::Equal:			depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL;			break;
	//		case Pawn::Render::DepthComparison::Always:			depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;		break;
	//		default:
	//			PE_ASSERT(false, TEXT("DirectX11: Unknown DepthComparison value"));
	//			depthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;
	//			break;
	//	}

	//	depthStencilDesc.StencilEnable = stencilEnable;
	//	depthStencilDesc.StencilReadMask = 0xFF;
	//	depthStencilDesc.StencilWriteMask = 0xFF;

	//	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//	depthStencilDesc.FrontFace.StencilDepthFailOp = stencilEnable ? D3D11_STENCIL_OP_INCR : D3D11_STENCIL_OP_KEEP;
	//	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//	depthStencilDesc.FrontFace.StencilFunc = stencilEnable ? D3D11_COMPARISON_ALWAYS : D3D11_COMPARISON_NEVER;
	//	depthStencilDesc.BackFace = depthStencilDesc.FrontFace;

	//	result = render->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &temp);
	//	if (FAILED(result))
	//	{
	//		PE_ASSERT(false, TEXT("DirectX11: Pipeline state. Creation failed! Error: {0}"), (uint32)result);
	//		return;
	//	}
	//	m_DepthStencilState = Pawn::Core::Memory::Scope<ID3D11DepthStencilState>(temp);
	//}

	//void DirectX11PipelineState::SetBlendState(bool enableBlend, BlendMask mask)
	//{
	//	PE_ASSERT(m_BlendState.get() == nullptr, TEXT("DirectX11: BlendState exists! Create new PipelineState and setup it!"));
	//	DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

	//	HRESULT result;
	//	ID3D11BlendState* temp;
	//	D3D11_BLEND_DESC blendDesc = {};

	//	blendDesc.RenderTarget[0].BlendEnable = enableBlend;
	//	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	//	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	//	switch (mask)
	//	{
	//	case Pawn::Render::BlendMask::All:		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;		break;
	//	case Pawn::Render::BlendMask::Red:		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED;		break;
	//	case Pawn::Render::BlendMask::Green:	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_GREEN;	break;
	//	case Pawn::Render::BlendMask::Blue:		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_BLUE;	break;
	//	case Pawn::Render::BlendMask::Alpha:	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALPHA;	break;
	//	default: 
	//		PE_ERROR(TEXT("DirectX11: Unknown DepthComparison value"));
	//		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//		break;
	//	}

	//	result = render->GetDevice()->CreateBlendState(&blendDesc, &temp);
	//	if (FAILED(result))
	//	{
	//		PE_ASSERT(false, TEXT("DirectX11: Pipeline state. Creation failed! Error: {0}"), (uint32)result);
	//		return;
	//	}

	//	m_BlendState = Pawn::Core::Memory::Scope<ID3D11BlendState>(temp);
	//}

	//void DirectX11PipelineState::SetRasterizerState(RasterizerCull cull, RasterizerFill fill,
	//	bool frontCounterClockwise, bool scissorEnabled, bool depthClipEnabled,
	//	bool slopeScaledDepthBias, int32 depthBias, float32 depthBiasClamp,
	//	bool multisampleEnabled, int32 sampleCount)
	//{
	//	PE_ASSERT(m_RasterizerState.get() == nullptr, TEXT("DirectX11: RasterizerState exists! Create new PipelineState and setup it!"));
	//	DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

	//	HRESULT result;
	//	ID3D11RasterizerState* temp;
	//	D3D11_RASTERIZER_DESC  rasterizerDesc = {};


	//	switch (cull)
	//	{
	//		case Pawn::Render::RasterizerCull::None:	rasterizerDesc.CullMode = D3D11_CULL_NONE;		break;
	//		case Pawn::Render::RasterizerCull::Back:	rasterizerDesc.CullMode = D3D11_CULL_BACK;		break;
	//		case Pawn::Render::RasterizerCull::Front:	rasterizerDesc.CullMode = D3D11_CULL_FRONT;		break;
	//		default:
	//			PE_ASSERT(false, TEXT("DirectX11: Unknown RasterizerCull value"));
	//			rasterizerDesc.CullMode = D3D11_CULL_BACK;
	//			break;
	//	}

	//	switch (fill)
	//	{
	//		case Pawn::Render::RasterizerFill::Fill:		rasterizerDesc.FillMode = D3D11_FILL_SOLID; break;
	//		case Pawn::Render::RasterizerFill::Wireframe:	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME; break;
	//		default:
	//			PE_ASSERT(false, TEXT("DirectX11: Unknown RasterizerFill value"));
	//			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	//			break;
	//	}

	//	rasterizerDesc.AntialiasedLineEnable = false;
	//	rasterizerDesc.DepthBias = depthBias;
	//	rasterizerDesc.DepthBiasClamp = depthBiasClamp;
	//	rasterizerDesc.DepthClipEnable = depthClipEnabled;
	//	rasterizerDesc.FrontCounterClockwise = frontCounterClockwise;
	//	rasterizerDesc.MultisampleEnable = multisampleEnabled;
	//	rasterizerDesc.ScissorEnable = scissorEnabled;
	//	rasterizerDesc.SlopeScaledDepthBias = slopeScaledDepthBias;

	//	result = render->GetDevice()->CreateRasterizerState(&rasterizerDesc, &temp);
	//	if (FAILED(result))
	//	{
	//		PE_ASSERT(false, TEXT("DirectX11: Pipeline state. Creation failed! Error: {0}"), (uint32)result);
	//		return;
	//	}

	//	m_RasterizerState = Pawn::Core::Memory::Scope<ID3D11RasterizerState>(temp);
	//}

	//void DirectX11PipelineState::SetInputLayout(BufferLayout& layout, InputClassification inputSlotClass, uint32 instanceDataStepRate)
	//{
	//	PE_ASSERT(m_InputLayout.get() == nullptr, TEXT("DirectX11: InputLayout exists! Create new PipelineState and setup it!"));
	//	PE_ASSERT(m_VertexShader.get() != nullptr, TEXT("DirectX11: VertexShader doesn't exist! Setup shaders before creating InputLayout!"));

	//	HRESULT result;
	//	SIZE_T i;
	//	Pawn::Core::Containers::Array<D3D11_INPUT_ELEMENT_DESC> polygonLayout;
	//	ID3D11InputLayout* temp;

	//	DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

	//	polygonLayout.Reserve(layout.GetElements().GetSize() * 4);

	//	i = 0;
	//	for (BufferElement element : layout.GetElements())
	//	{
	//		if (element.Type == ShaderType::Mat3x3)
	//		{
	//			for (uint8 row = 0; row < 4; ++row)
	//			{
	//				Pawn::Core::Containers::AnsiString str = (element.Name + Pawn::Core::Containers::ToAnsiString(row));
	//				ansichar* buf = new ansichar[str.GetSize()];

	//				D3D11_INPUT_ELEMENT_DESC desc = {};
	//				strcpy(buf, str.GetString());

	//				desc.SemanticName = buf;
	//				desc.SemanticIndex = element.SemanticIndex;
	//				desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//				desc.InputSlot = element.InputSlot;
	//				desc.AlignedByteOffset = (uint32)element.Offset + ((uint32)row * 16);
	//				desc.InputSlotClass = (inputSlotClass == InputClassification::PerInstance) ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
	//				desc.InstanceDataStepRate = (inputSlotClass == InputClassification::PerInstance) ? instanceDataStepRate : 0;
	//				polygonLayout.PushBack(desc);
	//			}
	//		}
	//		else if (element.Type == ShaderType::Mat3x3)
	//		{
	//			for (uint8 row = 0; row < 3; ++row)
	//			{
	//				Pawn::Core::Containers::AnsiString str = (element.Name + Pawn::Core::Containers::ToAnsiString(row));
	//				ansichar* buf = new ansichar[str.GetSize()];

	//				D3D11_INPUT_ELEMENT_DESC desc = {};
	//				strcpy(buf, str.GetString());

	//				desc.SemanticName = buf;
	//				desc.SemanticIndex = element.SemanticIndex;
	//				desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//				desc.InputSlot = element.InputSlot;
	//				desc.AlignedByteOffset = (uint32)element.Offset + ((uint32)row * 12);
	//				desc.InputSlotClass = (inputSlotClass == InputClassification::PerInstance) ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
	//				desc.InstanceDataStepRate = (inputSlotClass == InputClassification::PerInstance) ? instanceDataStepRate : 0;
	//				polygonLayout.PushBack(desc);
	//			}
	//		}
	//		else
	//		{
	//			ansichar* buf = new ansichar[element.Name.GetSize()];

	//			D3D11_INPUT_ELEMENT_DESC desc = {};
	//			strcpy(buf, element.Name.GetString());

	//			desc.SemanticName = buf;
	//			desc.SemanticIndex = element.SemanticIndex;
	//			desc.Format = DXGI_FORMAT(ConvertShaderTypeDirectX11(element.Type));
	//			desc.InputSlot = element.InputSlot;
	//			desc.AlignedByteOffset = (uint32)element.Offset;
	//			desc.InputSlotClass = (inputSlotClass == InputClassification::PerInstance) ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
	//			desc.InstanceDataStepRate = (inputSlotClass == InputClassification::PerInstance) ? instanceDataStepRate : 0;
	//			polygonLayout.PushBack(desc);
	//		}
	//	}

	//	result = render->GetDevice()->CreateInputLayout(polygonLayout.Data(), (uint32)polygonLayout.GetSize(),
	//		m_VertexShader->GetBuffer(), m_VertexShader->GetBufferSize(), &temp);

	//	if (FAILED(result))
	//	{
	//		PE_ASSERT(false, TEXT("DirectX11: Pipeline state. Creation failed! Error: {0}"), (uint32)result);
	//		return;
	//	}

	//	m_InputLayout = Pawn::Core::Memory::Scope<ID3D11InputLayout>(temp);
	//}

	//void DirectX11PipelineState::BindInput()
	//{
	//	DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
	//	render->GetDeviceContext()->IASetInputLayout(m_InputLayout.get());
	//}

	//void DirectX11PipelineState::Bind()
	//{
	//	DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

	//	if (m_VertexShader)
	//		m_VertexShader->Bind();
	//	if (m_PixelShader)
	//		m_PixelShader->Bind();
	//	if (m_DomainShader)
	//		m_DomainShader->Bind();
	//	if (m_HullShader)
	//		m_HullShader->Bind();
	//	if (m_GeometryShader)
	//		m_GeometryShader->Bind();

	//	float32 blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };

	//	render->GetDeviceContext()->RSSetViewports(1, &m_Viewport);
	//	render->GetDeviceContext()->RSSetState(m_RasterizerState.get());
	//	render->GetDeviceContext()->OMSetDepthStencilState(m_DepthStencilState.get(), 0);
	//	render->GetDeviceContext()->OMSetBlendState(m_BlendState.get(), blendFactor, 0xFFFFFFFF);
	//}

	//void DirectX11PipelineState::SetPrimitiveTopology(PrimitiveTopology topology, uint8 patchListPointCount)
	//{
	//	DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

	//	switch (topology)
	//	{
	//		case Pawn::Render::PrimitiveTopology::None:
	//		{
	//			render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);
	//			PE_ASSERT(false, TEXT("DirectX11: Primitive topology is none!"));
	//			break;
	//		}
	//		case Pawn::Render::PrimitiveTopology::PointList:
	//		{
	//			render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//			break;
	//		}
	//		case Pawn::Render::PrimitiveTopology::LineList:
	//		{
	//			render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//			break;
	//		}
	//		case Pawn::Render::PrimitiveTopology::LineStrip:
	//		{
	//			render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	//			break;
	//		}
	//		case Pawn::Render::PrimitiveTopology::TriangleList:
	//		{
	//			render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//			break;
	//		}
	//		case Pawn::Render::PrimitiveTopology::TriangleStrip:
	//		{
	//			render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//			break;
	//		}
	//		case Pawn::Render::PrimitiveTopology::TriangleFan:
	//		{
	//			render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//			break;
	//		}
	//		case Pawn::Render::PrimitiveTopology::PatchList:
	//		{
	//			PE_ASSERT(patchListPointCount > 32 || patchListPointCount == 0, TEXT("DirectX11: PipelineState: PatchList greater than 32!"));
	//			render->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY(32 + patchListPointCount));
	//			break;
	//		}
	//	}
	//}

	//void DirectX11PipelineState::SetVertexShader(Pawn::Core::Memory::Reference<Shader> vertexShader)
	//{
	//	DirectX11Shader* raw = dynamic_cast<DirectX11Shader*>(vertexShader.get());
	//	if (!raw)
	//	{
	//		PE_ASSERT(false, TEXT("Shader ptr is invalid!"));
	//		return;
	//	}
	//	if (raw->GetShaderType() != Shader::Type::Vertex)
	//	{
	//		PE_ASSERT(false, TEXT("Assigning {0} shader to vertex!"), (uint32)raw->GetShaderType());
	//		return;
	//	}
	//	m_VertexShader.reset(raw);
	//}

	//void DirectX11PipelineState::SetPixelShader(Pawn::Core::Memory::Reference<Shader> pixelShader)
	//{
	//	DirectX11Shader* raw = dynamic_cast<DirectX11Shader*>(pixelShader.get());
	//	if (!raw)
	//	{
	//		PE_ASSERT(false, TEXT("Shader ptr is invalid!"));
	//		return;
	//	}
	//	if (raw->GetShaderType() != Shader::Type::Pixel)
	//	{
	//		PE_ASSERT(false, TEXT("Assigning {0} shader to pixel!"), (uint32)raw->GetShaderType());
	//		return;
	//	}
	//	m_PixelShader.reset(raw);
	//}

	//void DirectX11PipelineState::SetComputeShader(Pawn::Core::Memory::Reference<Shader> computeShader)
	//{
	//	DirectX11Shader* raw = dynamic_cast<DirectX11Shader*>(computeShader.get());
	//	if (!raw)
	//	{
	//		PE_ASSERT(false, TEXT("Shader ptr is invalid!"));
	//		return;
	//	}
	//	if (raw->GetShaderType() != Shader::Type::Compute)
	//	{
	//		PE_ASSERT(false, TEXT("Assigning {0} shader to compute!"), (uint32)raw->GetShaderType());
	//		return;
	//	}
	//	m_PixelShader.reset(raw);
	//}

	//void DirectX11PipelineState::SetGeometryShader(Pawn::Core::Memory::Reference<Shader> geometryShader)
	//{
	//	DirectX11Shader* raw = dynamic_cast<DirectX11Shader*>(geometryShader.get());
	//	if (!raw)
	//	{
	//		PE_ASSERT(false, TEXT("Shader ptr is invalid!"));
	//		return;
	//	}
	//	if (raw->GetShaderType() != Shader::Type::Geometry)
	//	{
	//		PE_ASSERT(false, TEXT("Assigning {0} shader to geometry!"), (uint32)raw->GetShaderType());
	//		return;
	//	}
	//	m_PixelShader.reset(raw);
	//}

	//void DirectX11PipelineState::SetHullShader(Pawn::Core::Memory::Reference<Shader> hullShader)
	//{
	//	DirectX11Shader* raw = dynamic_cast<DirectX11Shader*>(hullShader.get());
	//	if (!raw)
	//	{
	//		PE_ASSERT(false, TEXT("Shader ptr is invalid!"));
	//		return;
	//	}
	//	if (raw->GetShaderType() != Shader::Type::Hull)
	//	{
	//		PE_ASSERT(false, TEXT("Assigning {0} shader to hull!"), (uint32)raw->GetShaderType());
	//		return;
	//	}
	//	m_PixelShader.reset(raw);
	//}

	//void DirectX11PipelineState::SetDomainShader(Pawn::Core::Memory::Reference<Shader> domainShader)
	//{
	//	DirectX11Shader* raw = dynamic_cast<DirectX11Shader*>(domainShader.get());
	//	if (!raw)
	//	{
	//		PE_ASSERT(false, TEXT("Shader ptr is invalid!"));
	//		return;
	//	}
	//	if (raw->GetShaderType() != Shader::Type::Domain)
	//	{
	//		PE_ASSERT(false, TEXT("Assigning {0} shader to domain!"), (uint32)raw->GetShaderType());
	//		return;
	//	}
	//	m_PixelShader.reset(raw);
	//}

	//void DirectX11PipelineState::BindUniforms(Pawn::Core::Containers::Array<Uniform*>& uniforms, Shader::Type stage)
	//{
	//	if (stage == Shader::Type::None)
	//	{
	//		PE_ASSERT(false, TEXT("None is unsupported!"));
	//		return;
	//	}

	//	uint32 index = 0;
	//	for (Uniform* uniform : uniforms)
	//	{
	//		if (!uniform)
	//		{
	//			PE_ERROR(TEXT("Uniform at index {0} is nullptr!"), index);
	//			continue;
	//		}
	//		uniform->Bind(index, stage);
	//		index++;
	//	}
	//}

	//void DirectX11PipelineState::SetViewport(uint32 x, uint32 y)
	//{
	//	DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

	//	m_Viewport.Width = static_cast<float32>(x);
	//	m_Viewport.Height = static_cast<float32>(y);
	//}

}


//void DirectX11Uniform::Bind(Shader::Type stage)
//{
//	DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
//	ID3D11Buffer* temp = m_Buffer.get();
//	switch (stage)
//	{
//	case Pawn::Render::Shader::Type::Vertex:        render->GetDeviceContext()->VSGetConstantBuffers(0, 1, &temp); return; break;
//	case Pawn::Render::Shader::Type::Pixel:         render->GetDeviceContext()->PSGetConstantBuffers(0, 1, &temp); return; break;
//	case Pawn::Render::Shader::Type::Compute:       render->GetDeviceContext()->CSGetConstantBuffers(0, 1, &temp); return; break;
//	case Pawn::Render::Shader::Type::Geometry:      render->GetDeviceContext()->GSGetConstantBuffers(0, 1, &temp); return; break;
//	case Pawn::Render::Shader::Type::Hull:          render->GetDeviceContext()->HSGetConstantBuffers(0, 1, &temp); return; break;
//	case Pawn::Render::Shader::Type::Domain:        render->GetDeviceContext()->DSGetConstantBuffers(0, 1, &temp); return; break;
//	case Pawn::Render::Shader::Type::None:
//	{
//		PE_ASSERT(false, TEXT("Stage None is unsupported!"));
//		return;
//		break;
//	}
//	}
//	PE_ASSERT(false, TEXT("Stage None is unsupported!"));
//}
//
//void DirectX11Uniform::Unbind(Shader::Type stage)
//{
//	DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
//	switch (stage)
//	{
//	case Pawn::Render::Shader::Type::Vertex:        render->GetDeviceContext()->VSGetConstantBuffers(0, 1, nullptr); return; break;
//	case Pawn::Render::Shader::Type::Pixel:         render->GetDeviceContext()->PSGetConstantBuffers(0, 1, nullptr); return; break;
//	case Pawn::Render::Shader::Type::Compute:       render->GetDeviceContext()->CSGetConstantBuffers(0, 1, nullptr); return; break;
//	case Pawn::Render::Shader::Type::Geometry:      render->GetDeviceContext()->GSGetConstantBuffers(0, 1, nullptr); return; break;
//	case Pawn::Render::Shader::Type::Hull:          render->GetDeviceContext()->HSGetConstantBuffers(0, 1, nullptr); return; break;
//	case Pawn::Render::Shader::Type::Domain:        render->GetDeviceContext()->DSGetConstantBuffers(0, 1, nullptr); return; break;
//	case Pawn::Render::Shader::Type::None:
//	{
//		PE_ASSERT(false, TEXT("Stage None is unsupported!"));
//		return;
//		break;
//	}
//	}
//	PE_ASSERT(false, TEXT("Stage None is unsupported!"));
//}