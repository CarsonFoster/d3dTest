#ifndef CWF_ORIENTATION_H
#define CWF_ORIENTATION_H

#include <DirectXMath.h>

namespace math = DirectX;

class Orientation {
private:
	math::XMMATRIX orientation;
public:
	Orientation() : orientation{ math::XMMatrixIdentity() } {}

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

	void update(float xTheta, float yTheta, float zTheta) {
		orientation = math::XMMatrixMultiply(
			orientation,
			math::XMMatrixRotationRollPitchYaw(xTheta, yTheta, zTheta)
		);
	}

	math::XMMATRIX& get() {
		return orientation;
	}
};


#endif