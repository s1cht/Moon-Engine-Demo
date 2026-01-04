#pragma once
#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Math/Transform.hpp>

namespace ME::Render
{
	struct PerspectiveProjectionInfo
	{
		float32 Near;
		float32 Far;
		float32 AspectRatio;
		float32 FieldOfView;
	};

	class MEAPI Camera
	{
	public:
		virtual ~Camera() = default;

		virtual void Update() = 0;
		virtual bool BufferUpdateRequired() const = 0;
		virtual void BufferUpdated() = 0;

		virtual void SetPosition(const ME::Core::Math::Vector3D& position) = 0;
		virtual void SetRotation(const ME::Core::Math::Vector3D& rotation) = 0;
		virtual void SetRotation(const ME::Core::Math::Quaternion& rotation) = 0;
		virtual void SetRotation(float32 pitch, float32 yaw, float32 roll) = 0;

		virtual const ME::Core::Math::Vector3D& GetPosition() const = 0;
		virtual const ME::Core::Math::Quaternion GetQuaternion() const = 0;
		virtual const ME::Core::Math::Vector3D GetRotation() const = 0;
		
		virtual const ME::Core::Math::Matrix4x4& GetView() const = 0;
		virtual const ME::Core::Math::Matrix4x4& GetProjection() const = 0;
	};
}