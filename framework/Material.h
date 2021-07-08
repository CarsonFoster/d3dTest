#ifndef CWF_MATERIAL_H
#define CWF_MATERIAL_H

#include <d3d11.h>
#include <initializer_list>
#include <memory>
#include <optional>
#include <vector>

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
		bool readOnly;
	};
private:
	D3D11_PRIMITIVE_TOPOLOGY pt;
	std::shared_ptr<D3D11_INPUT_ELEMENT_DESC[]> pDescs;
	std::vector<Vertex> vtx;
	std::vector<Index> idx;
	std::vector<ConstantBuffer> cBuffs;
	Shader vs;
	std::optional<Shader> oPS;
	std::optional<ID3D11RenderTargetView*> oPrtv;
	std::optional<ID3D11DepthStencilView*> oPdsv;
	std::optional<D3D11_VIEWPORT> vp;

public:
	// do not interact with DirectX
	void setTopology(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept {
		pt = topology;
	}

	void setInputLayout(std::shared_ptr<D3D11_INPUT_ELEMENT_DESC[]> pDescriptions) noexcept {
		pDescs = pDescriptions;
	}

	void addMesh(std::initializer_list<Vertex> vertices, std::initializer_list<Index> indices) noexcept {
		vtx.insert(vtx.cend(), vertices);
		idx.insert(idx.cend(), indices);
	}

	void addConstantBuffer(const void* pBuffer, size_t byteWidth, bool readOnly = true) noexcept {
		cBuffs.emplace_back({ pBuffer, byteWidth, readOnly });
	}

	void setVertexShader(const void* pByteCode, size_t length, bool bind = true) noexcept {
		vs = { pByteCode, length, bind };
	}

	void setPixelShader(const void* pByteCode, size_t length) noexcept {								   // optional
		oPS = { pByteCode, length, true };
	}
	
	void setRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* zbuffer) noexcept { // optional
		oPrtv = renderTarget;
		oPdsv = zbuffer;
	}

	void setViewport(D3D11_VIEWPORT viewport) noexcept {                                                   // optional
		vp = viewport;
	}


	//  interact with DirectX; these signatures will change
	void setupPipeline();
	void draw();
};

#endif