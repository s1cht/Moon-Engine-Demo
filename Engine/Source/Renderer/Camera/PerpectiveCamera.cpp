#include "PerpectiveCamera.h"
#include <Core/Math/MathMacros.hpp>

namespace Pawn::Render::Camera
{
	Pawn::Render::Camera::PerpectiveCamera::PerpectiveCamera() :
		m_ProjectionMatrix(Pawn::Core::Math::Matrix4x4()),
		m_ViewMatrix(Pawn::Core::Math::Matrix4x4()),
		m_ProjViewMatrix(Pawn::Core::Math::Matrix4x4()),
		m_FOV(90), m_AspectRatio(1920.L / 1080.L), m_Near(0.1f), m_Far(1000.f),
		m_Position(Pawn::Core::Math::Vector3D(0)),
		m_Rotation(Pawn::Core::Math::Quaternion(1, 0, 0, 0)) 
	{
		Update();
	}

	Pawn::Render::Camera::PerpectiveCamera::~PerpectiveCamera() {}
	
	void PerpectiveCamera::Update()
	{
		if (m_ProjectionMatrixUpdateQueued)
		{
			m_ProjectionMatrix = Pawn::Core::Math::Matrix4x4::Perspective((float32)RAD(m_FOV), m_AspectRatio, m_Near, m_Far);
			m_ProjectionMatrixUpdateQueued = false;
		}
		if (!m_UpdateQueued) return;

		m_ViewMatrix = Pawn::Core::Math::Matrix4x4::Translation(static_cast<Pawn::Core::Math::Vector3D32>(m_Position)) * Pawn::Core::Math::Matrix4x4::FromQuaternion(m_Rotation);

		m_ProjViewMatrix = m_ProjectionMatrix * m_ViewMatrix;

		m_UpdateQueued = false;
	}

	void Pawn::Render::Camera::PerpectiveCamera::SetFOV(uint32 fov)
	{
		m_FOV = fov;
		m_ProjectionMatrixUpdateQueued = true;
	}

	void PerpectiveCamera::SetAspectRatio(float32 aspectRatio)
	{
		m_AspectRatio = aspectRatio;
		m_ProjectionMatrixUpdateQueued = true;
	}

	void PerpectiveCamera::SetNear(float32 _near)
	{
		m_Near = _near;
		m_ProjectionMatrixUpdateQueued = true;
	}

	void PerpectiveCamera::SetFar(float32 _far)
	{
		m_Far = _far;
		m_ProjectionMatrixUpdateQueued = true;
	}

	void Pawn::Render::Camera::PerpectiveCamera::SetPosition(Pawn::Core::Math::Vector3D position)
	{
		m_Position = position;
		m_UpdateQueued = true;
	}

	void Pawn::Render::Camera::PerpectiveCamera::SetRotation(Pawn::Core::Math::Vector3D rotation)
	{
		m_Rotation = Pawn::Core::Math::Quaternion::FromRadians(rotation);
		m_UpdateQueued = true;
	}

	void Pawn::Render::Camera::PerpectiveCamera::SetRotation(Pawn::Core::Math::Quaternion rotation)
	{
		m_Rotation = rotation;
		m_UpdateQueued = true;
	}

	void PerpectiveCamera::SetRotation(float32 yaw, float32 pitch, float32 roll)
	{
		m_Rotation = Pawn::Core::Math::Quaternion::FromEulerAngles(yaw, pitch, roll);
		m_UpdateQueued = true;
	}
}