#ifndef CWF_CONSTANTBUFFERS_H
#define CWF_CONSTANTBUFFERS_H

#include "Graphics.h"
#include <DirectXMath.h>

namespace math = DirectX;

namespace ConstantBuffers {
	struct ConstBuffer {
		math::XMFLOAT4X4 m_transform;

		ConstBuffer(math::CXMMATRIX t) : m_transform{} {
			math::XMStoreFloat4x4(&m_transform, t);
		}

		ConstBuffer() : m_transform{} {}

		ConstBuffer& XM_CALLCONV operator=(math::FXMMATRIX t) {
			math::XMStoreFloat4x4(&m_transform, t);
			return *this;
		}
	};

	struct TConstBuffer {
		math::XMFLOAT4X4 m_transform;

		TConstBuffer(math::CXMMATRIX t) : m_transform{} {
			math::XMStoreFloat4x4(&m_transform, math::XMMatrixTranspose(t));
		}

		TConstBuffer() : m_transform{} {}
		TConstBuffer& XM_CALLCONV operator=(math::FXMMATRIX t) {
			math::XMStoreFloat4x4(&m_transform, math::XMMatrixTranspose(t));
			return *this;
		}
	};

	struct VPTConstBuffer {
		math::XMFLOAT4X4 m_transform;
		const Graphics& m_gfx;

		VPTConstBuffer(const Graphics& gfx, math::CXMMATRIX t) : m_gfx{ gfx }, m_transform{} {
			math::XMStoreFloat4x4(&m_transform,
				math::XMMatrixTranspose(
					math::XMMatrixMultiply(
						math::XMMatrixMultiply(
							m_gfx.camera().get(),
							m_gfx.getProjection()
						),
						t
					)
				)
			);
		}

		VPTConstBuffer(const Graphics& gfx) : m_gfx{ gfx }, m_transform{} {}

		VPTConstBuffer& XM_CALLCONV operator=(math::FXMMATRIX t) {
			math::XMStoreFloat4x4(&m_transform,
				math::XMMatrixTranspose(
					math::XMMatrixMultiply(
						math::XMMatrixMultiply(
							m_gfx.camera().get(),
							m_gfx.getProjection()
						),
						t
					)
				)
			);
			return *this;
		}

		constexpr size_t getBufferSize() const {
			return sizeof(m_transform);
		}
	};
}

#endif