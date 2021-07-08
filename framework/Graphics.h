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

class Graphics {
private:
	int clientWidth;
	int clientHeight;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;

	static inline void throwIfFailed(const Graphics& gfx, HRESULT hr, const char* file, int line);
	static inline void throwIfFailedNoGfx(HRESULT hr, const char* file, int line);
public:
#ifndef NDEBUG
	mutable DXDebugInfoManager info;
#endif
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
};

#endif
