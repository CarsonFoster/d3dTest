#ifndef CWF_CUBESKINNED_H
#define CWF_CUBESKINNED_H

#include "Graphics.h"
#include "Material.h"
#include "ShapeConcepts.h"
#include "WStringLiteral.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

namespace math = DirectX;

template <WStringLiteral file, VertexAndTexture Vtx>
class CubeSkinned {
public:
	using Idx = uint16_t;
private:
	static Material<Vtx, Idx> cube;
	static constexpr D3D11_INPUT_ELEMENT_DESC layout[]{
		{"Position", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
		{"TextureCoord", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u}
	};
public:
	static Graphics::IndexedVertexList<Vtx, Idx> mesh() {
		constexpr float third = (1.0f / 3.0f);
		Graphics::IndexedVertexList<Vtx, Idx> list{};
		// front face
		list.vertices.emplace_back(-0.5f, 0.5f, -0.5f);  // 0
		list.vertices.emplace_back(-0.5f, -0.5f, -0.5f); // 1
		list.vertices.emplace_back(0.5f, -0.5f, -0.5f);  // 2
		list.vertices.emplace_back(0.5f, 0.5f, -0.5f);   // 3
		list.vertices[0].tex.set(third, 0.25f);
		list.vertices[1].tex.set(third, 0.5f);
		list.vertices[2].tex.set(2 * third, 0.5f);
		list.vertices[3].tex.set(2 * third, 0.25f);

		// back face
		list.vertices.emplace_back(-0.5f, 0.5f, 0.5f);  // 4
		list.vertices.emplace_back(-0.5f, -0.5f, 0.5f); // 5
		list.vertices.emplace_back(0.5f, -0.5f, 0.5f);  // 6
		list.vertices.emplace_back(0.5f, 0.5f, 0.5f);   // 7
		list.vertices[4].tex.set(third, 1.0f);
		list.vertices[5].tex.set(third, 0.75f);
		list.vertices[6].tex.set(2 * third, 0.75f);
		list.vertices[7].tex.set(2 * third, 1.0f);

		// top face
		list.vertices.emplace_back(-0.5f, 0.5f, 0.5f); // 8
		list.vertices.emplace_back(0.5f, 0.5f, 0.5f);  // 9
		list.vertices[8].tex.set(third, 0.0f);
		list.vertices[9].tex.set(2 * third, 0.0f);

		// left face
		list.vertices.emplace_back(-0.5f, 0.5f, 0.5f);  // 10
		list.vertices.emplace_back(-0.5f, -0.5f, 0.5f); // 11
		list.vertices[10].tex.set(0.0f, 0.25f);
		list.vertices[11].tex.set(0.0f, 0.5f);

		// right face
		list.vertices.emplace_back(0.5f, 0.5f, 0.5f);  // 12
		list.vertices.emplace_back(0.5f, -0.5f, 0.5f); // 13
		list.vertices[12].tex.set(1.0f, 0.25f);
		list.vertices[13].tex.set(1.0f, 0.5f);

		list.indices = {
			0,3,1,    1,3,2,   // front face
			7,4,6,    6,4,5,   // back face
			10,0,11,  11,0,1,  // left face
			3,12,2,   2,12,13, // right face
			8,9,0,    0,9,3,   // top face
			1,2,5,    5,2,6    // bottom face
		};
		return list;
	}

	static void XM_CALLCONV addMesh(math::FXMMATRIX t) {
		math::XMFLOAT4 vtx[] {
			{ -0.5f, 0.5f, -0.5f },
			{ -0.5f, -0.5f, -0.5f },
			{ 0.5f, -0.5f, -0.5f },
			{ 0.5f, 0.5f, -0.5f },
			{ -0.5f, 0.5f, 0.5f },
			{ -0.5f, -0.5f, 0.5f },
			{ 0.5f, -0.5f, 0.5f },
			{ 0.5f, 0.5f, 0.5f },
			{ -0.5f, 0.5f, 0.5f },
			{ 0.5f, 0.5f, 0.5f },
			{ -0.5f, 0.5f, 0.5f },
			{ -0.5f, -0.5f, 0.5f },
			{ 0.5f, 0.5f, 0.5f },
			{ 0.5f, -0.5f, 0.5f },
		};

		constexpr size_t vtxSize{ std::size(vtx) };

		math::XMFLOAT4 dst[vtxSize]{};
		std::vector<Vtx> finalVtx{};

		math::XMVector4TransformStream(
			dst,
			sizeof(math::XMFLOAT4),
			vtx,
			sizeof(math::XMFLOAT4),
			vtxSize,
			t
		);

		for (auto& v : dst)
			finalVtx.emplace_back(v.x, v.y, v.z);

		constexpr float third = (1.0f / 3.0f);
		finalVtx[0].tex.set(third, 0.25f);
		finalVtx[1].tex.set(third, 0.5f);
		finalVtx[2].tex.set(2 * third, 0.5f);
		finalVtx[3].tex.set(2 * third, 0.25f);
		finalVtx[4].tex.set(third, 1.0f);
		finalVtx[5].tex.set(third, 0.75f);
		finalVtx[6].tex.set(2 * third, 0.75f);
		finalVtx[7].tex.set(2 * third, 1.0f);
		finalVtx[8].tex.set(third, 0.0f);
		finalVtx[9].tex.set(2 * third, 0.0f);
		finalVtx[10].tex.set(0.0f, 0.25f);
		finalVtx[11].tex.set(0.0f, 0.5f);
		finalVtx[12].tex.set(1.0f, 0.25f);
		finalVtx[13].tex.set(1.0f, 0.5f);

		std::vector<Idx> indices = {
			0,3,1,    1,3,2,   // front face
			7,4,6,    6,4,5,   // back face
			10,0,11,  11,0,1,  // left face
			3,12,2,   2,12,13, // right face
			8,9,0,    0,9,3,   // top face
			1,2,5,    5,2,6    // bottom face
		};
		cube.addMesh(finalVtx, indices);
	}

	static void addMesh() {
		cube.addMesh(mesh());
	}

	static constexpr D3D11_PRIMITIVE_TOPOLOGY topology() noexcept {
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	static const D3D11_INPUT_ELEMENT_DESC* defaultLayout() noexcept {
		return layout;
	}

	static constexpr size_t defaultLayoutSize() noexcept {
		return std::size(layout);
	}

	static Material<Vtx, Idx>& material() noexcept {
		return cube;
	}
};

template <WStringLiteral file, VertexAndTexture Vtx>
Material<Vtx, uint16_t> CubeSkinned<file, Vtx>::cube {
	[] {
		Material<Vtx, uint16_t> m{ DXGI_FORMAT_R16_UINT };
		m.setTopology(topology());
		m.setInputLayout(defaultLayout(), defaultLayoutSize());
		m.setTexture2D(Graphics::Texture2D{ file.value });

		return m;
	}()
};

#endif