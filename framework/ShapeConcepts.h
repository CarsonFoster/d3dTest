#ifndef CWF_SHAPECONCEPTS_H
#define CWF_SHAPECONCEPTS_H

#include <concepts>

template <class T>
concept Vertex = requires (T vtx) {
	vtx.pos.x;
	vtx.pos.y;
	vtx.pos.z;
	T{ 1.0f, 1.0f, 1.0f };
};

template <class T>
concept Vertex4D = Vertex<T> && requires (T vtx) {
	vtx.pos.w;
};

template <class T>
concept VertexAndTexture = Vertex<T> && requires (T vtx) {
	vtx.tex.u;
	vtx.tex.v;
};

#endif