#define NOMINMAX
#include "Graphics.h"
#include <d3d11.h>
#include <Windows.h>

Graphics::Graphics(HWND hWnd) {
	DXGI_SWAP_CHAIN_DESC swapChainDescriptor{};
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

	// TODO: check return value of D3D11CreateDeviceAndSwapChain
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0, // TODO: enable debug here later
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDescriptor,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pContext
	);

	Microsoft::WRL::ComPtr<ID3D11Resource> pBuffer;
	// TODO: check return value of GetBuffer
	pSwapChain->GetBuffer(0u, __uuidof(ID3D11Resource), &pBuffer);
	// TODO: check return value of CreateRenderTargetView
	pDevice->CreateRenderTargetView(pBuffer.Get(), nullptr, &pTarget);
}

void Graphics::endFrame() {
	// TODO: frame rate management
	// Present( SyncInterval, Flags)
	pSwapChain->Present(1u, 0u);
}

void Graphics::clearBuffer(float r, float g, float b) {
	const float colorRGBA[] = { r, g, b, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), colorRGBA);
}

HRESULT Graphics::getDeviceRemovedReason() const noexcept {
	if (pDevice) return pDevice->GetDeviceRemovedReason();
	return S_OK;
}