#pragma once

#include <Core.hpp>
#include "Camera.h"

namespace Pawn::Render::Camera
{
	class PAWN_API PerpectiveCamera : public Camera
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
		void SetPosition(Pawn::Core::Math::Vector3D position) override;
		void SetRotation(Pawn::Core::Math::Vector3D rotation) override;
		void SetRotation(Pawn::Core::Math::Quaternion rotation) override;
		void SetRotation(float32 yaw, float32 pitch, float32 roll) override;

	public:
		const Pawn::Core::Math::Vector3D& GetPosition() const override { return m_Position; };
		const Pawn::Core::Math::Quaternion& GetRotation() const override { return m_Rotation; };
		const Pawn::Core::Math::Matrix4x4& GetViewMatrix() const override { return m_ViewMatrix; };
		const Pawn::Core::Math::Matrix4x4& GetProjectionMatrix() const override { return m_ProjectionMatrix; };
		const Pawn::Core::Math::Matrix4x4& GetProjViewMatrix() const override { return m_ProjViewMatrix; };

	private:
		uint32 m_FOV;
		float32 m_AspectRatio;
		float32 m_Near;
		float32 m_Far;

		Pawn::Core::Math::Vector3D m_Position;
		Pawn::Core::Math::Quaternion m_Rotation;

	private:
		Pawn::Core::Math::Matrix4x4 m_ProjectionMatrix;
		Pawn::Core::Math::Matrix4x4 m_ViewMatrix;
		Pawn::Core::Math::Matrix4x4 m_ProjViewMatrix;

		bool m_UpdateQueued = false;
		bool m_ProjectionMatrixUpdateQueued = true;

	};

}
