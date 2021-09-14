#ifndef CWF_CONSTANTBUFFERS_H
#define CWF_CONSTANTBUFFERS_H

#include <DirectXMath.h>

namespace math = DirectX;

namespace ConstantBuffers {
	struct ConstBuffer {
		math::XMFLOAT4X4 transform;

		ConstBuffer(math::CXMMATRIX t) : transform{} {
			math::XMStoreFloat4x4(&transform, t);
		}

		ConstBuffer() : transform{} {}

		ConstBuffer& XM_CALLCONV operator=(math::FXMMATRIX t) {
			math::XMStoreFloat4x4(&transform, t);
			return *this;
		}
	};

	struct TConstBuffer {
		math::XMFLOAT4X4 transform;

		TConstBuffer(math::CXMMATRIX t) : transform{} {
			math::XMStoreFloat4x4(&transform, math::XMMatrixTranspose(t));
		}

		TConstBuffer() : transform{} {}
		TConstBuffer& XM_CALLCONV operator=(math::FXMMATRIX t) {
			math::XMStoreFloat4x4(&transform, math::XMMatrixTranspose(t));
			return *this;
		}
	};
}

#endif