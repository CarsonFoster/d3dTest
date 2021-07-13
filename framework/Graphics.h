#ifndef CWF_GRAPHICS_H
#define CWF_GRAPHICS_H

#include "CwfException.h"

#ifndef NDEBUG
#include "DXDebugInfoManager.h"
#endif

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <utility>
#include <vector>
#include <Windows.h>
#include <wrl.h>

#ifndef NDEBUG
#define THROW_IF_FAILED(gfx, func) { \
									(gfx).info.set(); \
									HRESULT hr{ (func) }; \
									throwIfFailed((gfx), hr, __FILE__, __LINE__); \
								   }
#define THROW_ON_INFO(gfx, func) { \
								  (gfx).info.set(); \
								  (func); \
								  std::vector<std::wstring> dbugInfo{ (gfx).info.getMessages() }; \
								  if (!dbugInfo.empty()) throw CwfException{ dbugInfo, __FILE__, __LINE__ }; \
								 }
#else
#define THROW_IF_FAILED(gfx, func) throwIfFailed((gfx), (func), __FILE__, __LINE__)
#define THROW_ON_INFO(gfx, func) func
#endif

#define THROW_IF_FAILED_NOGFX(func) throwIfFailedNoGfx((func), __FILE__, __LINE__)

namespace math = DirectX;

class Graphics {
private:
	int clientWidth;
	int clientHeight;
	math::XMMATRIX projection; // TODO: alignment
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
public:
#ifndef NDEBUG
	mutable DXDebugInfoManager info;
#endif
public:
	template <class Vertex, typename Index>
	struct IndexedVertexList {
		std::vector<Vertex> vertices;
		std::vector<Index> indices;
	};

	struct Shader {
		const void* pByteCode;
		size_t length;
	};

	struct TConstBuffer {
		math::XMMATRIX transform; // must be 16-byte aligned

		TConstBuffer(math::CXMMATRIX t) : transform{ math::XMMatrixTranspose(t) } {}
		TConstBuffer() : transform{} {}
		TConstBuffer& XM_CALLCONV operator=(math::FXMMATRIX t) {
			transform = math::XMMatrixTranspose(t);
			return *this;
		}

		void* operator new(size_t size) {
			void* p{ _aligned_malloc(size, 16) };
			return p;
		}

		void* operator new[](size_t size) {
			void* p{ _aligned_malloc(size, 16) };
			return p;
		}

		void operator delete(void* p) {
			_aligned_free(p);
		}

		void operator delete[](void* p) {
			_aligned_free(p);
		}
	};

	template <class U, size_t A = 16>
	class AlignedObject {
	public:
		struct AlignedDeleter {
			void operator()(void* p) {
				_aligned_free(p); // frees memory allocated with _aligned_malloc
			}
		};
	private:
		std::unique_ptr<U, AlignedDeleter> pObj;
	public:
		template <class... Args>
		AlignedObject(Args&&... args) : pObj{} {
			void* raw{ _aligned_malloc(sizeof(U), A) }; // allocate A-byte aligned memory (by default, 16)
			pObj = std::unique_ptr<U, AlignedDeleter>(new(raw) U{ std::forward<Args>(args)... }); // new(raw) U{} constructs a new U at the raw memory location
		}

		U& get() {
			return *pObj;
		}
	};

public:
	Graphics(HWND hWnd, int cWidth, int cHeight);
	~Graphics() = default;
	// no copy init/assign
	Graphics(const Graphics& o) = delete;
	Graphics& operator=(const Graphics& o) = delete;

	void endFrame();
	void clearBuffer(float r, float g, float b);
	void drawTestCube(bool, bool, bool, bool);

	HRESULT getDeviceRemovedReason() const noexcept;
	Microsoft::WRL::ComPtr<ID3D11Device> getDevice() const noexcept;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> getImmediateContext() const noexcept;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> getRenderTargetView() const noexcept;
	
	void setProjection(float fov_deg, float nearZ, float farZ) noexcept;
	math::XMMATRIX getProjection() const noexcept; // TODO: determine if this can be XMMATRIX or not
};

inline void throwIfFailed(const Graphics& gfx, HRESULT hr, const char* file, int line) {
	if (FAILED(hr)) {
		throw CwfException{ gfx, CwfException::DirectXErrorString{ hr }, file, line };
	}
}

inline void throwIfFailedNoGfx(HRESULT hr, const char* file, int line) {
	if (FAILED(hr)) {
		throw CwfException{ CwfException::DirectXErrorString{ hr }, file, line };
	}
}

#endif
