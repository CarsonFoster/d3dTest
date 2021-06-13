#include "App.h"
#include "CwfException.h"
#include "Window.h"
#include "WindowBuilder.h"
#include "WindowClass.h"
#include <memory>

void App::doFrame() {

}

LRESULT WndProc(Window* pWindow, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(1);
		break;
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

App::App(HINSTANCE hInstance) {
	WindowClass wc{ hInstance, className };
	wc.registerClass();

	w = std::make_unique<Window>(WindowBuilder{ hInstance, className,
												windowName, WndProc }
		.addWindowStyle(WS_MINIMIZEBOX)
		.setClientSize(1000, 1000)
		.build());
}

int App::run() {
	w->showWindow();
	std::optional<int> exitCode{};
	while (true) {
		try {
			exitCode = w->processMessagesOnQueue();
			if (exitCode) return *exitCode; // if the exitCode isn't empty, return its value
			doFrame();
		}
		catch (const CwfException& e) {
			w->createExceptionMessageBox(e);
			break;
		}
		catch (const std::exception& e) {
			w->createExceptionMessageBox(e);
			break;
		}
		catch (...) {
			w->createExceptionMessageBox(CWF_EXCEPTION(CwfException::Type::OTHER, L"Unknown exception occurred."));
			break;
		}
	}
}