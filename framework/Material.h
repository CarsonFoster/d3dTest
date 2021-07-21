#ifndef CWF_MATERIAL_H
#define CWF_MATERIAL_H

#include "Graphics.h"
#include "ShaderStage.h"
#include "Submaterial.h"
#include "lib/DirectXTK/DDSTextureLoader.h"
#include <cstddef> // for std::byte
#include <cstring> // for std::memcpy
#include <d3d11.h>
#include <initializer_list>
#include <memory> // std::unique_ptr
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
	std::vector<std::unique_ptr<std::byte[]>> copiedConstantBuffers;
	std::vector<std::unique_ptr<std::byte[], Graphics::AlignedDeleter>> copiedAlignedConstantBuffers;
	std::vector<ConstantBuffer> cBuffs;
	Shader vs;
	std::optional<Shader> oPS;
	std::optional<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> oPrtv;
	std::optional<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>> oPdsv;
	std::optional<D3D11_VIEWPORT> oVP;
	std::optional<Graphics::Texture2D> oTex2D;

	// need to maintain pointers to these for GPU
	struct {
		struct {
			Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer{};
			UINT stride{};
			UINT offset{};
		} vertex{};
		struct {
			Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer{};
		} index{};
		struct {
			std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vertexBuffers{};
			std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> pixelBuffers{};
			std::vector<ID3D11Buffer*> vertexRawBuffers{};
			std::vector<ID3D11Buffer*> pixelRawBuffers{};
		} constant{};
		struct {
			Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertex{};
			Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixel{};
		} shader{};
		struct {
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRView{};
			Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler{};
		} texture2D{};
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pLayout{};
	} Data{};

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

	void addMesh(const Graphics::IndexedVertexList<Vertex, Index>& mesh) noexcept {
		vtx.insert(vtx.cend(), mesh.vertices.begin(), mesh.vertices.end());
		idx.insert(idx.cend(), mesh.indices.begin(), mesh.indices.end());
	}

	void addConstantBuffer(const void* pBuffer, size_t byteWidth, ShaderStage stage, bool readOnly = true) noexcept {
		cBuffs.emplace_back(pBuffer, byteWidth, stage, readOnly);
	}

	void copyConstantBuffer(const void* pBuffer, size_t byteWidth, ShaderStage stage, bool readOnly = true, bool aligned = false) {
		if (!aligned) {
			auto copiedBuffer = std::make_unique<std::byte[]>(byteWidth);
			std::memcpy(copiedBuffer.get(), pBuffer, byteWidth);
			cBuffs.emplace_back(copiedBuffer.get(), byteWidth, stage, readOnly);
			copiedConstantBuffers.push_back(std::move(copiedBuffer));
		} else {
			void* raw = _aligned_malloc(byteWidth, 16);
			std::memcpy(raw, pBuffer, byteWidth);
			cBuffs.emplace_back(raw, byteWidth, stage, readOnly);
			copiedAlignedConstantBuffers.push_back(std::unique_ptr<std::byte[], Graphics::AlignedDeleter>{ reinterpret_cast<std::byte*>(raw) });
		}
	}

	void updateCopyConstantBuffer(size_t index, const Graphics& gfx, const void* pBuffer, size_t byteWidth) { // expensive
		if (index >= cBuffs.size() || cBuffs[index].readOnly || !pCmdList) return;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pImmediateContext{ gfx.getImmediateContext() };
		D3D11_MAPPED_SUBRESOURCE mappedResource{ 0 };
		ID3D11Buffer* pConstantBuffer{};
		switch (cBuffs[index].stage) {
		case ShaderStage::VERTEX:
			pConstantBuffer = Data.constant.vertexRawBuffers[index];
			break;
		case ShaderStage::PIXEL:
			pConstantBuffer = Data.constant.pixelRawBuffers[index];
			break;
		default:
#ifndef NDEBUG
			OutputDebugStringW(L"Update your other stage switch, dumb dumb.\n");
#endif
			return;
		}
		THROW_IF_FAILED(gfx, 
			pImmediateContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
		std::memcpy(mappedResource.pData, pBuffer, byteWidth);
		pImmediateContext->Unmap(pConstantBuffer, 0);
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

	void setTexture2D(Graphics::Texture2D texture) {
		oTex2D = texture;
	}

	// TODO: copy version of setTexture2D

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

			THROW_IF_FAILED(gfx, pDevice->CreateBuffer(&vtxDesc, &vtxData, &Data.vertex.pBuffer));

			Data.vertex.stride = sizeof(Vertex);
			Data.vertex.offset = 0u;

			pDeferred->IASetVertexBuffers(0u, 1u, Data.vertex.pBuffer.GetAddressOf(), &Data.vertex.stride, &Data.vertex.offset);
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

			THROW_IF_FAILED(gfx, pDevice->CreateBuffer(&idxDesc, &idxData, &Data.index.pBuffer));

			pDeferred->IASetIndexBuffer(Data.index.pBuffer.Get(), idxFormat, 0u);
		}

		// constant buffer
		if (!submaterialCalling) {
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
					Data.constant.vertexBuffers.push_back(pCBuff);
					break;
				case ShaderStage::PIXEL:
					Data.constant.pixelBuffers.push_back(pCBuff);
					break;
#ifndef NDEBUG
				default:
					OutputDebugStringW(L"Update your ShaderStage switch, dumb dumb.");
#endif
				}
			}
			for (auto& comPtr : Data.constant.vertexBuffers)
				Data.constant.vertexRawBuffers.push_back(comPtr.Get());
			for (auto& comPtr : Data.constant.pixelBuffers)
				Data.constant.pixelRawBuffers.push_back(comPtr.Get());
			if (!Data.constant.vertexRawBuffers.empty())
				pDeferred->VSSetConstantBuffers(0u, Data.constant.vertexRawBuffers.size(), Data.constant.vertexRawBuffers.data());
			if (!Data.constant.pixelRawBuffers.empty())
				pDeferred->VSSetConstantBuffers(0u, Data.constant.pixelRawBuffers.size(), Data.constant.pixelRawBuffers.data());
		}

		// primitive topology
		{
			pDeferred->IASetPrimitiveTopology(pt);
		}

		// vertex shader
		{
			if (vs.bind) {
				if (!Data.shader.pVertex) // if already generated, skip
					THROW_IF_FAILED(gfx, pDevice->CreateVertexShader(vs.pByteCode, vs.length, nullptr, &Data.shader.pVertex));
				pDeferred->VSSetShader(Data.shader.pVertex.Get(), nullptr, 0u);
			}
		}

		// pixel shader
		{
			if (oPS) {
				if (!Data.shader.pPixel) // if already generated, skip
					THROW_IF_FAILED(gfx, pDevice->CreatePixelShader(oPS->pByteCode, oPS->length, nullptr, &Data.shader.pPixel));
				pDeferred->PSSetShader(Data.shader.pPixel.Get(), nullptr, 0u);
			}
		}

		// input layout
		{
			if (!Data.pLayout) // if already generated, skip
				THROW_IF_FAILED(gfx, pDevice->CreateInputLayout(pDescs, numberOfDescs, vs.pByteCode, vs.length, &Data.pLayout));
			pDeferred->IASetInputLayout(Data.pLayout.Get());
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

		// texture
		{
			if (oTex2D) {
				bool valid_type{ true };
				if (std::holds_alternative<Graphics::Texture2D::RawData>(oTex2D->content)) {
					Graphics::Texture2D::RawData& rd{ std::get<Graphics::Texture2D::RawData>(oTex2D->content) };
					D3D11_TEXTURE2D_DESC textureDesc{};
					textureDesc.Width = rd.width;
					textureDesc.Height = rd.height;
					textureDesc.MipLevels = rd.mipLevels;
					textureDesc.ArraySize = rd.arraySize;
					textureDesc.Format = rd.format;
					textureDesc.SampleDesc.Count = rd.sampleCount;
					textureDesc.SampleDesc.Quality = rd.sampleQuality;
					textureDesc.Usage = D3D11_USAGE_DEFAULT;
					textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
					textureDesc.CPUAccessFlags = rd.cpuAccessFlags;
					textureDesc.MiscFlags = rd.miscFlags;

					D3D11_SUBRESOURCE_DATA textureData{};
					textureData.pSysMem = rd.pData;
					textureData.SysMemPitch = rd.pitch;

					Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
					THROW_IF_FAILED(gfx, pDevice->CreateTexture2D(&textureDesc, &textureData, &pTexture));

					D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
					srvDesc.Format = rd.format;
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MostDetailedMip = rd.mostDetailedMip;
					srvDesc.Texture2D.MipLevels = rd.mipLevels;

					THROW_IF_FAILED(gfx, pDevice->CreateShaderResourceView(pTexture.Get(), &srvDesc, &Data.texture2D.pSRView));

					pDeferred->PSSetShaderResources(0u, 1u, Data.texture2D.pSRView.GetAddressOf());
				} else if (std::holds_alternative<Graphics::Texture2D::File>(oTex2D->content)) {
					Graphics::Texture2D::File& file{ std::get<Graphics::Texture2D::File>(oTex2D->content) };
					THROW_IF_FAILED(gfx,
						DirectX::CreateDDSTextureFromFile(pDevice.Get(), file.filename, nullptr, &Data.texture2D.pSRView)
					);
					pDeferred->PSSetShaderResources(0u, 1u, Data.texture2D.pSRView.GetAddressOf());
				} else {
					valid_type = false;
#ifndef NDEBUG
					OutputDebugStringW(L"Texture2D variant had an unexpected type (update your if statement)");
#endif
				}

				if (valid_type) { // only execute if the type was valid
					D3D11_SAMPLER_DESC samplerDesc{};
					samplerDesc.Filter = oTex2D->sampler.filter;
					samplerDesc.AddressU = oTex2D->sampler.u;
					samplerDesc.AddressV = oTex2D->sampler.v;
					samplerDesc.AddressW = oTex2D->sampler.w;
					samplerDesc.MipLODBias = oTex2D->sampler.mipLODBias;
					samplerDesc.MaxAnisotropy = oTex2D->sampler.maxAnisotropy;
					samplerDesc.ComparisonFunc = oTex2D->sampler.comparisonFunc;
					samplerDesc.BorderColor[0] = oTex2D->sampler.borderColor.r;
					samplerDesc.BorderColor[1] = oTex2D->sampler.borderColor.g;
					samplerDesc.BorderColor[2] = oTex2D->sampler.borderColor.b;
					samplerDesc.BorderColor[3] = oTex2D->sampler.borderColor.a;
					samplerDesc.MinLOD = oTex2D->sampler.minLOD;
					samplerDesc.MaxLOD = oTex2D->sampler.maxLOD;

					THROW_IF_FAILED(gfx, pDevice->CreateSamplerState(&samplerDesc, &Data.texture2D.pSampler));

					pDeferred->PSSetSamplers(0u, 1u, Data.texture2D.pSampler.GetAddressOf());
				}
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