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

void Camera::setPosition(const math::XMFLOAT3& newPos) noexcept {
	m_pos = newPos;
	m_changed = true;
}

void Camera::setOrientation(float xTheta, float yTheta, float zTheta) noexcept {
	m_o.set(xTheta, yTheta, zTheta);
	m_changed = true;
}

void Camera::setUp(const math::XMFLOAT3& newUp) noexcept {
	m_up = newUp;
	m_changed = true;
}

void Camera::updatePosition(const math::XMFLOAT3& deltaPos) noexcept {
	m_pos.x += deltaPos.x;
	m_pos.y += deltaPos.y;
	m_pos.z += deltaPos.z;
	m_changed = true;
}

void Camera::updateOrientation(float dxTheta, float dyTheta, float dzTheta) noexcept {
	m_o.update(dxTheta, dyTheta, dzTheta);
	m_changed = true;
}

void Camera::updateUp(const math::XMFLOAT3& deltaUp) noexcept {
	m_up.x += deltaUp.x;
	m_up.y += deltaUp.y;
	m_up.z += deltaUp.z;
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