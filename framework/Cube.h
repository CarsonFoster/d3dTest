#ifndef CWF_CUBE_H
#define CWF_CUBE_H

#include "Graphics.h"
#include <d3d11.h>
#include <memory>

class Cube {
private:
	static constexpr D3D11_INPUT_ELEMENT_DESC layout[]{
		{"Position", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u}
	};
public:
	template <class Vertex, typename Index>
	static constexpr Graphics::IndexedVertexList<Vertex, Index> mesh() {
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

	static constexpr D3D11_PRIMITIVE_TOPOLOGY topology() {
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	static const D3D11_INPUT_ELEMENT_DESC* defaultLayout() {
		return layout;
	}

	static constexpr size_t defaultLayoutSize() {
		return std::size(layout);
	}
};

#endif