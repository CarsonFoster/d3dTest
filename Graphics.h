#ifndef CWF_GRAPHICS_H
#define CWF_GRAPHICS_H

#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>

class Graphics {
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
public:
	Graphics(HWND hWnd);
	~Graphics() = default;

	// no copy init/assign
	Graphics(const Graphics& o) = delete;
	Graphics& operator=(const Graphics& o) = delete;

	void endFrame();
	void clearBuffer(float r, float g, float b);

	static void clamp(float& value, float min, float max);
	static float clamp(float value, float min, float max);
};

#endif
