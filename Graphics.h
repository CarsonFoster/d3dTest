#ifndef CWF_GRAPHICS_H
#define CWF_GRAPHICS_H

#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>

class Graphics {
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
public:
	Graphics(HWND hWnd);
	~Graphics() = default;

	// no copy init/assign
	Graphics(const Graphics& o) = delete;
	Graphics& operator=(const Graphics& o) = delete;
};

#endif
