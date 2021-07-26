#include "Camera.h"
#include "Orientation.h"
#include <DirectXMath.h>

namespace math = DirectX;

Camera::Camera() noexcept
	: m_pos{ 0.0f, 0.0f, 0.0f },
	  m_up{ 0.0f, 1.0f, 0.0f },
	  m_matrix{},
	  m_o{} {
	
	math::XMStoreFloat4x4(&m_matrix,
		math::XMMatrixIdentity()
	);
}

Camera::Camera(const math::XMFLOAT3& pos) noexcept
	: m_pos{ pos },
	  m_up{ 0.0f, 1.0f, 0.0f },
	  m_matrix{},
	  m_o{} {
	
	math::XMStoreFloat4x4(&m_matrix,
		math::XMMatrixLookToLH(
			math::XMLoadFloat3(&m_pos),
			math::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
			math::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		)
	);
}

Camera::Camera(float xTheta, float yTheta, float zTheta) noexcept
	: m_pos{ 0.0f, 0.0f, 0.0f },
	  m_up{ 0.0f, 1.0f, 0.0f },
	  m_matrix{},
	  m_o{ xTheta, yTheta, zTheta } {

	math::XMStoreFloat4x4(&m_matrix,
		math::XMMatrixLookToLH(
			math::XMLoadFloat3(&m_pos),
			math::XMVector3Transform(
				math::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
				m_o.get()
			),
			math::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		)
	);
}

Camera::Camera(const math::XMFLOAT3& pos, float xTheta, float yTheta, float zTheta) noexcept
	: m_pos{ pos },
	  m_up{ 0.0f, 1.0f, 0.0f },
	  m_matrix{},
	  m_o{ xTheta, yTheta, zTheta } {

	math::XMStoreFloat4x4(&m_matrix,
		math::XMMatrixLookToLH(
			math::XMLoadFloat3(&m_pos),
			math::XMVector3Transform(
				math::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
				m_o.get()
			),
			math::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		)
	);
}

void Camera::reset() noexcept {
	m_pos = { 0.0f, 0.0f, 0.0f };
	m_up = { 0.0f, 1.0f, 0.0f };
	m_o.reset();
	math::XMStoreFloat4x4(&m_matrix,
		math::XMMatrixIdentity()
	);
}