#ifndef CWF_APP_H
#define CWF_APP_H

#include "framework/ConstantBuffers.h"
#include "framework/Material.h"
#include "framework/Submaterial.h"
#include "framework/Vertices.h"
#include "framework/Window.h"
#include <cstdint>
#include <DirectXMath.h>
#include <memory>
#include <Windows.h>

// App.h and App.cpp are all supposed to be client-created code; it's just organization

namespace math = DirectX;

LRESULT WndProc(Window* pWindow, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class App {
private:
	static constexpr const LPCWSTR s_className{ L"d3dTest" };
	static constexpr const LPCWSTR s_windowName{ L"d3dTest Window" };
	std::unique_ptr<Window> m_window;
	Material<Vertices::Float3Tex, uint16_t>& m_cube;
	Submaterial<Vertices::Float3Tex, uint16_t> m_otherCube;
	ConstantBuffers::TConstBuffer m_cbuf;
public:
	App(HINSTANCE hInstance);
	~App() = default;
	// no copy init/assign
	App(const App& o) = delete;
	App& operator=(const App& o) = delete;
	int run();
private:
	void doFrame();
};

#endif