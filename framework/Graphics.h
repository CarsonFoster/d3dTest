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
#include <variant>
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
	math::XMMATRIX projection; // should be ok, heap allocation forbidden for this class
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pZBuffer;
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

	struct AlignedDeleter {
		void operator()(void* p) noexcept {
			_aligned_free(p); // frees memory allocated with _aligned_malloc
		}
	};

	template <class U, size_t A = 16>
	class AlignedObject {
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

	struct Texture2D {
		struct RawData {
			UINT width;
			UINT height;
			UINT mipLevels;
			UINT arraySize;
			DXGI_FORMAT format;
			UINT sampleCount;
			UINT sampleQuality;
			UINT cpuAccessFlags;
			UINT miscFlags;
			void* pData;
			UINT pitch;
			UINT mostDetailedMip;

			RawData(UINT w, UINT h, DXGI_FORMAT f, void* pTextureData, UINT dataPitch)
				: width{ w }, height{ h }, mipLevels{ 1u }, arraySize{ 1u }, format{ f },
				sampleCount{ 1u }, sampleQuality{ 0u }, cpuAccessFlags{ 0u }, miscFlags{ 0u },
				pData{ pTextureData }, pitch{ dataPitch }, mostDetailedMip{ 0u } {}
		};
		struct File {
			const wchar_t* filename;

			File(const wchar_t* szFileName) : filename{ szFileName } {}
		};
		struct Sampler {
			D3D11_FILTER filter{ D3D11_FILTER_MIN_MAG_MIP_LINEAR };
			D3D11_TEXTURE_ADDRESS_MODE u{ D3D11_TEXTURE_ADDRESS_CLAMP };
			D3D11_TEXTURE_ADDRESS_MODE v{ D3D11_TEXTURE_ADDRESS_CLAMP };
			D3D11_TEXTURE_ADDRESS_MODE w{ D3D11_TEXTURE_ADDRESS_CLAMP };
			float mipLODBias{ 0.0f };
			UINT maxAnisotropy{ 0u };
			D3D11_COMPARISON_FUNC comparisonFunc{ D3D11_COMPARISON_NEVER };
			struct {
				float r{ 0.0f };
				float g{ 0.0f };
				float b{ 0.0f };
				float a{ 1.0f };
			} borderColor;
			float minLOD{ 0u };
			float maxLOD{ 0u };
		};
	public:
		std::variant<RawData, File> content;
		Sampler sampler;
		Texture2D(UINT w, UINT h, DXGI_FORMAT f, void* pTextureData, UINT dataPitch)
			: content{ RawData{ w, h, f, pTextureData, dataPitch } } {}
		Texture2D(const wchar_t* szFileName) : content{ File{ szFileName } } {}
	};

	struct Float3 {
		struct {
			float x;
			float y;
			float z;
		} pos;
		Float3(float X, float Y, float Z) {
			pos.x = X;
			pos.y = Y;
			pos.z = Z;
		}
	};

	struct Float4 {
		struct {
			float x;
			float y;
			float z;
			float w;
		} pos;
		Float4(float X, float Y, float Z, float W) {
			pos.x = X;
			pos.y = Y;
			pos.z = Z;
			pos.w = W;
		}
	};

public:
	Graphics(HWND hWnd, int cWidth, int cHeight);
	~Graphics() = default;
	// no copy init/assign
	Graphics(const Graphics& o) = delete;
	Graphics& operator=(const Graphics& o) = delete;
	// no heap allocation
	void* operator new(size_t size);
	void* operator new[](size_t size);
	void operator delete(void* p);
	void operator delete[](void* p);

	void endFrame();
	void clearBuffer(float r, float g, float b);
	void drawTestCube(bool, bool, bool, bool);

	HRESULT getDeviceRemovedReason() const noexcept;
	Microsoft::WRL::ComPtr<ID3D11Device> getDevice() const noexcept;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> getImmediateContext() const noexcept;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> getRenderTargetView() const noexcept;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> getZBuffer() const noexcept;
	
	void setProjection(float fov_deg, float nearZ, float farZ) noexcept;
	math::XMMATRIX getProjection() const noexcept;
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
