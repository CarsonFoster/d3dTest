#ifndef CWF_CAMERA_H
#define CWF_CAMERA_H

#include "Orientation.h"
#include <DirectXMath.h>

namespace math = DirectX;

class Camera {
private:
	math::XMFLOAT3 m_pos;
	math::XMFLOAT3 m_up;
	mutable math::XMFLOAT4X4 m_matrix;
	Orientation m_o;
	mutable bool m_changed;
public:
	Camera() noexcept;
	Camera(const math::XMFLOAT3& pos) noexcept;
	Camera(float xTheta, float yTheta, float zTheta) noexcept;
	Camera(const math::XMFLOAT3& pos, float xTheta, float yTheta, float zTheta) noexcept;

	void reset() noexcept;
	void setPosition(float x, float y, float z) noexcept;
	void setOrientation(float xTheta, float yTheta, float zTheta) noexcept;
	void setUp(float x, float y, float z) noexcept;
	void updatePosition(float x, float y, float z) noexcept;
	void updateOrientation(float dxTheta, float dyTheta, float dzTheta) noexcept;
	void updateUp(float x, float y, float z) noexcept;

	math::XMMATRIX get() const noexcept;
};

#endif