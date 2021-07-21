#ifndef CWF_SHAPECONCEPTS_H
#define CWF_SHAPECONCEPTS_H

#include <concepts>
#include <vector>

template <class T>
concept Vertex = requires (T vtx, std::vector<T> vec) {
	vtx.pos.x;
	vtx.pos.y;
	vtx.pos.z;
	T( 1.0f, 1.0f, 1.0f ); // there must be a constructor, so emplace_back will work
};

template <class T>
concept Vertex4D = Vertex<T> && requires (T vtx) {
	vtx.pos.w;
};

template <class T>
concept VertexAndTexture = Vertex<T> && requires (T vtx) {
	vtx.tex.u;
	vtx.tex.v;
	vtx.tex.set(1.0f, 1.0f);
};

#endif