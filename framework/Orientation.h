#ifndef CWF_ORIENTATION_H
#define CWF_ORIENTATION_H

#include <DirectXMath.h>

namespace math = DirectX;

class Orientation {
private:
	math::XMFLOAT4X4 m_matrix;
public:
	Orientation() noexcept : m_matrix{} {
		math::XMStoreFloat4x4(&m_matrix,
			math::XMMatrixIdentity()
		);
	}
	Orientation(float xTheta, float yTheta, float zTheta) noexcept : m_matrix{} {
		math::XMStoreFloat4x4(&m_matrix,
			math::XMMatrixRotationRollPitchYaw(xTheta, yTheta, zTheta)
		);
	}

	void update(float xTheta, float yTheta, float zTheta) noexcept {
		math::XMStoreFloat4x4(&m_matrix,
			math::XMMatrixMultiply(
				math::XMLoadFloat4x4(&m_matrix),
				math::XMMatrixRotationRollPitchYaw(xTheta, yTheta, zTheta)
			)
		);
	}

	void set(float xTheta, float yTheta, float zTheta) noexcept {
		math::XMStoreFloat4x4(&m_matrix,
			math::XMMatrixRotationRollPitchYaw(xTheta, yTheta, zTheta)
		);
	}

	void reset() noexcept {
		math::XMStoreFloat4x4(&m_matrix,
			math::XMMatrixIdentity()
		);
	}

	math::XMMATRIX get() const noexcept {
		return math::XMLoadFloat4x4(&m_matrix);
	}
};


#endif