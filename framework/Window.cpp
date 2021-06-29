#include "CwfException.h"
#include "Graphics.h"
#include "Window.h"
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <Windows.h>
#include <windowsx.h>

// Nested classes
Window::SmartHWND::SmartHWND() noexcept : hWnd{ nullptr } {}

Window::SmartHWND::SmartHWND(HWND h) noexcept : hWnd{ h } {}

Window::SmartHWND::SmartHWND(SmartHWND&& o) noexcept : hWnd{ o.hWnd } {
	o.hWnd = nullptr;
}

Window::SmartHWND& Window::SmartHWND::operator=(SmartHWND&& o) noexcept {
	if (&o == this) return *this;
	hWnd = o.hWnd;
	o.hWnd = nullptr;
	return *this;
}

Window::SmartHWND::~SmartHWND() noexcept {
	SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0); // clear out old window pointer
	DestroyWindow(hWnd);
}

HWND Window::SmartHWND::get() const noexcept {
	return hWnd;
}

Window::SmartHWND::operator bool() const noexcept {
	return (hWnd != nullptr);
}

Window::WindowInitializationStruct::WindowInitializationStruct(DWORD eStyle, LPCWSTR aClassName, LPCWSTR aWindowName,
	DWORD style, int aX, int aY, int width, int height, int cWidth, int cHeight, HWND parent, HMENU menu,
	HINSTANCE instance, ClientWindowProc aWindowProc) noexcept
	: extendedStyle{ eStyle }, className{ aClassName }, windowName{ aWindowName }, windowStyle{ style },
	x{ aX }, y{ aY }, windowWidth{ width }, windowHeight{ height }, clientWidth{ cWidth }, clientHeight{ cHeight },
	hParent{ parent }, hMenu{ menu }, hInstance{ instance }, windowProc{ aWindowProc } {}

Window::WindowInitializationStruct::WindowInitializationStruct(WindowInitializationStruct&& o) noexcept {
	extendedStyle = o.extendedStyle;
	o.extendedStyle = 0;
	className = o.className;
	o.className = nullptr;
	windowName = o.windowName;
	o.windowName = nullptr;
	windowStyle = o.windowStyle;
	o.windowStyle = 0;
	x = o.x;
	o.x = 0;
	y = o.y;
	o.y = 0;
	windowWidth = o.windowWidth;
	o.windowWidth = 0;
	windowHeight = o.windowHeight;
	o.windowHeight = 0;
	clientWidth = o.clientWidth;
	o.clientWidth = 0;
	clientHeight = o.clientHeight;
	o.clientHeight = 0;
	hParent = o.hParent;
	o.hParent = nullptr;
	hMenu = o.hMenu;
	o.hMenu = nullptr;
	hInstance = o.hInstance;
	o.hInstance = nullptr;
	windowProc = o.windowProc;
	o.windowProc = nullptr;
}
Window::WindowInitializationStruct& Window::WindowInitializationStruct::operator=(WindowInitializationStruct&& o) noexcept {
	if (&o == this) return *this;
	extendedStyle = o.extendedStyle;
	o.extendedStyle = 0;
	className = o.className;
	o.className = nullptr;
	windowName = o.windowName;
	o.windowName = nullptr;
	windowStyle = o.windowStyle;
	o.windowStyle = 0;
	x = o.x;
	o.x = 0;
	y = o.y;
	o.y = 0;
	windowWidth = o.windowWidth;
	o.windowWidth = 0;
	windowHeight = o.windowHeight;
	o.windowHeight = 0;
	clientWidth = o.clientWidth;
	o.clientWidth = 0;
	clientHeight = o.clientHeight;
	o.clientHeight = 0;
	hParent = o.hParent;
	o.hParent = nullptr;
	hMenu = o.hMenu;
	o.hMenu = nullptr;
	hInstance = o.hInstance;
	o.hInstance = nullptr;
	windowProc = o.windowProc;
	o.windowProc = nullptr;
	return *this;
}

// Constructor and Destructor
Window::Window(Window::WindowInitializationStruct wis)
	: shWnd{}, clientWindowProc{ wis.windowProc }, 
	clientWidth{ wis.clientWidth }, clientHeight{ wis.clientHeight }, kbd{}, mouse{} {
	
	HWND hWnd = CreateWindowExW(wis.extendedStyle, wis.className, wis.windowName, wis.windowStyle, wis.x, wis.y,
		wis.windowWidth, wis.windowHeight, wis.hParent, wis.hMenu, wis.hInstance, this);
	if (hWnd == nullptr) throw CWF_LAST_EXCEPTION();
	shWnd = hWnd;
	graphics = std::make_unique<Graphics>(hWnd, clientWidth, clientHeight);
}

/*Window::Window(Window&& o) noexcept : hWnd{o.hWnd}, clientWindowProc{o.clientWindowProc} {
	o.hWnd = nullptr;
	o.clientWindowProc = nullptr;
}

Window& Window::operator=(Window&& o) noexcept {
	if (&o == this) return *this;
	// no need for deletion or anything yet
	hWnd = o.hWnd;
	clientWindowProc = o.clientWindowProc;

	o.hWnd = nullptr;
	o.clientWindowProc = nullptr;
	return *this;
}*/

// Member functions
Graphics& Window::gfx() const {
	if (!graphics) throw CWF_EXCEPTION(CwfException::Type::FRAMEWORK, L"No graphics object found.");
	return *graphics;
}

int Window::getClientWidth() const noexcept {
	return clientWidth;
}

int Window::getClientHeight() const noexcept {
	return clientHeight;
}

Window::ClientWindowProc Window::getClientWindowProc() const noexcept {
	return clientWindowProc;
}

void Window::showWindow(int showCommand) {
	ShowWindow(shWnd.get(), showCommand);
}

std::optional<int> Window::processMessagesOnQueue() {
	MSG msg;
	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) { // uses nullptr here to receive all messages from current thread
		switch (msg.message) {
		// other messages
		case WM_QUIT:
			return msg.wParam; // return exit code
		case WM_THROW:
			throw CwfException{ CwfException::Type::WINDOWS, L"Windows failure in window proc callback function.",
				reinterpret_cast<const char*>(msg.lParam), static_cast<int>(msg.wParam) };
		
		// keyboard messages
		case WM_SYSKEYDOWN: // for F10 and Alt; we do the same thing as regular though
			[[fallthrough]];
		case WM_KEYDOWN:
			if (!(msg.lParam & 0x40000000) // the previous key state is 0
				|| kbd.isAutorepeatEnabled()) { // or autorepeat is enabled
				
				kbd.keyPressed(static_cast<unsigned char>(msg.wParam));
			}
			break;
		case WM_SYSKEYUP:
			[[fallthrough]];
		case WM_KEYUP:
			kbd.keyReleased(static_cast<unsigned char>(msg.wParam));
			break;
		case WM_CHAR:
			kbd.characterTyped(static_cast<unsigned char>(msg.wParam));	
			break;

		// mouse messages
		case WM_LBUTTONDOWN:
			mouse.buttonPressed(Mouse::Event::Button::LEFT, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_MBUTTONDOWN:
			mouse.buttonPressed(Mouse::Event::Button::MIDDLE, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_RBUTTONDOWN:
			mouse.buttonPressed(Mouse::Event::Button::RIGHT, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_LBUTTONUP:
			mouse.buttonReleased(Mouse::Event::Button::LEFT, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_MBUTTONUP:
			mouse.buttonReleased(Mouse::Event::Button::MIDDLE, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_RBUTTONUP:
			mouse.buttonReleased(Mouse::Event::Button::RIGHT, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_LBUTTONDBLCLK:
			mouse.buttonDoubleClicked(Mouse::Event::Button::LEFT, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_MBUTTONDBLCLK:
			mouse.buttonDoubleClicked(Mouse::Event::Button::MIDDLE, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_RBUTTONDBLCLK:
			mouse.buttonDoubleClicked(Mouse::Event::Button::RIGHT, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_MOUSEWHEEL:
			mouse.scrolled(msg.wParam, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_MOUSEMOVE:
			int x = GET_X_LPARAM(msg.lParam);
			int y = GET_Y_LPARAM(msg.lParam);
			if (x >= 0 && x < clientWidth && y >= 0 && y < clientHeight) {
				mouse.moved(x, y);
				if (!mouse.isInClientRegion()) { // in client region now, but not previously -> enter message and capture
					SetCapture(shWnd.get());
					mouse.entered(x, y);
				}
			} else {
				if (mouse.isLeftPressed() || mouse.isMiddlePressed() || mouse.isRightPressed()) {
					// if any button is pressed, we're still capturing the mouse
					mouse.moved(x, y);
				} else { // no button pressed, release capture and leave
					ReleaseCapture();
					mouse.left(x, y);
				}
			}
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

void Window::createExceptionMessageBox(const CwfException& e) {
	MessageBoxW(shWnd.get(), e.getExceptionString().c_str(), exceptionCaption, MB_ICONERROR);
}

void Window::createExceptionMessageBox(const std::exception& e) {
	MessageBoxW(shWnd.get(), CwfException::getStandardExceptionString(e).c_str(), exceptionCaption, MB_ICONERROR);
}

HWND Window::getHWND() const noexcept {
	return shWnd.get();
}

bool Window::setTitle(LPCWSTR title) noexcept {
	return SetWindowTextW(shWnd.get(), title);
}

void Window::createExceptionMessageBoxStatic(const CwfException& e) {
	MessageBoxW(nullptr, e.getExceptionString().c_str(), exceptionCaption, MB_ICONERROR);
}

void Window::createExceptionMessageBoxStatic(const std::exception& e) {
	MessageBoxW(nullptr, CwfException::getStandardExceptionString(e).c_str(), exceptionCaption, MB_ICONERROR);
}