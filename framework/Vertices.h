#ifndef CWF_VERTICES_H
#define CWF_VERTICES_H

namespace Vertices {
	struct Float3 {
		struct {
			float x;
			float y;
			float z;
		} pos;
		Float3(float X, float Y, float Z) {
			pos.x = X;
			pos.y = Y;
			pos.z = Z;
		}
	};

	struct Float3Tex : public Float3 {
		struct {
			float u;
			float v;
			void set(float U, float V) {
				u = U;
				v = V;
			}
		} tex;
		Float3Tex(float X, float Y, float Z) : Float3(X, Y, Z), tex{} {}
	};

	struct Float4 {
		struct {
			float x;
			float y;
			float z;
			float w;
		} pos;
		Float4(float X, float Y, float Z, float W) {
			pos.x = X;
			pos.y = Y;
			pos.z = Z;
			pos.w = W;
		}
		Float4(float X, float Y, float Z) : Float4(X, Y, Z, 1.0f) {}
	};

	struct Float4Tex : public Float4 {
		struct {
			float u;
			float v;
		} tex;
		Float4Tex(float X, float Y, float Z, float W) : Float4(X, Y, Z, W), tex{} {}
		Float4Tex(float X, float Y, float Z) : Float4(X, Y, Z), tex{} {}
	};
}

#endif