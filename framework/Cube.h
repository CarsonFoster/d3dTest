#ifndef CWF_CUBE_H
#define CWF_CUBE_H

#include "Graphics.h"
#include "Material.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

namespace math = DirectX;

template <class Vertex>
class Cube {
public:
	using Index = uint16_t;
private:
	static Material<Vertex, Index> cube;
	static constexpr D3D11_INPUT_ELEMENT_DESC layout[]{
		{"Position", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u}
	};
public:
	static Graphics::IndexedVertexList<Vertex, Index> mesh() {
		Graphics::IndexedVertexList<Vertex, Index> list{};
		list.vertices.emplace_back(-0.5f, 0.5f, 0.0f);
		list.vertices.emplace_back(-0.5f, -0.5f, 0.0f);
		list.vertices.emplace_back(0.5f, -0.5f, 0.0f);
		list.vertices.emplace_back(0.5f, 0.5f, 0.0f);
		list.vertices.emplace_back(-0.5f, 0.5f, 1.0f);
		list.vertices.emplace_back(-0.5f, -0.5f, 1.0f);
		list.vertices.emplace_back(0.5f, -0.5f, 1.0f);
		list.vertices.emplace_back(0.5f, 0.5f, 1.0f);

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
			{ -0.5f, 0.5f, 0.0f, 1.0f },
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{ 0.5f, -0.5f, 0.0f, 1.0f },
			{ 0.5f, 0.5f, 0.0f, 1.0f },
			{ -0.5f, 0.5f, 1.0f, 1.0f },
			{ -0.5f, -0.5f, 1.0f, 1.0f },
			{ 0.5f, -0.5f, 1.0f, 1.0f },
			{ 0.5f, 0.5f, 1.0f, 1.0f }
		};

		std::vector<math::XMFLOAT4> dst{};
		std::vector<Vertex> finalVtx{};
		dst.resize(std::size(vtx));
		finalVtx.resize(std::size(vtx));

		math::XMVector4TransformStream(
			dst.data(),
			sizeof(math::XMFLOAT4),
			vtx,
			sizeof(math::XMFLOAT4),
			std::size(vtx),
			t
		);

		for (auto& v : dst)
			finalVtx.emplace_back(v.x, v.y, v.z);

		std::vector<Index> indices {
			1,0,3,  2,1,3,
			5,4,0,  1,5,0,
			2,3,7,  6,2,7,
			6,7,4,  5,6,4,
			0,4,7,  3,0,7,
			5,1,2,  6,5,2
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

	static Material<Vertex, Index>& material() noexcept {
		return cube;
	}
};

template <class Vertex>
Material<Vertex, uint16_t> Cube<Vertex>::cube {
	[] {
		Material<Vertex, uint16_t> m{ DXGI_FORMAT_R16_UINT };
		m.setTopology(topology());
		m.setInputLayout(defaultLayout(), defaultLayoutSize());

		return m;
	}()
};

#endif