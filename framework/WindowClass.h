#ifndef CWF_WINDOWCLASS_H
#define CWF_WINDOWCLASS_H

#include <Windows.h>
#include "Window.h"

class WindowClass {
private:
	WNDCLASSEXW wc;
	bool registered;
	LRESULT (*clientWindowProc)(Window*, HWND, UINT, WPARAM, LPARAM);
	WindowClass(HINSTANCE hInstance) noexcept;
	static LRESULT CALLBACK SetupWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	WindowClass(HINSTANCE hInstance, LPCWSTR className) noexcept;
	~WindowClass();
	// no copy init/assignment
	WindowClass(const WindowClass& o) = delete;
	WindowClass& operator=(const WindowClass& o) = delete;

	void setStyle(UINT style) noexcept;
	void addStyle(UINT style) noexcept;
	void setExtraClassBytes(int bytes) noexcept;
	void setExtraWindowBytes(int bytes) noexcept;
	void setIcon(HICON icon) noexcept;
	void setCursor(HCURSOR cursor) noexcept;
	void setBackgroundBrush(HBRUSH brush) noexcept;
	void setMenuName(LPCWSTR menu) noexcept;
	void setIconSmall(HICON icon) noexcept;

	bool isRegistered() const noexcept;
	LPCWSTR getClassName() const noexcept;
	LPCWSTR getMenuName() const noexcept;

	bool registerClass();
};

#endif
