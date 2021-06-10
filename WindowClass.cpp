#include "Window.h"
#include "WindowClass.h"
#include <Windows.h>

LRESULT CALLBACK WindowClass::SetupWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_NCCREATE) {
		CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWindow = static_cast<Window*>(pCreate->lpCreateParams); // grab pointer from CreateWindowExW
		
		SetLastError(0); // have to clear last error to get accurate checking for SetWindowLongPtrW
		if (!SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow))) { // if return is 0
			if (GetLastError()) { // if return is nonzero
				PostMessageW(hWnd, WM_THROW, __LINE__, reinterpret_cast<LONG_PTR>(__FILE__)); // alert main thread
			} else { // no error otherwise
				return pWindow->getClientWindowProc()(pWindow, hWnd, msg, wParam, lParam);
			}
		}
	} else {
		Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
		if (pWindow) {
			if (msg == WM_KILLFOCUS) { // WM_KILLFOCUS bypasses message queue, can only handle it here
				pWindow->kbd.clearKeyStates();
			}
			Window::ClientWindowProc windowProc = pWindow->getClientWindowProc();
			if (windowProc) {
				return windowProc(pWindow, hWnd, msg, wParam, lParam);
			} else {
				PostMessageW(hWnd, WM_THROW, __LINE__, reinterpret_cast<LONG_PTR>(__FILE__)); // alert main thread
			}
		}/* else {
			DWORD errorCode{ GetLastError() };
			PostMessageW(hWnd, WM_THROW, __LINE__, reinterpret_cast<LONG_PTR>(__FILE__)); // alert main thread
		}*/
		// TODO: determine if I really need the above block of code or not
	}

}

WindowClass::WindowClass(HINSTANCE hInstance) noexcept : wc{}, registered{ false } {
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = SetupWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.hIconSm = nullptr;
}

WindowClass::WindowClass(HINSTANCE hInstance, LPCWSTR className) noexcept : WindowClass(hInstance) {
	wc.lpszClassName = className;
}

WindowClass::~WindowClass() {
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

void WindowClass::setStyle(UINT style) noexcept {
	wc.style = style;
}

void WindowClass::addStyle(UINT style) noexcept {
	wc.style |= style;
}

void WindowClass::setExtraClassBytes(int bytes) noexcept {
	wc.cbClsExtra = bytes;
}

void WindowClass::setExtraWindowBytes(int bytes) noexcept {
	wc.cbWndExtra = bytes;
}

void WindowClass::setIcon(HICON icon) noexcept {
	wc.hIcon = icon;
}

void WindowClass::setCursor(HCURSOR cursor) noexcept {
	wc.hCursor = cursor;
}

void WindowClass::setBackgroundBrush(HBRUSH brush) noexcept {
	wc.hbrBackground = brush;
}

void WindowClass::setMenuName(LPCWSTR menu) noexcept {
	wc.lpszMenuName = menu;
}

void WindowClass::setIconSmall(HICON icon) noexcept {
	wc.hIconSm = icon;
}

bool WindowClass::isRegistered() const noexcept {
	return registered;
}

LPCWSTR WindowClass::getClassName() const noexcept {
	return wc.lpszClassName;
}

LPCWSTR WindowClass::getMenuName() const noexcept {
	return wc.lpszMenuName;
}

bool WindowClass::registerClass() {
	if (registered) return false;
	RegisterClassExW(&wc);
	registered = true;
	return true;
}