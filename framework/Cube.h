#ifndef CWF_CUBE_H
#define CWF_CUBE_H

#include "Graphics.h"
#include "Material.h"
#include "ShapeConcepts.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

namespace math = DirectX;

template <Vertex Vtx>
class Cube {
public:
	using Idx = uint16_t;
private:
	static Material<Vtx, Idx> s_cube;
	static constexpr D3D11_INPUT_ELEMENT_DESC s_layout[]{
		{"Position", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u}
	};
public:
	static Graphics::IndexedVertexList<Vtx, Idx> mesh() {
		Graphics::IndexedVertexList<Vtx, Idx> list{};
		list.vertices.emplace_back(-0.5f, 0.5f, -0.5f);
		list.vertices.emplace_back(-0.5f, -0.5f, -0.5f);
		list.vertices.emplace_back(0.5f, -0.5f, -0.5f);
		list.vertices.emplace_back(0.5f, 0.5f, -0.5f);
		list.vertices.emplace_back(-0.5f, 0.5f, 0.5f);
		list.vertices.emplace_back(-0.5f, -0.5f, 0.5f);
		list.vertices.emplace_back(0.5f, -0.5f, 0.5f);
		list.vertices.emplace_back(0.5f, 0.5f, 0.5f);

		list.indices = {
			1,0,3,  2,1,3,
			5,4,0,  1,5,0,
			2,3,7,  6,2,7,
			6,7,4,  5,6,4,
			0,4,7,  3,0,7,
			5,1,2,  6,5,2
		};
		return list;
	}

	static void XM_CALLCONV addMesh(math::FXMMATRIX t) {
		math::XMFLOAT4 vtx[]{
			{ -0.5f, 0.5f, -0.5f, 1.0f },
			{ -0.5f, -0.5f, -0.5f, 1.0f },
			{ 0.5f, -0.5f, -0.5f, 1.0f },
			{ 0.5f, 0.5f, -0.5f, 1.0f },
			{ -0.5f, 0.5f, 0.5f, 1.0f },
			{ -0.5f, -0.5f, 0.5f, 1.0f },
			{ 0.5f, -0.5f, 0.5f, 1.0f },
			{ 0.5f, 0.5f, 0.5f, 1.0f }
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

		std::vector<Idx> indices {
			1,0,3,  2,1,3,
			5,4,0,  1,5,0,
			2,3,7,  6,2,7,
			6,7,4,  5,6,4,
			0,4,7,  3,0,7,
			5,1,2,  6,5,2
		};
		s_cube.addMesh(finalVtx, indices);
	}

	static void addMesh() {
		s_cube.addMesh(mesh());
	}

	static constexpr D3D11_PRIMITIVE_TOPOLOGY topology() noexcept {
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	static const D3D11_INPUT_ELEMENT_DESC* defaultLayout() noexcept {
		return s_layout;
	}

	static constexpr size_t defaultLayoutSize() noexcept {
		return std::size(s_layout);
	}

	static Material<Vtx, Idx>& material() noexcept {
		return s_cube;
	}
};

template <Vertex Vtx>
Material<Vtx, uint16_t> Cube<Vtx>::s_cube {
	[] {
		Material<Vtx, uint16_t> m{ DXGI_FORMAT_R16_UINT };
		m.setTopology(topology());
		m.setInputLayout(defaultLayout(), defaultLayoutSize());

		return m;
	}()
};

#endif