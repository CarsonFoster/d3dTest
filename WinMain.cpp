#include "Window.h"
#include "WindowClass.h"
#include "WindowBuilder.h"
#include <optional>
#include <Windows.h>

LRESULT WndProc(Window* pWindow, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(1);
		break;
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd
) {
	const auto pClassName = L"d3dTest";
	const auto pWindowName = L"d3dTest Window";

	/* 
	* So, if the Window creation doesn't fail, I want the exceptions to go to Window::createExceptionMessageBox,
	* so that the Window will own the created message box. However, if the Window creation fails, there is no Window to
	* own the message box, so the exceptions go to Window::createExceptionMessageBoxStatic.
	*/

	try {
		// Register class
		WindowClass wc{ hInstance, pClassName };
		wc.registerClass();

		// Create window instance
		Window w{
			WindowBuilder{ hInstance, wc.getClassName(), pWindowName, WndProc }
			.addWindowStyle(WS_MINIMIZEBOX)
			.setClientSize(960, 540)
			.build()
		};

		// Show window
		w.showWindow();

		std::optional<int> exitCode{};
		while (true) {
			try {
				exitCode = w.processMessagesOnQueue();
			}
			catch (const CwfException& e) {
				w.createExceptionMessageBox(e);
				break;
			}
			catch (const std::exception& e) {
				w.createExceptionMessageBox(e);
				break;
			}
			catch (...) {
				w.createExceptionMessageBox(CWF_EXCEPTION(CwfException::CwfExceptionType::OTHER, L"Unknown exception occurred."));
				break;
			}
			if (exitCode) return *exitCode; // if the exitCode isn't empty, return its value
		}
	} catch (const CwfException& e) {
		Window::createExceptionMessageBoxStatic(e);
	} catch (const std::exception& e) {
		Window::createExceptionMessageBoxStatic(e);
	} catch (...) {
		Window::createExceptionMessageBoxStatic(CWF_EXCEPTION(CwfException::CwfExceptionType::OTHER, L"Unknown exception occurred."));
	}

	return 0;
}