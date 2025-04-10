#pragma once

#include <Core.h>

import Pawn.Core.Math;

namespace Pawn::Render::Camera
{
	class PAWN_API Camera
	{
	public:
		virtual ~Camera() {};

		virtual void Update() = 0;

		virtual void SetFOV(uint32 fov) = 0;
		virtual void SetAspectRatio(float32 aspectRatio) = 0;
		virtual void SetNear(float32 _near) = 0;
		virtual void SetFar(float32 _far) = 0;
		virtual void SetPosition(Pawn::Core::Math::Vector3D position) = 0;
		virtual void SetRotation(Pawn::Core::Math::Vector3D rotation) = 0;
		virtual void SetRotation(Pawn::Core::Math::Quaternion rotation) = 0;
		virtual void SetRotation(float32 yaw, float32 pitch, float32 roll) = 0;

		virtual const Pawn::Core::Math::Vector3D& GetPosition() const = 0;
		virtual const Pawn::Core::Math::Quaternion& GetRotation() const = 0;
		virtual const Pawn::Core::Math::Matrix4x4& GetViewMatrix() const = 0;
		virtual const Pawn::Core::Math::Matrix4x4& GetProjectionMatrix() const = 0;
		virtual const Pawn::Core::Math::Matrix4x4& GetProjViewMatrix() const = 0;

	};

}