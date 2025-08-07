#include "SandboxLayer.h"
#include <Core/Platform/Base/IO.hpp>
#include <Application/Application.h>
#include <Renderer/Renderer.h>
#include <Renderer/RenderCommand.h>
#include <Renderer/Base/CommandBuffer.h>
#include <Renderer/Base/RenderPass.h>
#include <Renderer/Base/Framebuffer.h>
#include <Renderer/ImGui/ImGuiReferences.h>
#include <Renderer/Camera/PerpectiveCamera.h>

#include <Utility/ShaderCompiler.hpp>
#include <Assets/AssetManager.h>

#include "Assets/ShaderManager.hpp"

using namespace ME;

void SandboxLayer::OnAttach()
{
	bool result = true;

//	result = Assets::AssetManager::Load(Core::IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath")) + Core::Containers::String(TEXT("assets/Meshes/torch.obj")), true);
	if (!result)
	{
		ME_ERROR(TEXT("Failed to load asset!"));
	}

	//result = Assets::AssetManager::Load(Core::IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath")) + Core::Containers::String(TEXT("assets/Meshes/flashlight.obj")), true);
	if (!result)
	{
		ME_ERROR(TEXT("Failed to load asset!"));
	}

	Utility::ShaderCompiler& compiler = Utility::ShaderCompiler::Get();
	compiler.InitCompiler();

	ME::Assets::ShaderManager::Get().LoadCompiler();
	ME::Assets::ShaderManager::Get().SetShaderSourcePath(ME::Core::IO::DirectoryStorage::GetDirectory(TEXT("Shaders")));
	ME::Assets::ShaderManager::Get().SetCompiledShaderPath(ME::Core::IO::DirectoryStorage::GetDirectory(TEXT("CompiledShaders")));

	//m_Layout = Render::VertexBufferLayout(Render::InputClassification::PerVertex, {
	//{ Render::ShaderType::Float3, "POSITION", 0, 0 },
	//{ Render::ShaderType::Float2, "TCOORD", 1, 0 },
	//{ Render::ShaderType::Float3, "NORMALPOS", 2, 0 }
	//	});

	m_Layout = Render::VertexBufferLayout(Render::InputClassification::PerVertex, {});

	Assets::ShaderGroupPaths mainShaders = {};
	mainShaders.Vertex = TEXT("vs.hlsl");
	mainShaders.Pixel = TEXT("ps.hlsl");

	ME::Assets::ShaderManager::Get().LoadShadersFromFiles(TEXT("Main"), mainShaders);

	m_Flashlight.Reserve(4);
	m_Flashlight.EmplaceBack(Assets::AssetManager::Get().GetMesh(TEXT("Button")));
	m_Flashlight.EmplaceBack(Assets::AssetManager::Get().GetMesh(TEXT("Flashlight")));
	m_Flashlight.EmplaceBack(Assets::AssetManager::Get().GetMesh(TEXT("Glass")));
	m_Flashlight.EmplaceBack(Assets::AssetManager::Get().GetMesh(TEXT("Ring")));

	ME::Core::Containers::Array<Render::Texture2D*> swapChainTextures = Render::RenderCommand::Get()->GetSwapChain()->GetImages();
	ME::Core::Containers::Array<Render::AttachmentSpecification> attachments;

	auto imageSpec =  swapChainTextures[0]->GetSpecification();
	Render::AttachmentSpecification attachmentSpecs = {};
	attachmentSpecs.IsStencil = false;
	attachmentSpecs.IsDepth = false;
	attachmentSpecs.AttachmentFormat = imageSpec.Format;
	attachmentSpecs.LoadOp = Render::LoadOperation::Clear;
	attachmentSpecs.StoreOp = Render::StoreOperation::Store;
	attachmentSpecs.InitialLayout = Render::ImageLayout::Undefined;
	attachmentSpecs.FinalLayout = Render::ImageLayout::Present;

	attachments.EmplaceBack(attachmentSpecs);

	Render::SubpassSpecification subpass = {};
	subpass.ColorAttachmentRefs = { 0 };
	subpass.DepthStencilAttachmentRef = ~0u;
	subpass.PipelineBindPoint = Render::PipelineBindPoint::Graphics;

	Render::SubpassDependency subpassDependency = {};
	subpassDependency.SubpassSrc = ~0u;
	subpassDependency.AccessFlagsDst = Render::AccessFlags::DepthStencilWrite | Render::AccessFlags::ColorAttachmentWrite;
	subpassDependency.PipelineStageFlagsSrc = Render::PipelineStageFlags::ColorAttachmentOutput | Render::PipelineStageFlags::EarlyFragmentTests;
	subpassDependency.PipelineStageFlagsDst = Render::PipelineStageFlags::ColorAttachmentOutput | Render::PipelineStageFlags::EarlyFragmentTests;

	Render::RenderPassSpecification mainRPSpecs = {};
	mainRPSpecs.AttachmentSpecs = attachments;
	mainRPSpecs.SubpassSpecs = { subpass };
	mainRPSpecs.SubpassDependencies = { subpassDependency };
	mainRPSpecs.DebugName = "Main render pass";

	m_MainRenderPass = Render::RenderPass::Create(mainRPSpecs);

	for (auto image : swapChainTextures)
	{
		Render::FramebufferSpecification framebufferSpecification = {};
		framebufferSpecification.Attachments = { image };
		framebufferSpecification.Layers = 1;
		framebufferSpecification.RenderPass = m_MainRenderPass;
		framebufferSpecification.Resolution.x = image->GetResolution().x;
		framebufferSpecification.Resolution.y = image->GetResolution().y;

		m_WindowFramebuffers.EmplaceBack(Render::Framebuffer::Create(framebufferSpecification));
	}

	
	Render::InputAssesemblySpecification iaSpec = {};
	iaSpec.Topology = Render::PrimitiveTopology::TriangleList;
	iaSpec.PrimitiveRestart = false;

	Render::RasterizationSpecification rasterSpec = {};
	rasterSpec.DiscardEnabled = false;
	rasterSpec.FrontCounterClockwise = false;
	rasterSpec.DepthBiasEnabled = false;
	rasterSpec.DepthClampEnabled = true;
	rasterSpec.DepthBiasConstantFactor = 1.f;
	rasterSpec.DepthBiasClamp = 0.f;
	rasterSpec.DepthBiasSlopeFactor = 0.f;
	rasterSpec.LineWidth = 1.0f;
	rasterSpec.Cull = Render::RasterizationCull::None;
	rasterSpec.Fill = Render::RasterizationFill::Fill;

	Render::MultisamplingSpecification multisamplingSpec = {};
	multisamplingSpec.Samples = Render::SampleCount::Count1;
	multisamplingSpec.EnableSampleShading = false;
	multisamplingSpec.AlphaToCoverage = true;

	Render::DepthStencilSpecification dsSpec = {};
	dsSpec.DepthEnabled = false;
	dsSpec.StencilEnabled = false;
	dsSpec.MaxDepthBounds = 1.0f;
	dsSpec.MinDepthBounds = 0.f;
	dsSpec.DepthFunction = Render::DepthComparison::LessEqual;

	Render::ColorBlendingSpecification cbSpec = {};
	cbSpec.Attachments = { {false, true  } };
	cbSpec.BlendConstants = { 1.f, 1.f, 1.f, 1.f };
	cbSpec.LogicOperation = Render::LogicOperation::None;

	Render::PipelineSpecification mainPipelineSpec = {};
	mainPipelineSpec.Type = Render::PipelineType::Graphics;
	mainPipelineSpec.RenderPass = m_MainRenderPass;
	mainPipelineSpec.Subpass = 0;
	mainPipelineSpec.Shaders = ME::Assets::ShaderManager::Get().GetShaderGroup(TEXT("Main"));
	mainPipelineSpec.BufferLayout = m_Layout;
	mainPipelineSpec.InputAssembly = iaSpec;
	mainPipelineSpec.Rasterization = rasterSpec;
	mainPipelineSpec.Multisampling = multisamplingSpec;
	mainPipelineSpec.DepthStencil = dsSpec;
	mainPipelineSpec.ColorBlending = cbSpec;
 	mainPipelineSpec.BasePipeline = nullptr;

	m_Primary = Render::Pipeline::Create(mainPipelineSpec);

	/*

	Render::VertexBufferLayout layout = {
		{ Render::ShaderType::Float3, "POSITION", 0, 0 },
		{ Render::ShaderType::Float2, "TCOORD", 0, 0 },
		{ Render::ShaderType::Float3, "NORMALPOS", 0, 0 }
	};

	m_WindowWidth = (uint32)Application::Get().GetWindow()->GetWidth();
	m_WindowHeight = (uint32)Application::Get().GetWindow()->GetHeight();

	m_Primary = Core::Memory::Reference<Render::Pipeline>(Render::Pipeline::Create());

	Render::Shader::SetShaderSourceExtension(TEXT(".hlsl"));
	Render::Shader::SetCompiledShaderExtension(TEXT(".cso"));

	m_VertexShader = Core::Memory::Reference<Render::Shader>(Render::Shader::Create(TEXT("vs_primary"), Render::Shader::Type::Vertex, false));
	m_PixelShader = Core::Memory::Reference<Render::Shader>(Render::Shader::Create(TEXT("ps_primary"), Render::Shader::Type::Pixel, false));

	const float32 points[8][7] = 
	{
		{ -0.5f, -0.5f, -0.5f,   1.f, 0.f, 0.f, 1.f },
		{  0.5f, -0.5f, -0.5f,   0.f, 1.f, 0.f, 1.f },
		{  0.5f,  0.5f, -0.5f,   0.f, 0.f, 1.f, 1.f },
		{ -0.5f,  0.5f, -0.5f,   1.f, 1.f, 0.f, 1.f },
		{ -0.5f, -0.5f,  0.5f,   1.f, 0.f, 1.f, 1.f },
		{  0.5f, -0.5f,  0.5f,   0.f, 1.f, 1.f, 1.f },
		{  0.5f,  0.5f,  0.5f,   1.f, 1.f, 1.f, 1.f },
		{ -0.5f,  0.5f,  0.5f,   0.f, 0.f, 0.f, 1.f },
	};


	const uint32 indices[36] = 
	{
		4, 5, 6,
		4, 6, 7,

		1, 0, 3,
		1, 3, 2,

		0, 4, 7,
		0, 7, 3,

		5, 1, 2,
		5, 2, 6,

		3, 7, 6,
		3, 6, 2,

		0, 1, 5,
		0, 5, 4
	};


	m_VertexBuffer = Core::Memory::Reference<Render::VertexBuffer>(Render::VertexBuffer::Create((void*)points, sizeof(float32) * 8 * 7, Render::Usage::Default));
	m_IndexBuffer = Core::Memory::Reference<Render::IndexBuffer>(Render::IndexBuffer::Create((void*)indices, 36, Render::Usage::Default));

	m_Primary->SetViewport(m_WindowWidth, m_WindowHeight);
	m_Primary->SetVertexShader(m_VertexShader);
	m_Primary->SetPixelShader(m_PixelShader);
	m_Primary->SetDepthStencilState(true, true, Render::DepthComparison::LessEqual);

	m_Primary->SetBlendState(false, Render::BlendMask::All);
	m_Primary->SetInputLayout(layout, Pawn::Render::InputClassification::PerVertex, 0);

	m_CameraBuffer = Core::Memory::Reference<Render::Uniform>(Render::Uniform::Create(sizeof(Core::Math::Matrix4x4), Render::Usage::Dynamic));
	m_SceneBuffer = Core::Memory::Reference<Render::Uniform>(Render::Uniform::Create(sizeof(Core::Math::Matrix4x4), Render::Usage::Dynamic));
	m_LightBuffer = Core::Memory::Reference<Render::Uniform>(Render::Uniform::Create(sizeof(Light), Render::Usage::Dynamic));

	m_Light = { Pawn::Core::Math::Vector4D32(1), Pawn::Core::Math::Vector3D32(1), 1.f};
	m_WorldMatrix = Core::Math::Matrix4x4(1);

	m_Camera = Core::Memory::Reference<Render::Camera::Camera>(new Render::Camera::PerpectiveCamera());
	m_Camera->SetFOV(90.f);
	m_Camera->SetAspectRatio(1920.f / 1080.f);
	m_Camera->SetPosition(Core::Math::Vector3D(0));
	m_Camera->SetRotation(0, 0, 0);
	*/
}

void SandboxLayer::OnUpdate(float64 deltaTime)
{
	//auto camPos = m_Camera->GetPosition();
	//static float32 yaw = 0.f;
	//static float32 pitch = 0.f;
	//static const float32 cameraSpeed = 0.001f;
	//static const float32 cameraSens = 0.001f;
	//static bool update = false;

	//if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_W))
	//{
	//	camPos += Core::Math::Vector3D::ForwardVector * ((float64)cameraSpeed * deltaTime);
	//	update = true;
	//}
	//if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_S))
	//{
	//	camPos += Core::Math::Vector3D::BackwardVector * ((float64)cameraSpeed * deltaTime);
	//	update = true;
	//}
	//if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_A))
	//{
	//	camPos += Core::Math::Vector3D::LeftVector * ((float64)cameraSpeed * deltaTime);
	//	update = true;
	//}
	//if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_D))
	//{
	//	camPos += Core::Math::Vector3D::RightVector * ((float64)cameraSpeed * deltaTime);
	//	update = true;
	//}
	//if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_E))
	//{
	//	camPos += Core::Math::Vector3D::UpVector * ((float64)cameraSpeed * deltaTime);
	//	update = true;
	//}
	//if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_Q))
	//{
	//	camPos += Core::Math::Vector3D::DownVector * ((float64)cameraSpeed * deltaTime);
	//	update = true;
	//}

	//if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_UP))
	//{
	//	yaw += cameraSens * deltaTime;
	//	update = true;
	//}
	//if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_DOWN))
	//{
	//	yaw -= cameraSens * deltaTime;
	//	update = true;
	//}
	//if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_LEFT))
	//{
	//	pitch += cameraSens * deltaTime;
	//	update = true;
	//}
	//if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_RIGHT))
	//{
	//	pitch -= cameraSens * deltaTime;
	//	update = true;
	//}

	//if (update)
	//{
	//	m_Camera->SetRotation(yaw, pitch, yaw);
	//	m_Camera->SetPosition(camPos);
	//	update = false;
	//}

	//m_Camera->Update();

	//Core::Math::Matrix4x4 mat = m_Camera->GetProjViewMatrix().Transpose();

	ME::Core::Memory::Reference<ME::Render::CommandBuffer> currentCommandBuffer = Render::RenderCommand::GetAvailableCommandBuffer();
	currentCommandBuffer->Reset();

	Render::ClearValue clrVal = {};
	clrVal.ColorClearValue = Core::Math::Vector4D32(0.f, 0.f, 0.f, 1.f);

	Render::RenderPassBeginInfo beginInfo = {};
	beginInfo.RenderPass = m_MainRenderPass;
	beginInfo.Framebuffer = m_WindowFramebuffers[Render::RenderCommand::GetFrameIndex()];
	beginInfo.ClearValues = { clrVal, clrVal };
	beginInfo.RenderArea = {};
	beginInfo.RenderArea.Offset = { 0,0 };
	beginInfo.RenderArea.Extent = Render::RenderCommand::Get()->GetSwapChain()->GetExtent();

	currentCommandBuffer->Record();

	Render::RenderCommand::BeginRenderPass(currentCommandBuffer, beginInfo);
	//D3DPERF_BeginEvent(0xffffffff, TEXT("Flashlight render"));
	//m_CameraBuffer->SetData(const_cast<void*>(static_cast<const void*>(&mat)), sizeof(Core::Math::Matrix4x4));
	//m_SceneBuffer->SetData(static_cast<void*>(&m_WorldMatrix), sizeof(Core::Math::Matrix4x4));
	//m_LightBuffer->SetData(static_cast<void*>(&m_Light), sizeof(Light));

	//m_CameraBuffer->Bind(0, Render::Shader::Type::Vertex);
	//m_SceneBuffer->Bind(1, Render::Shader::Type::Vertex);
	//m_LightBuffer->Bind(0, Render::Shader::Type::Pixel);


	static Core::Math::Rect2D scissor = {};
	scissor.Extent = Render::RenderCommand::Get()->GetSwapChain()->GetExtent();
	scissor.Offset.x = 0;
	scissor.Offset.y = 0;

	m_Primary->SetViewports(currentCommandBuffer, { {0,0,1920, 1050, 0, 1} });
	m_Primary->SetScissors(currentCommandBuffer, { scissor });

	m_Primary->Bind(currentCommandBuffer);

	Render::RenderCommand::Draw(currentCommandBuffer, 3, 1);

	for (const auto& mesh : m_Flashlight)
	{
		//mesh->GetVertexBuffer()->Bind(m_Layout);
		//mesh->GetIndexBuffer()->Bind();
		//Render::Renderer::Submit(mesh->GetIndexBuffer()->GetCount(), 0);
	}

	Render::RenderCommand::EndRenderPass(currentCommandBuffer);

	currentCommandBuffer->Finish();
	Render::RenderCommand::NewFrame();
	Render::RenderCommand::Submit(currentCommandBuffer);

	Render::RenderCommand::Present();

	//D3DPERF_EndEvent();
}

void SandboxLayer::OnEvent(Core::Event& event)
{
	ME::Core::EventDispatcher dispathcher(event);
	dispathcher.Dispatch<Events::WindowResizedEvent>(BIND_EVENT_FN(SandboxLayer::SetViewportSize));

	//m_Primary->SetViewport(m_WindowWidth, m_WindowHeight);
}

void SandboxLayer::OnImGuiRender(float64 deltaTime, ImGuiContext* dllContext)
{
	ImGui::SetCurrentContext(dllContext);
	static bool statsVisible = true;
	static bool settsVisible = true;

	if (ImGui::Begin("Camera stats", &statsVisible))
	{
		ImGui::Text("CameraPosition: %.5f, %.5f, %.5f", m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z);
		ImGui::Text("------------");
		ImGui::Text("Camera view matrix:");
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetViewMatrix().a11, m_Camera->GetViewMatrix().a21, m_Camera->GetViewMatrix().a31, m_Camera->GetViewMatrix().a41);
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetViewMatrix().a12, m_Camera->GetViewMatrix().a22, m_Camera->GetViewMatrix().a32, m_Camera->GetViewMatrix().a42);
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetViewMatrix().a13, m_Camera->GetViewMatrix().a23, m_Camera->GetViewMatrix().a33, m_Camera->GetViewMatrix().a43);
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetViewMatrix().a14, m_Camera->GetViewMatrix().a24, m_Camera->GetViewMatrix().a34, m_Camera->GetViewMatrix().a44);

		ImGui::Text("------------");
		ImGui::Text("Camera projection matrix:");
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetProjectionMatrix().a11, m_Camera->GetProjectionMatrix().a21, m_Camera->GetProjectionMatrix().a31, m_Camera->GetProjectionMatrix().a41);
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetProjectionMatrix().a12, m_Camera->GetProjectionMatrix().a22, m_Camera->GetProjectionMatrix().a32, m_Camera->GetProjectionMatrix().a42);
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetProjectionMatrix().a13, m_Camera->GetProjectionMatrix().a23, m_Camera->GetProjectionMatrix().a33, m_Camera->GetProjectionMatrix().a43);
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetProjectionMatrix().a14, m_Camera->GetProjectionMatrix().a24, m_Camera->GetProjectionMatrix().a34, m_Camera->GetProjectionMatrix().a44);
	}

	ImGui::End();

	static int32 CameraFOV = 70.f;
	static float32 CameraNear = 0.1f;
	static float32 CameraFar = 1000.f;
	static float32 CameraAspectRatioX = 1920.f;
	static float32 CameraAspectRatioY = 1080.f;

	if (ImGui::Begin("Camera settings", &settsVisible))
	{
		if (ImGui::DragInt("Camera FOV", &CameraFOV, 1.f, 10, 120))
		{
			m_Camera->SetFOV(CameraFOV);
		}
		if (ImGui::DragFloat("Camera near", &CameraNear, 1.f, 0, 100.f))
		{
			m_Camera->SetNear(CameraNear);
		}
		if (ImGui::DragFloat("Camera far", &CameraFar, 0.1f, 0, 100000.f))
		{
			m_Camera->SetFar(CameraFar);
		}
		if (ImGui::DragFloat("Camera AR X", &CameraAspectRatioX, 0.1f, 0, 8120.f))
		{
			m_Camera->SetAspectRatio(CameraAspectRatioX / CameraAspectRatioY);
		}
		if (ImGui::DragFloat("Camera AR Y", &CameraAspectRatioY, 0.1f, 0, 8120.f))
		{
			m_Camera->SetAspectRatio(CameraAspectRatioX / CameraAspectRatioY);
		}
	}

	ImGui::End();

	static bool lightUpdate = false;
		
	if (ImGui::Begin("Light settings", &settsVisible))
	{
		if (ImGui::DragFloat4("Color", m_Light.Color.XYZW, 0.01f, 0.f, 1.f))
		{
			lightUpdate = true;
		}
		if (ImGui::DragFloat3("Direction", m_Light.Direction.XYZ, 0.01f, -1.f, 1.f))
		{
			lightUpdate = true;
		}
	}

	ImGui::End();
}

bool SandboxLayer::SetViewportSize(ME::Events::WindowResizedEvent& event)
{
	m_WindowWidth = (uint32)event.GetSizeX();
	m_WindowHeight = (uint32)event.GetSizeY();

	//m_Primary->SetViewport(m_WindowWidth, m_WindowHeight);

	return false;
}
