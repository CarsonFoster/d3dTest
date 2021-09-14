#define NOMINMAX

#include "Camera.h"
#include "CwfException.h"
#include "Graphics.h"
#include <array>
#include <cstddef>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <Windows.h>

namespace math = DirectX;

Graphics::Graphics(HWND hWnd, int clientWidth, int clientHeight)
	: m_clientWidth{ clientWidth }, m_clientHeight{ clientHeight },
	/*m_projection{},*/ m_camera{} {

	// math::XMStoreFloat4x4(&m_projection, math::XMMatrixIdentity());
	
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
			&m_pSwapChain,
			&m_pDevice,
			nullptr,
			&m_pContext
		)
	);

	RECT rect{};
	if (GetClientRect(hWnd, &rect)) {
		OutputDebugStringA((std::to_string(rect.right - rect.left) + " " + std::to_string(rect.bottom - rect.top) + "\n").c_str());
	}

	DXGI_SWAP_CHAIN_DESC tmp{};
	m_pSwapChain->GetDesc(&tmp);
	OutputDebugStringA((std::to_string(tmp.BufferDesc.Width) + " " + std::to_string(tmp.BufferDesc.Height) + "\n").c_str());

	Microsoft::WRL::ComPtr<ID3D11Resource> pBuffer;
	THROW_IF_FAILED(*this,
		m_pSwapChain->GetBuffer(0u, __uuidof(ID3D11Resource), &pBuffer)
	);

	THROW_IF_FAILED(*this,
		m_pDevice->CreateRenderTargetView(pBuffer.Get(), nullptr, &m_pTarget)
	);

	// Z Buffer setup below
	D3D11_DEPTH_STENCIL_DESC zBufferDesc{};
	zBufferDesc.DepthEnable = TRUE;
	zBufferDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	zBufferDesc.DepthFunc = D3D11_COMPARISON_LESS;
	zBufferDesc.StencilEnable = FALSE;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pState;
	THROW_IF_FAILED(*this,
		m_pDevice->CreateDepthStencilState(&zBufferDesc, &pState)
	);

	m_pContext->OMSetDepthStencilState(pState.Get(), 1u);

	D3D11_TEXTURE2D_DESC zBufferTextureDesc{};
	zBufferTextureDesc.Width = clientWidth;
	zBufferTextureDesc.Height = clientHeight;
	zBufferTextureDesc.MipLevels = 1u;
	zBufferTextureDesc.ArraySize = 1u;
	zBufferTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	zBufferTextureDesc.SampleDesc.Count = 1u;
	zBufferTextureDesc.SampleDesc.Quality = 0u;
	zBufferTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	zBufferTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	zBufferTextureDesc.CPUAccessFlags = 0;
	zBufferTextureDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pZBufferTexture;
	THROW_IF_FAILED(*this,
		m_pDevice->CreateTexture2D(&zBufferTextureDesc, nullptr, &pZBufferTexture)
	);

	D3D11_DEPTH_STENCIL_VIEW_DESC zBufferViewDesc;
	zBufferViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	zBufferViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	zBufferViewDesc.Flags = 0;
	zBufferViewDesc.Texture2D.MipSlice = 0;

	THROW_IF_FAILED(*this,
		m_pDevice->CreateDepthStencilView(pZBufferTexture.Get(), &zBufferViewDesc, &m_pZBuffer)
	);

	m_pContext->OMSetRenderTargets(1u, m_pTarget.GetAddressOf(), m_pZBuffer.Get());
}

void Graphics::endFrame() {
	// TODO: frame rate management
	// Present( SyncInterval, Flags)
	THROW_IF_FAILED(*this,
		m_pSwapChain->Present(1u, 0u)
	);
}

void Graphics::clearBuffer(float r, float g, float b) {
	const float colorRGBA[] = { r, g, b, 1.0f };
	m_pContext->ClearRenderTargetView(m_pTarget.Get(), colorRGBA); // does not return an HRESULT
	m_pContext->ClearDepthStencilView(m_pZBuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::drawTestCube(bool isLeftPressed, bool isRightPressed, bool isUpPressed, bool isDownPressed) {
	struct Vertex2D {
		struct {
			float x;
			float y;
			float z;
		} pos;
	};

	const constexpr float half_S = 0.5f;
	const Vertex2D vertices[]{
		{-half_S, half_S, 0},     // 0
		{-half_S, -half_S, 0},    // 1
		{half_S, -half_S, 0},     // 2
		{half_S, half_S, 0},      // 3
		{-half_S, half_S, 1.0f},  // 4
		{-half_S, -half_S, 1.0f}, // 5
		{half_S, -half_S, 1.0f},  // 6
		{half_S, half_S, 1.0f},   // 7
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
	THROW_IF_FAILED(*this, m_pDevice->CreateBuffer(&bDesc, &data, &pBuffer));

	UINT stride{ sizeof(Vertex2D) };
	UINT offset{ 0u };
	m_pContext->IASetVertexBuffers(0u, 1u, pBuffer.GetAddressOf(), &stride, &offset);

	const uint16_t indices[]{
		1,0,3,  2,1,3, // front face
		5,4,0,  1,5,0, // left face
		2,3,7,  6,2,7, // right face
		6,7,4,  5,6,4, // back face
		0,4,7,  3,0,7, // top face
		5,1,2,  6,5,2  // bottom face
	};

	D3D11_BUFFER_DESC ibDesc{};
	ibDesc.ByteWidth = sizeof(indices);
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0u;
	ibDesc.MiscFlags = 0u;
	ibDesc.StructureByteStride = sizeof(uint16_t);

	D3D11_SUBRESOURCE_DATA iData{};
	iData.pSysMem = &indices;
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	THROW_IF_FAILED(*this, m_pDevice->CreateBuffer(&ibDesc, &iData, &pIndexBuffer));

	m_pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	static float angleY{ 0.0f };
	angleY += (isLeftPressed ? 0.1f : isRightPressed ? -0.1f : 0.0f);
	static float angleX{ 0.0f };
	angleX += (isUpPressed ? 0.1f : isDownPressed ? -0.1f : 0.0f);
	struct CBufMatrixTransform {
		math::XMMATRIX transform;
	};
	CBufMatrixTransform t{ math::XMMatrixTranspose(
								math::XMMatrixRotationX(angleX)
								* math::XMMatrixRotationY(angleY)
								* math::XMMatrixTranslation(0.0f, 0.0f, 2.0f)
								* math::XMMatrixPerspectiveLH(1.0f, 1.0f, 0.5f, 4.0f)) };
	
	D3D11_BUFFER_DESC cbDesc{};
	cbDesc.ByteWidth = sizeof(t);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0u;
	cbDesc.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA cbData{};
	cbData.pSysMem = &t;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pCBuffer;
	THROW_IF_FAILED(*this, m_pDevice->CreateBuffer(&cbDesc, &cbData, &pCBuffer));

	m_pContext->VSSetConstantBuffers(0u, 1u, pCBuffer.GetAddressOf());

	struct CBufColors {
		float colors[6][4];
	};
	CBufColors cBuffer{
		{
			{1.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f, 1.0f},
			{0.5f, 0.0f, 0.5f, 1.0f},
			{0.0f, 0.5f, 0.5f, 1.0f},
			{0.5f, 0.5f, 0.0, 1.0f}
		}
	};

	cbDesc.ByteWidth = sizeof(cBuffer);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0u;
	cbDesc.StructureByteStride = 0u;
	cbData.pSysMem = &cBuffer;

	THROW_IF_FAILED(*this, m_pDevice->CreateBuffer(&cbDesc, &cbData, &pCBuffer));

	m_pContext->PSSetConstantBuffers(0u, 1u, pCBuffer.GetAddressOf());

	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	THROW_IF_FAILED(*this, D3DReadFileToBlob(L"TestTriangleVertexShader.cso", &pBlob));
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	THROW_IF_FAILED(*this, m_pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
	m_pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	const D3D11_INPUT_ELEMENT_DESC pDescs[] = {
		{ "Position", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
	};
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	THROW_IF_FAILED(*this,
		m_pDevice->CreateInputLayout(pDescs, std::size(pDescs), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout));
	m_pContext->IASetInputLayout(pInputLayout.Get());

	THROW_IF_FAILED(*this, D3DReadFileToBlob(L"TestTrianglePixelShader.cso", &pBlob));
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	THROW_IF_FAILED(*this, m_pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
	m_pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = m_clientWidth;
	viewport.Height = m_clientHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pContext->RSSetViewports(1u, &viewport);

	THROW_ON_INFO(*this, m_pContext->DrawIndexed(std::size(indices), 0u, 0));
}

HRESULT Graphics::getDeviceRemovedReason() const noexcept {
	if (m_pDevice) return m_pDevice->GetDeviceRemovedReason();
	return S_OK;
}

Microsoft::WRL::ComPtr<ID3D11Device> Graphics::getDevice() const noexcept {
	return m_pDevice;
}

Microsoft::WRL::ComPtr<ID3D11DeviceContext> Graphics::getImmediateContext() const noexcept {
	return m_pContext;
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView> Graphics::getRenderTargetView() const noexcept {
	return m_pTarget;
}

Microsoft::WRL::ComPtr<ID3D11DepthStencilView> Graphics::getZBuffer() const noexcept {
	return m_pZBuffer;
}

void Graphics::setProjection(float fov_deg, float nearZ, float farZ) noexcept {
	float aspectRatio = static_cast<float>(m_clientWidth) / static_cast<float>(m_clientHeight);
	math::XMStoreFloat4x4(&m_projection, math::XMMatrixPerspectiveFovLH(math::XMConvertToRadians(fov_deg), aspectRatio, nearZ, farZ));
}

const math::XMMATRIX& Graphics::getProjection() const noexcept {
	return math::XMLoadFloat4x4(&m_projection);
}

const Camera& Graphics::camera() const noexcept {
	return m_camera;
}

Camera& Graphics::camera() noexcept {
	return m_camera;
}