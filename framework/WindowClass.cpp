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
				pWindow->mouse.clearButtonStates();
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

WindowClass::WindowClass(HINSTANCE hInstance) noexcept : m_windowClass{}, m_registered{ false } {
	m_windowClass.cbSize = sizeof(WNDCLASSEXW);
	m_windowClass.style = CS_OWNDC;
	m_windowClass.lpfnWndProc = SetupWindowProc;
	m_windowClass.cbClsExtra = 0;
	m_windowClass.cbWndExtra = 0;
	m_windowClass.hInstance = hInstance;
	m_windowClass.hIcon = nullptr;
	m_windowClass.hCursor = nullptr;
	m_windowClass.hbrBackground = nullptr;
	m_windowClass.lpszMenuName = nullptr;
	m_windowClass.hIconSm = nullptr;
}

WindowClass::WindowClass(HINSTANCE hInstance, LPCWSTR className) noexcept : WindowClass(hInstance) {
	m_windowClass.lpszClassName = className;
}

WindowClass::~WindowClass() {
	UnregisterClassW(m_windowClass.lpszClassName, m_windowClass.hInstance);
}

void WindowClass::setStyle(UINT style) noexcept {
	m_windowClass.style = style;
}

void WindowClass::addStyle(UINT style) noexcept {
	m_windowClass.style |= style;
}

void WindowClass::setExtraClassBytes(int bytes) noexcept {
	m_windowClass.cbClsExtra = bytes;
}

void WindowClass::setExtraWindowBytes(int bytes) noexcept {
	m_windowClass.cbWndExtra = bytes;
}

void WindowClass::setIcon(HICON icon) noexcept {
	m_windowClass.hIcon = icon;
}

void WindowClass::setCursor(HCURSOR cursor) noexcept {
	m_windowClass.hCursor = cursor;
}

void WindowClass::setBackgroundBrush(HBRUSH brush) noexcept {
	m_windowClass.hbrBackground = brush;
}

void WindowClass::setMenuName(LPCWSTR menu) noexcept {
	m_windowClass.lpszMenuName = menu;
}

void WindowClass::setIconSmall(HICON icon) noexcept {
	m_windowClass.hIconSm = icon;
}

bool WindowClass::isRegistered() const noexcept {
	return m_registered;
}

LPCWSTR WindowClass::getClassName() const noexcept {
	return m_windowClass.lpszClassName;
}

LPCWSTR WindowClass::getMenuName() const noexcept {
	return m_windowClass.lpszMenuName;
}

bool WindowClass::registerClass() {
	if (m_registered) return false;
	RegisterClassExW(&m_windowClass);
	m_registered = true;
	return true;
}