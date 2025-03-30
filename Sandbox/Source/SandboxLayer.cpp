#include "SandboxLayer.h"
#include <Application/Application.h>
#include <Renderer/Renderer.h>
#include <Renderer/RenderCommand.h>
#include <Assets/Mesh.h>

using namespace Pawn;

void SandboxLayer::OnAttach()
{
	//Core::Memory::Reference<Assets::Mesh> mesh = Core::Memory::Reference<Assets::Mesh>(new Assets::Mesh());

	//Utility::AssetLoader::Load(Core::IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath")) + String(TEXT("assets/Meshes/torch.obj")));

	Render::BufferLayout layout = {
		{ Render::ShaderType::Float3, "POSITION", 0, 0 },
		{ Render::ShaderType::Float4, "COLOR", 0, 0 }
	};

	m_WindowWidth = (uint32)Application::Get().GetWindow()->GetWidth();
	m_WindowHeight = (uint32)Application::Get().GetWindow()->GetHeight();

	m_Primary = Core::Memory::Reference<Render::PipelineState>(Render::PipelineState::Create());

	Render::Shader::SetShaderSourceExtension(TEXT(".hlsl"));
	Render::Shader::SetCompiledShaderExtension(TEXT(".cso"));

	m_VertexShader = Core::Memory::Reference<Render::Shader>(Render::Shader::CreateShader(TEXT("vs_primary"), Render::Shader::Type::Vertex, true));
	m_PixelShader = Core::Memory::Reference<Render::Shader>(Render::Shader::CreateShader(TEXT("ps_primary"), Render::Shader::Type::Pixel, true));

	m_Primary->SetViewport(m_WindowWidth, m_WindowHeight);
	m_Primary->SetVertexShader(m_VertexShader);
	m_Primary->SetPixelShader(m_PixelShader);
	m_Primary->SetDepthStencilState(false, true, Render::DepthComparison::Greater);
	m_Primary->SetRasterizerState(Render::RasterizerCull::Back, Render::RasterizerFill::Fill, true, false, true, false, 0, 0.f, false, 0);


	m_Primary->SetBlendState(true, Render::BlendMask::All);
	m_Primary->SetInputLayout(layout, Pawn::Render::InputClassification::PerVertex, 0);

	const float32 points[4][7] =
	{
		{ -0.5f, -0.5f, 0.f,    1.f, 1.f, 1.f, 1.f },
		{  0.5f, -0.5f, 0.f,    0.5f, 0.5f, 0.5f, 1.f },
		{  0.5f,  0.5f, 0.f,    1.f, 1.f, 0.f, 1.f },
		{ -0.5f,  0.5f, 0.f,    0.f, 1.f, 1.f, 1.f },
	};

	const uint32 indeces[6] = { 0, 1, 2, 0, 2, 3 };

	m_VertexBuffer = Core::Memory::Reference<Render::VertexBuffer>(Render::VertexBuffer::Create((void*)points, sizeof(float32) * 4 * 7, Render::Usage::Default));
	m_IndexBuffer = Core::Memory::Reference<Render::IndexBuffer>(Render::IndexBuffer::Create((void*)indeces, 6, Render::Usage::Default));
}

void SandboxLayer::OnUpdate(float64 deltaTime)
{
	Render::BufferLayout layout = {
		{ Render::ShaderType::Float3, "POSITION", 0, 0 },
		{ Render::ShaderType::Float4, "COLOR", 0, 0 }
	};

	D3DPERF_BeginEvent(0xffffffff, TEXT("Start"));
	m_Primary->SetPrimitiveTopology(Pawn::Render::PrimitiveTopology::TriangleList, 0);
	m_Primary->BindInput();
	m_VertexBuffer->Bind(layout);
	m_IndexBuffer->Bind();
	m_Primary->Bind();
	Render::RenderCommand::BindBackBuffer();

	Render::Renderer::Submit(m_IndexBuffer->GetCount(), 0);
	D3DPERF_EndEvent();
}

void SandboxLayer::OnEvent(Core::Event& event)
{
	Pawn::Core::EventDispatcher dispathcher(event);
	dispathcher.Dispatch<Events::WindowResizedEvent>(BIND_EVENT_FN(SandboxLayer::SetViewportSize));

	m_Primary->SetViewport(m_WindowWidth, m_WindowHeight);
}

bool SandboxLayer::SetViewportSize(Pawn::Events::WindowResizedEvent& event)
{
	m_WindowWidth = (uint32)event.GetSizeX();
	m_WindowHeight = (uint32)event.GetSizeY();

	m_Primary->SetViewport(m_WindowWidth, m_WindowHeight);

	return false;
}
