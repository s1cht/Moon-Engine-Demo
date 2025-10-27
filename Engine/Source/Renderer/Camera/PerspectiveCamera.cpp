#include "PerspectiveCamera.hpp"

namespace ME::Render::Camera
{
	ME::Render::Camera::PerspectiveCamera::PerspectiveCamera() :
		m_FOV(90),
		m_AspectRatio(static_cast<float32>(1920) / 1080),
		m_Near(0.1f), m_Far(1000.f), m_Transform({ 0, 0, 0 }), m_ProjectionMatrix(ME::Core::Math::Matrix4x4()),
		m_ProjViewMatrix(ME::Core::Math::Matrix4x4()),
		m_UpdateQueued(false),
		m_ProjectionMatrixUpdateQueued(false)
	{
		m_ProjectionMatrix = ME::Core::Math::Matrix4x4::FromPerspectiveView(static_cast<float32>(Core::Math::ToRadians(m_FOV)), m_AspectRatio, m_Near, m_Far);
	}

	void PerspectiveCamera::Update()
	{
		if (m_ProjectionMatrixUpdateQueued)
		{
			m_ProjectionMatrix = ME::Core::Math::Matrix4x4::FromPerspectiveView(static_cast<float32>(Core::Math::ToRadians(m_FOV)), m_AspectRatio, m_Near, m_Far);
			m_ProjectionMatrixUpdateQueued = false;
		}
		if (!m_UpdateQueued) return;

		m_Transform = ME::Core::Math::Transform(m_NewPosition) * ME::Core::Math::Transform::FromQuaternion(m_NewRotation);
		m_UpdateQueued = false;
	}
}