#include "Graphics.h"
#include <d3d11.h>
#include <Windows.h>

Graphics::Graphics(HWND hWnd) {
	DXGI_SWAP_CHAIN_DESC swapChainDescriptor{};
	// TODO: fill these in with real values
	swapChainDescriptor.BufferDesc = 0;
	swapChainDescriptor.SampleDesc = 0;
	swapChainDescriptor.BufferUsage = 0;
	swapChainDescriptor.BufferCount = 0;
	swapChainDescriptor.OutputWindow = 0;
	swapChainDescriptor.Windowed = 0;
	swapChainDescriptor.SwapEffect = 0;
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