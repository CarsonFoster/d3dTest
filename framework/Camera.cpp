#include "Camera.h"
#include "Orientation.h"
#include <DirectXMath.h>

namespace math = DirectX;

Camera::Camera() noexcept
	: m_pos{ 0.0f, 0.0f, 0.0f },
	  m_up{ 0.0f, 1.0f, 0.0f },
	  m_matrix{},
	  m_o{},
	  m_changed{ true } {}

Camera::Camera(const math::XMFLOAT3& pos) noexcept
	: m_pos{ pos },
	  m_up{ 0.0f, 1.0f, 0.0f },
	  m_matrix{},
	  m_o{},
	  m_changed{ true } {}

Camera::Camera(float xTheta, float yTheta, float zTheta) noexcept
	: m_pos{ 0.0f, 0.0f, 0.0f },
	  m_up{ 0.0f, 1.0f, 0.0f },
	  m_matrix{},
	  m_o{ xTheta, yTheta, zTheta },
	  m_changed{ true } {}

Camera::Camera(const math::XMFLOAT3& pos, float xTheta, float yTheta, float zTheta) noexcept
	: m_pos{ pos },
	  m_up{ 0.0f, 1.0f, 0.0f },
	  m_matrix{},
	  m_o{ xTheta, yTheta, zTheta },
	  m_changed{ true } {}

void Camera::reset() noexcept {
	m_pos = { 0.0f, 0.0f, 0.0f };
	m_up = { 0.0f, 1.0f, 0.0f };
	m_o.reset();
	m_changed = true;
}

void Camera::setPosition(float x, float y, float z) noexcept {
	m_pos = {x, y, z};
	m_changed = true;
}

void Camera::setOrientation(float xTheta, float yTheta, float zTheta) noexcept {
	m_o.set(xTheta, yTheta, zTheta);
	m_changed = true;
}

void Camera::setUp(float x, float y, float z) noexcept {
	m_up = {x, y, z};
	m_changed = true;
}

void Camera::updatePosition(float x, float y, float z) noexcept {
	m_pos.x += x;
	m_pos.y += y;
	m_pos.z += z;
	m_changed = true;
}

void Camera::updateOrientation(float dxTheta, float dyTheta, float dzTheta) noexcept {
	m_o.update(dxTheta, dyTheta, dzTheta);
	m_changed = true;
}

void Camera::updateUp(float x, float y, float z) noexcept {
	m_up.x += x;
	m_up.y += y;
	m_up.z += z;
	m_changed = true;
}

math::XMMATRIX Camera::get() const noexcept {
	if (m_changed) {
		math::XMStoreFloat4x4(&m_matrix,
			math::XMMatrixLookToLH(
				math::XMLoadFloat3(&m_pos),
				math::XMVector3Transform(
					math::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
					m_o.get()
				),
				math::XMLoadFloat3(&m_up)
			)
		);
		m_changed = false;
	}
	return math::XMLoadFloat4x4(&m_matrix);
}