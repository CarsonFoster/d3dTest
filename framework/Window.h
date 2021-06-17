#ifndef CWF_WINDOW_H
#define CWF_WINDOW_H

#include "CwfException.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <exception>
#include <memory>
#include <optional>
#include <Windows.h>

#define WM_THROW (WM_USER + 0)

class WindowBuilder;

class Window {
public:
	using ClientWindowProc = LRESULT(*)(Window*, HWND, UINT, WPARAM, LPARAM);
private:
	static constexpr const wchar_t* exceptionCaption = L"Exception in Program";
	HWND hWnd;
	ClientWindowProc clientWindowProc;
	int clientWidth;
	int clientHeight;
	std::unique_ptr<Graphics> graphics;

	class WindowInitializationStruct {
	public:
		DWORD extendedStyle;
		LPCWSTR className;
		LPCWSTR windowName;
		DWORD windowStyle;
		int x;
		int y;
		int windowWidth;
		int windowHeight;
		int clientWidth;
		int clientHeight;
		HWND hParent;
		HMENU hMenu;
		HINSTANCE hInstance;
		ClientWindowProc windowProc;

		WindowInitializationStruct(DWORD eStyle, LPCWSTR aClassName, LPCWSTR aWindowName,
			DWORD style, int aX, int aY, int width, int height, int cWidth, int cHeight, HWND parent, HMENU menu,
			HINSTANCE instance, ClientWindowProc aWindowProc) noexcept;
		~WindowInitializationStruct() = default;
		WindowInitializationStruct(WindowInitializationStruct&& o) noexcept;
		WindowInitializationStruct& operator=(WindowInitializationStruct&& o) noexcept;

		WindowInitializationStruct(const WindowInitializationStruct& o) = delete;
		WindowInitializationStruct& operator=(const WindowInitializationStruct& o) = delete;
	};
public:
	Keyboard kbd;
	Mouse mouse;

	Window(WindowInitializationStruct wis);
	// NOTE: Remember to keep move semantics up to date as more stuff is added to 
	// NOTE: move semantics are currently disabled, as the pointer to the window in GetWindowLongPtrW() is invalidated
	// 	     when the window is moved
	// Window(Window&& o) noexcept;
	// Window& operator=(Window&& o) noexcept;
	// no copy init/assign
	~Window() noexcept;
	Window(const Window& o) = delete;
	Window& operator=(const Window& o) = delete;

	Graphics& gfx() const;
	int getClientWidth() const noexcept;
	int getClientHeight() const noexcept;
	std::optional<int> processMessagesOnQueue();
	void showWindow(int showCommand = SW_SHOW);
	void createExceptionMessageBox(const CwfException& e);
	void createExceptionMessageBox(const std::exception& e);
	ClientWindowProc getClientWindowProc() const noexcept;
	bool setTitle(LPCWSTR title) noexcept;
	HWND getHWND() const noexcept;
	
	static void createExceptionMessageBoxStatic(const CwfException& e);
	static void createExceptionMessageBoxStatic(const std::exception& e);

	friend class WindowBuilder;
};

#endif