#ifndef CWF_APP_H
#define CWF_APP_H

#include "framework/Window.h"
#include <memory>
#include <Windows.h>
// App.h and App.cpp are all supposed to be client-created code; it's just organization

LRESULT WndProc(Window* pWindow, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class App {
private:
	static constexpr const LPCWSTR className{ L"d3dTest" };
	static constexpr const LPCWSTR windowName{ L"d3dTest Window" };
	std::unique_ptr<Window> w;
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