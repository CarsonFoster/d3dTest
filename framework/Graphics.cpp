#define NOMINMAX
#include "CwfException.h"
#include "Graphics.h"
#include <array>
#include <cmath>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <Windows.h>

inline void Graphics::throwIfFailed(const Graphics& gfx, HRESULT hr, const char* file, int line) {
	if (FAILED(hr)) {
		throw CwfException{ gfx, CwfException::DirectXErrorString{ hr }, file, line };
	}
}

inline void Graphics::throwIfFailedNoGfx(HRESULT hr, const char* file, int line) {
	if (FAILED(hr)) {
		throw CwfException{ CwfException::DirectXErrorString{ hr }, file, line };
	}
}

const float SQRTTHREE{ std::sqrt(3.0f) };

Graphics::Graphics(HWND hWnd, int cWidth, int cHeight) : clientWidth{ cWidth }, clientHeight{ cHeight } {
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

	UINT deviceFlags = 0;
#ifndef NDEBUG
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	THROW_IF_FAILED(*this,
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			deviceFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swapChainDescriptor,
			&pSwapChain,
			&pDevice,
			nullptr,
			&pContext
		)
	);

	Microsoft::WRL::ComPtr<ID3D11Resource> pBuffer;
	THROW_IF_FAILED(*this,
		pSwapChain->GetBuffer(0u, __uuidof(ID3D11Resource), &pBuffer)
	);

	THROW_IF_FAILED(*this,
		pDevice->CreateRenderTargetView(pBuffer.Get(), nullptr, &pTarget)
	);
}

void Graphics::endFrame() {
	// TODO: frame rate management
	// Present( SyncInterval, Flags)
	THROW_IF_FAILED(*this,
		pSwapChain->Present(1u, 0u)
	);
}

void Graphics::clearBuffer(float r, float g, float b) {
	const float colorRGBA[] = { r, g, b, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), colorRGBA); // does not return an HRESULT
}

void Graphics::drawTestTriangle() {
	struct Vertex2D {
		struct {
			float x;
			float y;
		} pos;
		struct {
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} color;
	};

	const constexpr float S = 1.5f;
	const float x = S * SQRTTHREE * 0.5f;
	const Vertex2D vertices[]{
		{0, x * 0.5f, 255, 0, 0, 0},
		{0.5f * S, -x * 0.5f, 0, 255, 0, 0},
		{-0.5f * S, -x * 0.5f, 0, 0, 255, 0}
	}; // NDC
	
	D3D11_BUFFER_DESC bDesc{};
	bDesc.ByteWidth = sizeof(vertices);
	bDesc.Usage = D3D11_USAGE_DEFAULT;
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.CPUAccessFlags = 0u;
	bDesc.MiscFlags = 0u;
	bDesc.StructureByteStride = sizeof(Vertex2D);

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = &vertices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
	THROW_IF_FAILED(*this, pDevice->CreateBuffer(&bDesc, &data, &pBuffer));

	UINT stride{ sizeof(Vertex2D) };
	UINT offset{ 0u };
	pContext->IASetVertexBuffers(0u, 1u, pBuffer.GetAddressOf(), &stride, &offset);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	THROW_IF_FAILED(*this, D3DReadFileToBlob(L"TestTriangleVertexShader.cso", &pBlob));
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	THROW_IF_FAILED(*this, pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	const D3D11_INPUT_ELEMENT_DESC pDescs[] = {
		{ "Position", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
		{ "Color", 0u, DXGI_FORMAT_R8G8B8A8_UNORM, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u}
	};
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	THROW_IF_FAILED(*this,
		pDevice->CreateInputLayout(pDescs, std::size(pDescs), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout));
	pContext->IASetInputLayout(pInputLayout.Get());

	THROW_IF_FAILED(*this, D3DReadFileToBlob(L"TestTrianglePixelShader.cso", &pBlob));
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	THROW_IF_FAILED(*this, pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = clientWidth;
	viewport.Height = clientHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	pContext->RSSetViewports(1u, &viewport);

	THROW_ON_INFO(*this, pContext->Draw(std::size(vertices), 0u));
}

HRESULT Graphics::getDeviceRemovedReason() const noexcept {
	if (pDevice) return pDevice->GetDeviceRemovedReason();
	return S_OK;
}