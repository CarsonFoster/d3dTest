#ifndef CWF_ORIENTATION_H
#define CWF_ORIENTATION_H

#include <DirectXMath.h>

namespace math = DirectX;

class Orientation {
private:
	math::XMMATRIX orientation;
public:
	Orientation() noexcept : orientation{ math::XMMatrixIdentity() } {}
	Orientation(float xTheta, float yTheta, float zTheta) noexcept
		: orientation{ math::XMMatrixRotationRollPitchYaw(xTheta, yTheta, zTheta) } {}

	// aligned heap allocation only
	void* operator new(size_t size) {
		return _aligned_malloc(size, 16);
	}

	void* operator new[](size_t size) {
		return _aligned_malloc(size, 16);
	}

	void operator delete(void* p) {
		_aligned_free(p);
	}

	void operator delete[](void* p) {
		_aligned_free(p);
	}

	void update(float xTheta, float yTheta, float zTheta) noexcept {
		orientation = math::XMMatrixMultiply(
			orientation,
			math::XMMatrixRotationRollPitchYaw(xTheta, yTheta, zTheta)
		);
	}

	void set(float xTheta, float yTheta, float zTheta) noexcept {
		orientation = math::XMMatrixRotationRollPitchYaw(xTheta, yTheta, zTheta);
	}

	void reset() noexcept {
		orientation = math::XMMatrixIdentity();
	}

	const math::XMMATRIX& get() const noexcept {
		return orientation;
	}
};


#endif