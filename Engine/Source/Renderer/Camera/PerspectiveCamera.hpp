#pragma once

#include <Core.hpp>
#include "Camera.hpp"

namespace ME::Render
{
	class MEAPI PerspectiveCamera final : public Camera
	{
	public:
		PerspectiveCamera();
		PerspectiveCamera(const PerspectiveCamera&) = default;
		PerspectiveCamera(PerspectiveCamera&&) = default;
		~PerspectiveCamera() override = default;

	public:
		void Update() override;
		bool BufferUpdateRequired() const override
		{
			return m_BufferUpdateRequired;
		}

		void BufferUpdated() override
		{
			m_BufferUpdateRequired = false;
		}

	public:
		void SetFOV(uint32 fov) override
		{
			m_FOV = fov;
			m_ProjectionMatrixUpdateQueued = true;
		}

		void SetAspectRatio(float32 aspectRatio) override
		{
			m_AspectRatio = aspectRatio;
			m_ProjectionMatrixUpdateQueued = true;
		}

		void SetNear(float32 nearArg) override
		{
			m_Near = nearArg;
			m_ProjectionMatrixUpdateQueued = true;
		}

		void SetFar(float32 farArg) override
		{
			m_Far = farArg;
			m_ProjectionMatrixUpdateQueued = true;
		}

	    inline void SetTransform(const ME::Core::Math::Transform& transform) override
		{
		    m_Transform = transform;
			m_BufferUpdateRequired = true;
		}

		inline void SetPosition(const ME::Core::Math::Vector3D& position) override
		{
		    m_NewPosition = position;
		    m_UpdateQueued = true;
		}

	    inline void SetRotation(const ME::Core::Math::Vector3D& rotation) override
	    {
		    m_NewRotation = ME::Core::Math::Quaternion::FromEulerAnglesXYZ(rotation);
		    m_UpdateQueued = true;
		}

	    inline void SetRotation(const ME::Core::Math::Quaternion& rotation) override
		{
		    m_NewRotation = rotation;
		    m_UpdateQueued = true;
		}

	    inline void SetRotation(float32 yaw, float32 pitch, float32 roll) override
		{
		    m_NewRotation = ME::Core::Math::Quaternion::FromEulerAnglesXYZ(yaw, pitch, roll);
		    m_UpdateQueued = true;
		}

		inline const ME::Core::Math::Transform& GetTransform() const override { return m_Transform; }
		inline const ME::Core::Math::Vector3D& GetRotation() const override { return m_Transform.Rotation(); }
		inline const ME::Core::Math::Vector3D& GetPosition() const override { return m_Transform.Position(); }
		inline const ME::Core::Math::Matrix4x4& GetViewMatrix() const override { return m_Transform.Matrix(); }
		inline const ME::Core::Math::Matrix4x4& GetProjectionMatrix() const override { return m_ProjectionMatrix; }

    private:
		uint32 m_FOV;
		float32 m_AspectRatio;
		float32 m_Near;
		float32 m_Far;

		ME::Core::Math::Transform m_Transform;

		// For update
		ME::Core::Math::Vector3D m_NewPosition;
		ME::Core::Math::Quaternion m_NewRotation;

	private:
		ME::Core::Math::Matrix4x4 m_ProjectionMatrix;

		bool m_UpdateQueued;
		bool m_ProjectionMatrixUpdateQueued;
		bool m_BufferUpdateRequired;
		bool m_TransformUpdated;
	};
}
