#pragma once
#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Math/Transform.hpp>

namespace ME::Render
{
	class MEAPI Camera
	{
	public:
		virtual ~Camera() = default;

		virtual void Update() = 0;
		virtual bool BufferUpdateRequired() const = 0;
		virtual void BufferUpdated() = 0;

		virtual void SetFOV(uint32 fov) = 0;
		virtual void SetAspectRatio(float32 aspectRatio) = 0;
		virtual void SetNear(float32 _near) = 0;
		virtual void SetFar(float32 _far) = 0;

		virtual void SetTransform(const ME::Core::Math::Transform& transform) = 0;

		virtual void SetPosition(const ME::Core::Math::Vector3D& position) = 0;

		virtual void SetRotation(const ME::Core::Math::Vector3D& rotation) = 0;
		virtual void SetRotation(const ME::Core::Math::Quaternion& rotation) = 0;
		virtual void SetRotation(float32 yaw, float32 pitch, float32 roll) = 0;

		virtual const ME::Core::Math::Vector3D& GetPosition() const = 0;
		virtual const ME::Core::Math::Vector3D& GetRotation() const = 0;
		virtual const ME::Core::Math::Transform& GetTransform() const = 0;

		virtual const ME::Core::Math::Matrix4x4& GetViewMatrix() const = 0;
		virtual const ME::Core::Math::Matrix4x4& GetProjectionMatrix() const = 0;
	};
}