#ifndef CWF_SUBMATERIAL_H
#define CWF_SUBMATERIAL_H

#include <vector>

/*
* A submaterial is a class-like object, representing a type of DirectX object that uses the same shader and general info
* as a material, but requires different constant buffers. They should only exist as "children" of materials.
* You can add as many meshes as you want; they all just have to be of the same material.
*/

template <class Vertex, typename Index>
class Submaterial {
private:
	struct ConstantBuffer {
		const void* pBuffer;
		size_t length;
		bool readOnly;
	};
private:
	std::vector<Vertex> vtx;
	std::vector<Index> idx;
	std::vector<ConstantBuffer> cBuffs;

public:
	Submaterial() : vtx{}, idx{}, cBuffs{} {}

	// do not interact with DirectX
	void addMesh(std::initializer_list<Vertex> vertices, std::initializer_list<Index> indices) noexcept {
		vtx.insert(vtx.cend(), vertices);
		idx.insert(idx.cend(), indices);
	}

	void addConstantBuffer(const void* pBuffer, size_t byteWidth, bool readOnly = true) noexcept {
		cBuffs.emplace_back({ pBuffer, byteWidth, readOnly });
	}

	
	// interact with DirectX; these signatures will change
	void setupPipeline();
	void draw();
};

#endif