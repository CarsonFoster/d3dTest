#ifndef CWF_SUBMATERIAL_H
#define CWF_SUBMATERIAL_H

#include "Graphics.h"
#include "ShaderStage.h"
#include <d3d11.h>
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
	std::vector<ConstantBuffer> cBuffs;

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

			Microsoft::WRL::ComPtr<ID3D11Buffer> pVtxBuffer;
			THROW_IF_FAILED(gfx, pDevice->CreateBuffer(&vtxDesc, &vtxData, &pVtxBuffer));

			UINT stride{ sizeof(Vertex) };
			UINT offset{ 0u };

			pDeferred->IASetVertexBuffers(0u, 1u, pVtxBuffer.GetAddressOf(), &stride, &offset);
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

			Microsoft::WRL::ComPtr<ID3D11Buffer> pIdxBuffer;
			THROW_IF_FAILED(gfx, pDevice->CreateBuffer(&idxDesc, &idxData, &pIdxBuffer));

			pDeferred->IASetIndexBuffer(pIdxBuffer.Get(), parent.getIndexFormat(), 0u);
		}

		// constant buffer
		{
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

		parent.setupPipeline(gfx, pDeferred, pCmdList, true);
	}

	void draw(const Graphics& gfx) {
		gfx.getImmediateContext()->ExecuteCommandList(pCmdList.Get(), FALSE);
	}
};

#endif