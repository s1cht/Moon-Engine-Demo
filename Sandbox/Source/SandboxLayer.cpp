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

struct CameraBuffer
{
	Core::Math::Matrix4x4 View;
	Core::Math::Matrix4x4 Proj;
};

void SandboxLayer::OnAttach()
{
	bool result = true;

	result = Assets::AssetManager::Load(Core::IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath")) + Core::Containers::String(TEXT("assets/Meshes/torch.obj")), true);
	if (!result)
	{
		ME_ERROR(TEXT("Failed to load asset!"));
	}

	result = Assets::AssetManager::Load(Core::IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath")) + Core::Containers::String(TEXT("assets/Meshes/flashlight.obj")), true);
	if (!result)
	{
		ME_ERROR(TEXT("Failed to load asset!"));
	}

	Utility::ShaderCompiler& compiler = Utility::ShaderCompiler::Get();
	compiler.InitCompiler();

	ME::Assets::ShaderManager::Get().LoadCompiler();
	ME::Assets::ShaderManager::Get().SetShaderSourcePath(ME::Core::IO::DirectoryStorage::GetDirectory(TEXT("Shaders")));
	ME::Assets::ShaderManager::Get().SetCompiledShaderPath(ME::Core::IO::DirectoryStorage::GetDirectory(TEXT("CompiledShaders")));

	m_Layout = Render::VertexBufferLayout(Render::InputClassification::PerVertex, {
		{ Render::ShaderType::Float3, "POSITION", 0, 0 },
		{ Render::ShaderType::Float2, "TCOORD", 1, 0 },
		{ Render::ShaderType::Float3, "NORMALPOS", 2, 0 }
	});

	Assets::ShaderPaths mainShaders = {};
	mainShaders.Vertex = TEXT("vs_primary.hlsl");
	mainShaders.Pixel = TEXT("ps_primary.hlsl");

	Render::ResourceLayout layoutS1;
	layoutS1.EmplaceBack(
		Render::ResourceType::Uniform,
		Render::ShaderStage::Vertex,
		1);

	layoutS1.EmplaceBack(
		Render::ResourceType::Uniform,
		Render::ShaderStage::Vertex,
		1);

	Render::ResourceLayout layoutS2;
	layoutS2.EmplaceBack(
		Render::ResourceType::Uniform,
		Render::ShaderStage::Pixel,
		1);

	Render::ResourceLayoutPack layoutPack;
	layoutPack.EmplaceBack(layoutS1);
	layoutPack.EmplaceBack(layoutS2);


	Assets::ShaderGroupSpecification shaderGroupSpecification = {};
	shaderGroupSpecification.Paths = mainShaders;
	shaderGroupSpecification.Layout = layoutPack;

	ME::Assets::ShaderManager::Get().LoadShadersFromFiles(TEXT("Main"), shaderGroupSpecification);

	m_Flashlight.Reserve(4);
	m_Flashlight.EmplaceBack(Assets::AssetManager::Get().GetMesh(TEXT("Button")));
	m_Flashlight.EmplaceBack(Assets::AssetManager::Get().GetMesh(TEXT("Flashlight")));
	m_Flashlight.EmplaceBack(Assets::AssetManager::Get().GetMesh(TEXT("Glass")));
	m_Flashlight.EmplaceBack(Assets::AssetManager::Get().GetMesh(TEXT("Ring")));

	ME::Core::Containers::Array<ME::Core::Memory::Reference<Render::Texture2D>> swapChainTextures = Render::RenderCommand::Get()->GetSwapChain()->GetImages();
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
	attachmentSpecs.SampleCount = Render::SampleCount::Count1;

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

	Render::RenderCommand::CreateFramebuffers(m_MainRenderPass);

	Render::InputAssesemblySpecification iaSpec = {};
	iaSpec.Topology = Render::PrimitiveTopology::TriangleList;
	iaSpec.PrimitiveRestart = false;

	Render::RasterizationSpecification rasterSpec = {};
	rasterSpec.DiscardEnabled = false;
	rasterSpec.FrontCounterClockwise = true;
	rasterSpec.DepthBiasEnabled = false;
	rasterSpec.DepthClampEnabled = true;
	rasterSpec.DepthBiasConstantFactor = 1.0f;
	rasterSpec.DepthBiasClamp = 0.f;
	rasterSpec.DepthBiasSlopeFactor = 0.f;
	rasterSpec.LineWidth = 1.0f;
	rasterSpec.Cull = Render::RasterizationCull::Back;
	rasterSpec.Fill = Render::RasterizationFill::Fill;

	Render::MultisamplingSpecification multisamplingSpec = {};
	multisamplingSpec.Samples = Render::SampleCount::Count1;
	multisamplingSpec.EnableSampleShading = true;
	multisamplingSpec.AlphaToCoverage = true;

	Render::DepthStencilSpecification dsSpec = {};
	dsSpec.DepthEnabled = true;
	dsSpec.StencilEnabled = false;
	dsSpec.MaxDepthBounds = 1.0f;
	dsSpec.MinDepthBounds = 0.f;
	dsSpec.DepthFunction = Render::DepthComparison::LessEqual;

	Render::ColorBlendingSpecification cbSpec = {};
	cbSpec.Attachments = { {false, false  } };
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

	m_WindowHeight = Application::Get().GetWindow()->GetHeight();
	m_WindowWidth = Application::Get().GetWindow()->GetWidth();

	Render::VertexBufferSpecification vS = {};
	vS.Size = sizeof(float32) * 24 * 8;
	vS.DebugName = TEXT("ad");

	Render::IndexBufferSpecification iS = {};
	iS.IndexCount = 36;
	iS.DebugName = TEXT("as");

	m_VertexBuffer = Render::VertexBuffer::Create(vS);
	m_IndexBuffer = Render::IndexBuffer::Create(iS);

	Render::UniformSpecification cameraBufferSpec = {};
	cameraBufferSpec.SetIndex = Render::RenderCommand::GetResourceHandler()->CreateResourceSet(layoutS1);
	cameraBufferSpec.Set = 0;
	cameraBufferSpec.Binding = 0;
	cameraBufferSpec.Layout = layoutS1;
	cameraBufferSpec.DebugName = TEXT("Camera buffer");
	cameraBufferSpec.MemoryType = Render::MemoryType::RAM;
	cameraBufferSpec.Size = sizeof(CameraBuffer);
	cameraBufferSpec.Stage = Render::ShaderStage::Vertex;
	m_CameraBuffer = Render::RUniform::Create(cameraBufferSpec);

	Render::UniformSpecification sceneBufferSpec = {};
	sceneBufferSpec.SetIndex = cameraBufferSpec.SetIndex;
	sceneBufferSpec.Set = 0;
	sceneBufferSpec.Binding = 1;
	sceneBufferSpec.Layout = layoutS1;
	sceneBufferSpec.DebugName = TEXT("Scene buffer");
	sceneBufferSpec.MemoryType = Render::MemoryType::VRAM;
	sceneBufferSpec.Size = sizeof(Core::Math::Matrix4x4);
	sceneBufferSpec.Stage = Render::ShaderStage::Vertex;
	m_SceneBuffer = Render::RUniform::Create(sceneBufferSpec);

	Render::UniformSpecification lightBufferSpec = {};
	lightBufferSpec.SetIndex = Render::RenderCommand::GetResourceHandler()->CreateResourceSet(layoutS2);
	lightBufferSpec.Set = 1;
	lightBufferSpec.Binding = 0;
	lightBufferSpec.Layout = layoutS2;
	lightBufferSpec.DebugName = TEXT("Light buffer");
	lightBufferSpec.MemoryType = Render::MemoryType::RAM;
	lightBufferSpec.Size = sizeof(Light);
	lightBufferSpec.Stage = Render::ShaderStage::Pixel;
	m_LightBuffer = Render::RUniform::Create(lightBufferSpec);

	m_Light = { ME::Core::Math::Vector4D32(1), ME::Core::Math::Vector3D32(1), 1.f};
	m_WorldMatrix = Core::Math::Matrix4x4(1);

	m_Camera = Core::Memory::Reference<Render::Camera::Camera>(new Render::Camera::PerpectiveCamera());
	m_Camera->SetFOV(120.f);
	m_Camera->SetAspectRatio(1920.f / 1080.f);
	m_Camera->SetPosition(m_Camera->GetPosition() + Core::Math::Vector3D(-10, -10, 0));
	m_Camera->SetRotation(RAD(45), RAD(45), 0);
}

Core::Math::Vector3D calcPos(float64 x, float64 y)
{
	Core::Math::Vector3D v = Core::Math::Vector3D(
		cos(RAD(x)) * cos(RAD(y)),
		sin(RAD(y)),
		sin(RAD(x)) * cos(RAD(y))
	);
	float64 length_of_v = sqrt((v.X * v.X) + (v.Y * v.Y) + (v.Z * v.Z));
	return Core::Math::Vector3D(v.X / length_of_v, v.Y / length_of_v, v.Z / length_of_v);
}

void SandboxLayer::OnUpdate(float64 deltaTime)
{
	OnKeyInputStartedEvent(deltaTime);
	m_Camera->Update();

	Render::RenderCommand::NewFrame();
	auto camBuf = m_CameraBuffer->AcquireNextBuffer();
	auto sceBuf = m_SceneBuffer->AcquireNextBuffer();
	auto ligBuf = m_LightBuffer->AcquireNextBuffer();

	ME::Core::Memory::Reference<ME::Render::CommandBuffer> currentCommandBuffer = Render::RenderCommand::GetAvailableCommandBuffer();
	currentCommandBuffer->Reset();

	Render::ClearValue clrVal = {};
	clrVal.ColorClearValue = Core::Math::Vector4D32(0.f, 0.f, 0.f, 1.f);

	Render::RenderPassBeginInfo beginInfo = {};
	beginInfo.RenderPass = m_MainRenderPass;
	beginInfo.Framebuffer = Render::RenderCommand::GetAvailableFramebuffer();
	beginInfo.ClearValues = { clrVal, clrVal };
	beginInfo.RenderArea = {};
	beginInfo.RenderArea.Offset = { 0,0 };
	beginInfo.RenderArea.Extent = Render::RenderCommand::Get()->GetSwapChain()->GetExtent();

	currentCommandBuffer->Record();

	if (m_CameraBufferUpdateRequired)
	{
		CameraBuffer camS = {};
		camS.View = m_Camera->GetViewMatrix();
		camS.Proj = m_Camera->GetProjectionMatrix();
		camBuf->SetData(currentCommandBuffer, &camS, sizeof(CameraBuffer));
		Render::RenderCommand::WriteResource(sceBuf);
		m_CameraBufferUpdateRequired = false;
	}
	ligBuf->SetData(currentCommandBuffer, &m_Light, sizeof(Light));
	sceBuf->SetData(currentCommandBuffer, &m_WorldMatrix, sizeof(Core::Math::Matrix4x4));

	Render::RenderCommand::WriteResource(camBuf);
	Render::RenderCommand::WriteResource(ligBuf);

	static Core::Math::Rect2D scissor = {};
	scissor.Extent = Render::RenderCommand::Get()->GetSwapChain()->GetExtent();
	scissor.Offset.x = 0;
	scissor.Offset.y = 0;

	for (const auto& mesh : m_Flashlight)
	{
		mesh->UpdateBuffers(currentCommandBuffer);
	}

	// Main render pass

	Render::RenderCommand::BeginRenderPass(currentCommandBuffer, beginInfo);

	m_Primary->SetViewports(currentCommandBuffer, { {0,0,m_WindowWidth, m_WindowHeight, 0, 1} });
	m_Primary->SetScissors(currentCommandBuffer, { scissor });
	m_Primary->Bind(currentCommandBuffer);

	Render::RenderCommand::BindResourceSet(currentCommandBuffer, m_Primary, camBuf);
	Render::RenderCommand::BindResourceSet(currentCommandBuffer, m_Primary, sceBuf);
	Render::RenderCommand::BindResourceSet(currentCommandBuffer, m_Primary, ligBuf);

	for (const auto& mesh : m_Flashlight)
	{
		mesh->Bind(currentCommandBuffer);
		Render::RenderCommand::DrawIndexed(currentCommandBuffer, mesh->GetIndexBuffer()->GetCount(), 0);
	}

	Render::RenderCommand::EndRenderPass(currentCommandBuffer);

	currentCommandBuffer->Finish();
	Render::RenderCommand::Submit(currentCommandBuffer);

	Render::RenderCommand::Present();
}

void SandboxLayer::OnEvent(Core::Event& event)
{
	ME::Core::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<Events::WindowResizedEvent>(BIND_EVENT_FN(SandboxLayer::UpdateRender));
	dispatcher.Dispatch<Events::MouseMovedEvent>(BIND_EVENT_FN(SandboxLayer::OnMouseMovedEvent));
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

bool SandboxLayer::UpdateRender(ME::Events::WindowResizedEvent& event)
{
	m_WindowWidth = static_cast<uint32>(event.GetSizeX());
	m_WindowHeight = static_cast<uint32>(event.GetSizeY());

	return false;
}

bool SandboxLayer::OnMouseMovedEvent(ME::Events::MouseMovedEvent& event)
{
	if (Input::InputController::IsMouseRightButtonDown())
	{
		static float32 yaw, pitch = 0.f;
		yaw -= RAD(event.GetDeltaX() * m_MouseSensitivity);
		pitch -= RAD(event.GetDeltaY() * m_MouseSensitivity);
		m_Camera->SetRotation(yaw, pitch, 0.f);
		m_CameraBufferUpdateRequired = true;
	}
	return false;
}

//bool OnKeyInputStartedEvent(ME::Events::KeyInputStartedEvent& event)
bool SandboxLayer::OnKeyInputStartedEvent(float32 deltaTime)
{
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_W))
	{
		m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetLookVector() * static_cast<float64>(m_CameraSpeed) * deltaTime);
		m_CameraBufferUpdateRequired = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_S))
	{
		m_Camera->SetPosition(m_Camera->GetPosition() - m_Camera->GetLookVector() * (static_cast<float64>(m_CameraSpeed) * deltaTime));
		m_CameraBufferUpdateRequired = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_A))
	{
		m_Camera->SetPosition(m_Camera->GetPosition() - m_Camera->GetRightVector() * (static_cast<float64>(m_CameraSpeed) * deltaTime));
		m_CameraBufferUpdateRequired = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_D))
	{
		m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetRightVector() * (static_cast<float64>(m_CameraSpeed) * deltaTime));
		m_CameraBufferUpdateRequired = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_SPACE))
	{
		m_Camera->SetPosition(m_Camera->GetPosition() - m_Camera->GetUpVector() * (static_cast<float64>(m_CameraSpeed) * deltaTime));
		m_CameraBufferUpdateRequired = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_LEFTCONTROL))
	{
		m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetUpVector() * (static_cast<float64>(m_CameraSpeed) * deltaTime));
		m_CameraBufferUpdateRequired = true;
	}
	m_CameraBufferUpdateRequired = true;
	return false;
}
