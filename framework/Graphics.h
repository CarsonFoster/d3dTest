#ifndef CWF_GRAPHICS_H
#define CWF_GRAPHICS_H

#ifndef NDEBUG
#include "DXDebugInfoManager.h"
#endif

#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>

#ifndef NDEBUG
#define THROW_IF_FAILED(gfx, func) { \
									gfx.info.set(); \
									HRESULT hr{ (func) }; \
									throwIfFailed(gfx, hr, __FILE__, __LINE__); \
								   }
#else
#define THROW_IF_FAILED(gfx, hr) throwIfFailed(gfx, hr, __FILE__, __LINE__)
#endif

#define THROW_IF_FAILED_NOGFX(hr) throwIfFailedNoGfx(hr, __FILE__, __LINE__)

class Graphics {
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;

	static inline void throwIfFailed(const Graphics& gfx, HRESULT hr, const char* file, int line);
	static inline void throwIfFailedNoGfx(HRESULT hr, const char* file, int line);
public:
#ifndef NDEBUG
	DXDebugInfoManager info;
#endif
public:
	Graphics(HWND hWnd);
	~Graphics() = default;

	// no copy init/assign
	Graphics(const Graphics& o) = delete;
	Graphics& operator=(const Graphics& o) = delete;

	void endFrame();
	void clearBuffer(float r, float g, float b);

	HRESULT getDeviceRemovedReason() const noexcept;
};

#endif
