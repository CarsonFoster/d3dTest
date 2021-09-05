#include "CwfException.h"
#include "Graphics.h"
#include "Window.h"
#include <cstddef>
#include <exception>
#include <memory>
#include <optional>
#include <ShellScalingApi.h>
#include <string>
#include <Windows.h>
#include <windowsx.h>

// Nested classes
Window::SmartHWND::SmartHWND() noexcept : m_hWnd{ nullptr } {}

Window::SmartHWND::SmartHWND(HWND h) noexcept : m_hWnd{ h } {}

Window::SmartHWND::SmartHWND(SmartHWND&& o) noexcept : m_hWnd{ o.m_hWnd } {
	o.m_hWnd = nullptr;
}

Window::SmartHWND& Window::SmartHWND::operator=(SmartHWND&& o) noexcept {
	if (&o == this) return *this;
	m_hWnd = o.m_hWnd;
	o.m_hWnd = nullptr;
	return *this;
}

Window::SmartHWND::~SmartHWND() noexcept {
	SetWindowLongPtrW(m_hWnd, GWLP_USERDATA, 0); // clear out old window pointer
	DestroyWindow(m_hWnd);
}

HWND Window::SmartHWND::get() const noexcept {
	return m_hWnd;
}

Window::SmartHWND::operator bool() const noexcept {
	return (m_hWnd != nullptr);
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
	: m_hWnd{}, m_clientWindowProc{ wis.windowProc }, 
	m_clientWidth{ wis.clientWidth }, m_clientHeight{ wis.clientHeight }, kbd{}, mouse{} {
	
	// SetProcessDpiAwareness(PROCESS_DPI_AWARENESS::PROCESS_SYSTEM_DPI_AWARE);
	SetProcessDPIAware();

	HWND hWnd = CreateWindowExW(wis.extendedStyle, wis.className, wis.windowName, wis.windowStyle, wis.x, wis.y,
		wis.windowWidth, wis.windowHeight, wis.hParent, wis.hMenu, wis.hInstance, this);
	if (hWnd == nullptr) throw CWF_LAST_EXCEPTION();
	m_hWnd = hWnd;
	OutputDebugStringA((std::to_string(wis.windowWidth) + " " + std::to_string(wis.windowHeight) + "\n").c_str());
	RECT rect{};
	if (GetWindowRect(hWnd, &rect)) {
		OutputDebugStringA((std::to_string(rect.right - rect.left) + " " + std::to_string(rect.bottom - rect.top) + "\n").c_str());
	}
	if (GetClientRect(hWnd, &rect)) {
		OutputDebugStringA((std::to_string(rect.right - rect.left) + " " + std::to_string(rect.bottom - rect.top) + "\n").c_str());
	}
	m_graphics = std::make_unique<Graphics>(hWnd, m_clientWidth, m_clientHeight);
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
	if (!m_graphics) throw CWF_EXCEPTION(CwfException::Type::FRAMEWORK, L"No graphics object found.");
	return *m_graphics;
}

int Window::getClientWidth() const noexcept {
	return m_clientWidth;
}

int Window::getClientHeight() const noexcept {
	return m_clientHeight;
}

Window::ClientWindowProc Window::getClientWindowProc() const noexcept {
	return m_clientWindowProc;
}

void Window::showWindow(int showCommand) {
	ShowWindow(m_hWnd.get(), showCommand);
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
		{
			int x = GET_X_LPARAM(msg.lParam);
			int y = GET_Y_LPARAM(msg.lParam);
			if (x >= 0 && x < m_clientWidth && y >= 0 && y < m_clientHeight) {
				mouse.moved(x, y);
				if (!mouse.isInClientRegion()) { // in client region now, but not previously -> enter message and capture
					SetCapture(m_hWnd.get());
					mouse.entered(x, y);
				}
			}
			else {
				if (mouse.isLeftPressed() || mouse.isMiddlePressed() || mouse.isRightPressed()) {
					// if any button is pressed, we're still capturing the mouse
					mouse.moved(x, y);
				}
				else { // no button pressed, release capture and leave
					ReleaseCapture();
					mouse.left(x, y);
				}
			}
			break;
		}

		// raw input
		case WM_INPUT:
			HRAWINPUT hRawInput{ reinterpret_cast<HRAWINPUT>(msg.lParam) };
			UINT size;
			
			// read size of data and allocate buffer
			GetRawInputData(hRawInput, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
			if (size == 0) break;
			std::unique_ptr<std::byte[]> data{ std::make_unique<std::byte[]>(size) };

			// get data
			if (GetRawInputData(hRawInput, RID_INPUT, data.get(), &size, sizeof(RAWINPUTHEADER)) != size)
				break; // bail if read wrong amount of info

			RAWINPUT* raw{ reinterpret_cast<RAWINPUT*>(data.get()) };
			if (raw->header.dwType == RIM_TYPEMOUSE
				// there must be some movement
				&& (raw->data.mouse.lLastX != 0 || raw->data.mouse.lLastY != 0)
				// if these are absolute (not a delta), ditch the data
				&& !(raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)) {

				mouse.raw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
			}

			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

void Window::createExceptionMessageBox(const CwfException& e) {
	MessageBoxW(m_hWnd.get(), e.getExceptionString().c_str(), s_exceptionCaption, MB_ICONERROR);
}

void Window::createExceptionMessageBox(const std::exception& e) {
	MessageBoxW(m_hWnd.get(), CwfException::getStandardExceptionString(e).c_str(), s_exceptionCaption, MB_ICONERROR);
}

HWND Window::getHWND() const noexcept {
	return m_hWnd.get();
}

bool Window::setTitle(LPCWSTR title) noexcept {
	return SetWindowTextW(m_hWnd.get(), title);
}

void Window::createExceptionMessageBoxStatic(const CwfException& e) {
	MessageBoxW(nullptr, e.getExceptionString().c_str(), s_exceptionCaption, MB_ICONERROR);
}

void Window::createExceptionMessageBoxStatic(const std::exception& e) {
	MessageBoxW(nullptr, CwfException::getStandardExceptionString(e).c_str(), s_exceptionCaption, MB_ICONERROR);
}