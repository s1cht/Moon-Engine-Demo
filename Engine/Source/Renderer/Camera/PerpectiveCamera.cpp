#include "PerpectiveCamera.h"
#include <Core/Math/MathMacros.hpp>

namespace ME::Render::Camera
{
	ME::Render::Camera::PerpectiveCamera::PerpectiveCamera() :
		m_ProjectionMatrix(ME::Core::Math::Matrix4x4()),
		m_ViewMatrix(ME::Core::Math::Matrix4x4()),
		m_ProjViewMatrix(ME::Core::Math::Matrix4x4()),
		m_FOV(90), m_AspectRatio(1920.L / 1080.L), m_Near(0.1f), m_Far(1000.f),
		m_Position(ME::Core::Math::Vector3D(0,0,-20)),
		m_LookVector(ME::Core::Math::Vector3D::ForwardVector),
		m_Rotation(ME::Core::Math::Quaternion(1, 0, 0, 0)) 
	{
		Update();
	}

	ME::Render::Camera::PerpectiveCamera::~PerpectiveCamera() {}
	
	void PerpectiveCamera::Update()
	{
		if (m_ProjectionMatrixUpdateQueued)
		{
			m_ProjectionMatrix = ME::Core::Math::Matrix4x4::Perspective((float32)RAD(m_FOV), m_AspectRatio, m_Near, m_Far);
			m_ProjectionMatrixUpdateQueued = false;
		}
		if (!m_UpdateQueued) return;

		ME::Core::Math::Matrix4x4 rotMatrix = ME::Core::Math::Matrix4x4::FromQuaternion(m_Rotation);
		m_LookVector = rotMatrix.TransformNormal(ME::Core::Math::Vector3D::ForwardVector);
		m_UpVector = rotMatrix.TransformNormal(ME::Core::Math::Vector3D::UpVector);
		m_RightVector = rotMatrix.TransformNormal(ME::Core::Math::Vector3D::RightVector);

		m_ViewMatrix = rotMatrix * Core::Math::Matrix4x4::Translation(-m_Position);

		m_UpdateQueued = false;
	}

	void ME::Render::Camera::PerpectiveCamera::SetFOV(uint32 fov)
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

	void ME::Render::Camera::PerpectiveCamera::SetPosition(ME::Core::Math::Vector3D position)
	{
		m_Position = position;
		m_UpdateQueued = true;
	}

	void ME::Render::Camera::PerpectiveCamera::SetRotation(ME::Core::Math::Vector3D rotation)
	{
		SetRotation(static_cast<float32>(rotation.Y), static_cast<float32>(rotation.X), static_cast<float32>(rotation.Z));
	}

	void ME::Render::Camera::PerpectiveCamera::SetRotation(ME::Core::Math::Quaternion rotation)
	{
		m_Rotation = rotation;
		m_UpdateQueued = true;
	}

	void PerpectiveCamera::SetRotation(float32 yaw, float32 pitch, float32 roll)
	{
		ME::Core::Math::Quaternion qYaw = ME::Core::Math::Quaternion::FromAxisAngle(Core::Math::Vector3D::UpVector, yaw);
		ME::Core::Math::Quaternion qPitch = ME::Core::Math::Quaternion::FromAxisAngle(Core::Math::Vector3D::RightVector, pitch);

		m_Rotation =  qPitch * qYaw;
		m_Rotation.Normalize();
		m_UpdateQueued = true;
	}
}