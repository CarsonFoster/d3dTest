#ifndef CWF_SUBMATERIAL_H
#define CWF_SUBMATERIAL_H

#include "Graphics.h"
#include "ShaderStage.h"
#include <cstddef> // std::byte
#include <cstring> // std::memcpy
#include <d3d11.h>
#include <memory>
#include <vector>
#include <wrl.h>

/*
* A submaterial is a class-like object, representing a type of DirectX object that uses the same shader and general info
* as a material, but requires different constant buffers. They should only exist as "children" of materials.
* You can add as many meshes as you want; they all just have to be of the same material.
*/

template<class Vertex, typename Index>
class Material;

template <class Vertex, typename Index>
class Submaterial {
private:
	struct ConstantBuffer {
		const void* pBuffer;
		size_t length;
		ShaderStage stage;
		bool readOnly;
		ConstantBuffer(const void* p, size_t l, ShaderStage s, bool r)
			: pBuffer{ p }, length{ l }, stage{ s }, readOnly{ r }{}
	};
private:
	Material<Vertex, Index>& parent;
	std::vector<Vertex> vtx;
	std::vector<Index> idx;
	std::vector<std::unique_ptr<std::byte[]>> copiedConstantBuffers;
	std::vector<std::unique_ptr<std::byte[], Graphics::AlignedDeleter>> copiedAlignedConstantBuffers;
	std::vector<ConstantBuffer> cBuffs;

	// need to maintain for GPU access later
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
	} Data{};

	// generated by DirectX
	Microsoft::WRL::ComPtr<ID3D11CommandList> pCmdList;

public:
	Submaterial(Material<Vertex, Index>& parentMaterial) : parent{ parentMaterial }, vtx{}, idx{}, cBuffs{}, pCmdList{} {}

	// do not interact with DirectX
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
		cBuffs.emplace_back( pBuffer, byteWidth, stage, readOnly );
	}

	void copyConstantBuffer(const void* pBuffer, size_t byteWidth, ShaderStage stage, bool readOnly = true, bool aligned = false) {
		if (!aligned) {
			auto bytes = std::make_unique<std::byte[]>(byteWidth);
			std::memcpy(bytes.get(), pBuffer, byteWidth);
			cBuffs.emplace_back(bytes.get(), byteWidth, stage, readOnly);
			copiedConstantBuffers.push_back(std::move(bytes));
		}
		else {
			void* raw = _aligned_malloc(byteWidth, 16);
			std::memcpy(raw, pBuffer, byteWidth);
			cBuffs.emplace_back(raw, byteWidth, stage, readOnly);
			copiedAlignedConstantBuffers.push_back(std::unique_ptr<std::byte[], Graphics::AlignedDeleter>{raw});
		}
	}

	void updateCopyConstantBuffer(size_t index, const Graphics& gfx, const void* pBuffer, size_t byteWidth) { // expensive
		// TODO: alignment
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
#ifndef NDEBUG
		default:
			OutputDebugStringW(L"Update your other stage switch (submaterial), dumb dumb.\n");
#endif
		}
		THROW_IF_FAILED(gfx,
			pImmediateContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
		std::memcpy(mappedResource.pData, pBuffer, byteWidth);
		pImmediateContext->Unmap(pConstantBuffer, 0);
	}
	
	// call in another thread for optimal performance
	void setupPipeline(const Graphics& gfx) {
		if (pCmdList) return; // do not re-generate resources
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice{ gfx.getDevice() };

		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeferred;
		THROW_IF_FAILED(gfx, pDevice->CreateDeferredContext(0, &pDeferred));

		// vertex buffer
		{
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

			pDeferred->IASetVertexBuffers(0u, 1u, Data.vertex.pBuffer.GetAddressOf(), 
				&Data.vertex.stride, &Data.vertex.offset);
		}

		// index buffer
		{
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

			pDeferred->IASetIndexBuffer(Data.index.pBuffer.Get(), parent.getIndexFormat(), 0u);
		}

		// constant buffer
		{
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

		parent.setupPipeline(gfx, pDeferred, pCmdList, true);
	}

	void draw(const Graphics& gfx) {
		gfx.getImmediateContext()->ExecuteCommandList(pCmdList.Get(), FALSE);
	}
};

#endif