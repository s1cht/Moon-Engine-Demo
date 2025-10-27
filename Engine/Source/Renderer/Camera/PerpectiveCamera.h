#pragma once

#include <Core.hpp>
#include "Camera.h"

namespace ME::Render::Camera
{
	class MOON_API PerpectiveCamera : public Camera
	{
	public:
		PerpectiveCamera();
		~PerpectiveCamera();

	public:
		void Update() override;

	public:
		void SetFOV(uint32 fov) override;
		void SetAspectRatio(float32 aspectRatio) override;
		void SetNear(float32 _near) override;
		void SetFar(float32 _far) override;
		void SetPosition(ME::Core::Math::Vector3D position) override;
		void SetRotation(ME::Core::Math::Vector3D rotation) override;
		void SetRotation(ME::Core::Math::Quaternion rotation) override;
		void SetRotation(float32 yaw, float32 pitch, float32 roll) override;

	public:
		const ME::Core::Math::Vector3D& GetPosition() const override { return m_Position; }
		const ME::Core::Math::Vector3D& GetLookVector() const override { return m_LookVector; }
		const ME::Core::Math::Vector3D& GetUpVector() const override { return m_UpVector; }
		const ME::Core::Math::Vector3D& GetRightVector() const override { return m_RightVector; }

		const ME::Core::Math::Quaternion& GetRotation() const override { return m_Rotation; }
		const ME::Core::Math::Matrix4x4& GetViewMatrix() const override { return m_ViewMatrix; }

		const ME::Core::Math::Matrix4x4& GetProjectionMatrix() const override { return m_ProjectionMatrix; }
		const ME::Core::Math::Matrix4x4& GetProjViewMatrix() const override { return m_ProjViewMatrix; }

	private:
		uint32 m_FOV;
		float32 m_AspectRatio;
		float32 m_Near;
		float32 m_Far;

		ME::Core::Math::Vector3D m_Position;
		ME::Core::Math::Quaternion m_Rotation;

		ME::Core::Math::Vector3D m_LookVector;
		ME::Core::Math::Vector3D m_UpVector;
		ME::Core::Math::Vector3D m_RightVector;
	private:
		ME::Core::Math::Matrix4x4 m_ProjectionMatrix;
		ME::Core::Math::Matrix4x4 m_ViewMatrix;
		ME::Core::Math::Matrix4x4 m_ProjViewMatrix;

		bool m_UpdateQueued = false;
		bool m_ProjectionMatrixUpdateQueued = true;

	};

}
