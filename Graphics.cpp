#include "Graphics.h"
#include <d3d11.h>
#include <Windows.h>

Graphics::Graphics(HWND hWnd) {
	DXGI_SWAP_CHAIN_DESC swapChainDescriptor{};
	// TODO: fill these in with real values
	swapChainDescriptor.BufferDesc.Width = 0; // get width from output window
	swapChainDescriptor.BufferDesc.Height = 0; // get height from output window
	swapChainDescriptor.BufferDesc.RefreshRate.Numerator = 0; // don't care about refresh rate
	swapChainDescriptor.BufferDesc.RefreshRate.Denominator = 0;
	swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDescriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDescriptor.SampleDesc.Count = 1; // no AA
	swapChainDescriptor.SampleDesc.Quality = 0; // no AA
	swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // render to our back buffer
	swapChainDescriptor.BufferCount = 1; // windowed, so desktop is front buffer and 1 means one back buffer
	swapChainDescriptor.OutputWindow = hWnd;
	swapChainDescriptor.Windowed = TRUE; // windowed
	swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDescriptor.Flags = 0;

	// TODO: check return value
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0, // TODO: enable debug here later
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDescriptor,
		&swapChain,
		&device,
		nullptr,
		&context
	);
}