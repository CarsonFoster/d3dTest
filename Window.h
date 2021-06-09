#ifndef CWF_WINDOW_H
#define CWF_WINDOW_H

#include "CwfException.h"
#include <exception>
#include <optional>
#include <Windows.h>

class WindowFactory;

class Window {
public:
	using ClientWindowProc = LRESULT(*)(Window*, HWND, UINT, WPARAM, LPARAM);
private:
	static constexpr const wchar_t* exceptionCaption = L"Exception in Program";
	HWND hWnd;
	ClientWindowProc clientWindowProc;

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
		HWND hParent;
		HMENU hMenu;
		HINSTANCE hInstance;
		ClientWindowProc windowProc;

		WindowInitializationStruct(DWORD eStyle, LPCWSTR aClassName, LPCWSTR aWindowName,
			DWORD style, int aX, int aY, int width, int height, HWND parent, HMENU menu,
			HINSTANCE instance, ClientWindowProc aWindowProc) noexcept;
		~WindowInitializationStruct() = default;
		WindowInitializationStruct(WindowInitializationStruct&& o) noexcept;
		WindowInitializationStruct& operator=(WindowInitializationStruct&& o) noexcept;

		WindowInitializationStruct(const WindowInitializationStruct& o) = delete;
		WindowInitializationStruct& operator=(const WindowInitializationStruct& o) = delete;
	};
public:
	// NOTE: Remember to keep move semantics up to date as more stuff is added to 
	// NOTE: move semantics are currently disabled, as the pointer to the window in GetWindowLongPtrW() is invalidated
	// 	     when the window is moved
	// Window(Window&& o) noexcept;
	// Window& operator=(Window&& o) noexcept;
	Window(WindowInitializationStruct wis);
	// no copy init/assign
	Window(const Window& o) = delete;
	Window& operator=(const Window& o) = delete;

	static std::optional<int> processMessagesOnQueue();

	ClientWindowProc getClientWindowProc() const noexcept;
	void showWindow(int showCommand = SW_SHOW);
	void createExceptionMessageBox(CwfException e);
	void createExceptionMessageBox(std::exception e);

	friend class WindowFactory;
};

#endif