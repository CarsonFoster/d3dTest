#ifndef CWF_MATERIAL_H
#define CWF_MATERIAL_H

#include "Graphics.h"
#include "ShaderStage.h"
#include "Submaterial.h"
#include <d3d11.h>
#include <initializer_list>
#include <optional>
#include <vector>
#include <wrl.h>

/*
* A material is a class-like object, representing a type of DirectX object that uses the same shader,resources,etc.
* You can add as many meshes as you want; they all just have to be of the same material.
* An übershader is one large shader that uses conditionals to determine which code to execute; this means that we
* don't have to load multiple shaders, which is expensive.
*/

template <class Vertex, typename Index>
class Material {
private:
	struct Shader {
		const void* pByteCode;
		size_t length;
		bool bind;
	};

	struct ConstantBuffer {
		const void* pBuffer;
		size_t length;
		ShaderStage stage;
		bool readOnly;
		ConstantBuffer(const void* p, size_t l, ShaderStage s, bool r) 
			: pBuffer{ p }, length{ l }, stage{ s }, readOnly{ r }{}
	};
private:
	// set by user
	D3D11_PRIMITIVE_TOPOLOGY pt;
	const D3D11_INPUT_ELEMENT_DESC* pDescs;
	size_t numberOfDescs;
	DXGI_FORMAT idxFormat;
	std::vector<Vertex> vtx;
	std::vector<Index> idx;
	std::vector<ConstantBuffer> cBuffs;
	std::vector<Submaterial<Vertex, Index>> subs;
	Shader vs;
	std::optional<Shader> oPS;
	std::optional<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> oPrtv;
	std::optional<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>> oPdsv;
	std::optional<D3D11_VIEWPORT> oVP;

	// generated by DirectX
	Microsoft::WRL::ComPtr<ID3D11CommandList> pCmdList;

public:
	Material(DXGI_FORMAT indexFormat) : pt{}, numberOfDescs{}, idxFormat{ indexFormat } {}

	DXGI_FORMAT getIndexFormat() const noexcept {
		return idxFormat;
	}

	// do not interact with DirectX
	void setTopology(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept {
		pt = topology;
	}

	void setInputLayout(const D3D11_INPUT_ELEMENT_DESC* pDescriptions, size_t size) noexcept {
		pDescs = pDescriptions;
		numberOfDescs = size;
	}

	void addMesh(std::initializer_list<Vertex> vertices, std::initializer_list<Index> indices) noexcept {
		vtx.insert(vtx.cend(), vertices);
		idx.insert(idx.cend(), indices);
	}

	void addMesh(std::vector<Vertex> vertices, std::vector<Index> indices) noexcept {
		vtx.insert(vtx.cend(), vertices.begin(), vertices.end());
		idx.insert(idx.cend(), indices.begin(), indices.end());
	}

	void addConstantBuffer(const void* pBuffer, size_t byteWidth, ShaderStage stage, bool readOnly = true) noexcept {
		cBuffs.emplace_back(pBuffer, byteWidth, stage, readOnly);
	}

	void setVertexShader(const void* pByteCode, size_t length, bool bind = true) noexcept {
		vs = { pByteCode, length, bind };
	}

	void setPixelShader(const void* pByteCode, size_t length) noexcept {								   // optional
		oPS = { pByteCode, length, true };
	}
	
	void setRenderTarget(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTarget, 
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> zbuffer) noexcept {								   // optional
		
		oPrtv = renderTarget;
		oPdsv = zbuffer;
	}

	void setViewport(D3D11_VIEWPORT viewport) noexcept {                                                   // optional
		oVP = viewport;
	}

	void setViewport(float topLeftX, float topLeftY, float width, float height) noexcept {
		D3D11_VIEWPORT vp{};
		vp.TopLeftX = topLeftX;
		vp.TopLeftY = topLeftY;
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		oVP = vp;
	}

	Submaterial<Vertex, Index>& createSubmaterial() noexcept {
		subs.emplace_back(this);
		return subs[subs.size() - 1];
	}


	void setupPipeline(const Graphics& gfx) {
		if (pCmdList) return; // do not re-generate resources

		Microsoft::WRL::ComPtr<ID3D11Device> pDevice{ gfx.getDevice() };
		
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeferred;
		THROW_IF_FAILED(gfx, pDevice->CreateDeferredContext(0, &pDeferred));

		setupPipeline(gfx, pDeferred, pCmdList);
	}

	//  should call in another thread for optimal performance
	void setupPipeline(const Graphics& gfx, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeferred, 
		Microsoft::WRL::ComPtr<ID3D11CommandList>& pListToFill, bool submaterialCalling = false) {
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice{ gfx.getDevice() };

		// vertex buffer
		if (!submaterialCalling) {
			D3D11_BUFFER_DESC vtxDesc{};
			vtxDesc.ByteWidth = vtx.size() * sizeof(Vertex);
			vtxDesc.Usage = D3D11_USAGE_DEFAULT;
			vtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vtxDesc.CPUAccessFlags = 0u;
			vtxDesc.MiscFlags = 0u;
			vtxDesc.StructureByteStride = sizeof(Vertex);

			D3D11_SUBRESOURCE_DATA vtxData{};
			vtxData.pSysMem = vtx.data();

			Microsoft::WRL::ComPtr<ID3D11Buffer> pVtxBuffer;
			THROW_IF_FAILED(gfx, pDevice->CreateBuffer(&vtxDesc, &vtxData, &pVtxBuffer));

			UINT stride{ sizeof(Vertex) };
			UINT offset{ 0u };

			pDeferred->IASetVertexBuffers(0u, 1u, pVtxBuffer.GetAddressOf(), &stride, &offset);
		}

		// index buffer
		if (!submaterialCalling) {
			D3D11_BUFFER_DESC idxDesc{};
			idxDesc.ByteWidth = idx.size() * sizeof(Index);
			idxDesc.Usage = D3D11_USAGE_DEFAULT;
			idxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			idxDesc.CPUAccessFlags = 0u;
			idxDesc.MiscFlags = 0u;
			idxDesc.StructureByteStride = sizeof(Index);

			D3D11_SUBRESOURCE_DATA idxData{};
			idxData.pSysMem = idx.data();

			Microsoft::WRL::ComPtr<ID3D11Buffer> pIdxBuffer;
			THROW_IF_FAILED(gfx, pDevice->CreateBuffer(&idxDesc, &idxData, &pIdxBuffer));

			pDeferred->IASetIndexBuffer(pIdxBuffer.Get(), idxFormat, 0u);
		}

		// constant buffer
		if (!submaterialCalling) {
			std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vertexBuffers;
			std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> pixelBuffers;
			for (ConstantBuffer& cb : cBuffs) {
				D3D11_BUFFER_DESC cbDesc{};
				cbDesc.ByteWidth = cb.length;
				cbDesc.Usage = (cb.readOnly ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC);
				cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				cbDesc.CPUAccessFlags = (cb.readOnly ? 0u : D3D11_CPU_ACCESS_WRITE);
				cbDesc.MiscFlags = 0u;
				cbDesc.StructureByteStride = 0u;

				D3D11_SUBRESOURCE_DATA cbData{};
				cbData.pSysMem = cb.pBuffer;

				Microsoft::WRL::ComPtr<ID3D11Buffer> pCBuff;
				THROW_IF_FAILED(gfx, pDevice->CreateBuffer(&cbDesc, &cbData, &pCBuff));

				switch (cb.stage) {
				case ShaderStage::VERTEX:
					vertexBuffers.push_back(pCBuff);
					break;
				case ShaderStage::PIXEL:
					pixelBuffers.push_back(pCBuff);
					break;
#ifndef NDEBUG
				default:
					OutputDebugStringW(L"Update your ShaderStage switch, dumb dumb.");
#endif
				}
			}
			std::vector<ID3D11Buffer*> vertexRawBuffers;
			std::vector<ID3D11Buffer*> pixelRawBuffers;
			for (auto& comPtr : vertexBuffers)
				vertexRawBuffers.push_back(comPtr.Get());
			for (auto& comPtr : pixelBuffers)
				pixelRawBuffers.push_back(comPtr.Get());
			if (!vertexRawBuffers.empty())
				pDeferred->VSSetConstantBuffers(0u, vertexRawBuffers.size(), vertexRawBuffers.data());
			if (!pixelRawBuffers.empty())
				pDeferred->VSSetConstantBuffers(0u, pixelRawBuffers.size(), pixelRawBuffers.data());
		}

		// primitive topology
		{
			pDeferred->IASetPrimitiveTopology(pt);
		}

		// vertex shader
		{
			if (vs.bind) {
				Microsoft::WRL::ComPtr<ID3D11VertexShader> pVtxShader;
				THROW_IF_FAILED(gfx, pDevice->CreateVertexShader(vs.pByteCode, vs.length, nullptr, &pVtxShader));
				pDeferred->VSSetShader(pVtxShader.Get(), nullptr, 0u);
			}
		}

		// pixel shader
		{
			if (oPS) {
				Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
				THROW_IF_FAILED(gfx, pDevice->CreatePixelShader(oPS->pByteCode, oPS->length, nullptr, &pPixelShader));
				pDeferred->PSSetShader(pPixelShader.Get(), nullptr, 0u);
			}
		}

		// input layout
		{
			Microsoft::WRL::ComPtr<ID3D11InputLayout> pLayout;
			THROW_IF_FAILED(gfx, pDevice->CreateInputLayout(pDescs, numberOfDescs, vs.pByteCode, vs.length, &pLayout));
			pDeferred->IASetInputLayout(pLayout.Get());
		}

		// render target and z buffer
		{
			if (oPrtv) {
				pDeferred->OMSetRenderTargets(1u, oPrtv->GetAddressOf(), oPdsv->Get());
			}
		}

		// viewport
		{
			if (oVP) {
				pDeferred->RSSetViewports(1u, &(*oVP));
			}
		}

		// draw command
		pDeferred->DrawIndexed(idx.size(), 0u, 0);

		// generate command list
		THROW_IF_FAILED(gfx, pDeferred->FinishCommandList(FALSE, &pListToFill));
	}

	// call on main thread
	void draw(const Graphics& gfx) {
		gfx.getImmediateContext()->ExecuteCommandList(pCmdList.Get(), FALSE);
	}
};

#endif