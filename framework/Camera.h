#ifndef CWF_CAMERA_H
#define CWF_CAMERA_H

#include "Orientation.h"
#include <DirectXMath.h>

namespace math = DirectX;

class Camera {
	// TODO: CODE STYLE REVIEW!!!!
private:
	math::XMFLOAT3 m_pos;
	math::XMFLOAT3 m_up;
	math::XMFLOAT4X4 m_matrix;
	// TODO: handle alignment when on stack but not dynamically allocated itself (i.e. as member object)
	Orientation m_o;
public:
	Camera() noexcept;
	Camera(const math::XMFLOAT3& pos) noexcept;
	Camera(float xTheta, float yTheta, float zTheta) noexcept;
	Camera(const math::XMFLOAT3& pos, float xTheta, float yTheta, float zTheta) noexcept;

	void reset() noexcept;
	void setPosition(const math::XMFLOAT3& newPos) noexcept;
	void setOrientation(float xTheta, float yTheta, float zTheta) noexcept;
	void setUp(const math::XMFLOAT3& newUp) noexcept;
	void updatePosition(const math::XMFLOAT3& deltaPos) noexcept;
	void updateOrientation(float dxTheta, float dyTheta, float dzTheta) noexcept;
	void updateUp(const math::XMFLOAT3& deltaUp) noexcept;

	const math::XMMATRIX& get() const noexcept;
};

#endif