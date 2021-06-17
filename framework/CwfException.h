#ifndef CWF_CWFEXTENSION_H
#define CWF_CWFEXTENSION_H

#include <exception>
#include <optional>
#include <string>
#include <variant>
#include <Windows.h>

#define CWF_EXCEPTION(type, message) CwfException{ type, message, __FILE__, __LINE__ }
#define CWF_LAST_EXCEPTION() CwfException{ CwfException::getWindowsErrorString(HRESULT_FROM_WIN32(GetLastError())), __FILE__, __LINE__ }

class CwfException {
public:
	enum class Type {
		WINDOWS, DIRECTX, FRAMEWORK, OTHER
	};
	class WindowsErrorStringSmartPtr {
	private:
		wchar_t* ptr;
	public:
		WindowsErrorStringSmartPtr(wchar_t* p) noexcept;
		~WindowsErrorStringSmartPtr() noexcept;
		WindowsErrorStringSmartPtr(WindowsErrorStringSmartPtr&& o) noexcept;
		WindowsErrorStringSmartPtr& operator=(WindowsErrorStringSmartPtr&& o) noexcept;
		// no copy init/assign
		WindowsErrorStringSmartPtr(const WindowsErrorStringSmartPtr& o) = delete;
		WindowsErrorStringSmartPtr& operator=(const WindowsErrorStringSmartPtr& o) = delete;
		const wchar_t* get() const noexcept;
	};
private:
	int line;
	const char* file;
	Type type;
	std::variant<const wchar_t*, WindowsErrorStringSmartPtr> msg;
public:

	CwfException(Type t, const wchar_t* message, const char* filename, int lineNumber) noexcept;
	CwfException(std::optional<WindowsErrorStringSmartPtr> ptr, const char* filename, int lineNumber) noexcept;
	~CwfException() = default;

	static std::wstring getStandardExceptionString(std::exception e) noexcept;
	static std::optional<WindowsErrorStringSmartPtr> getWindowsErrorString(HRESULT hr) noexcept;

	int getLine() const noexcept;
	const char* getFile() const noexcept;
	Type getType() const noexcept;
	const wchar_t* getTypeAsString() const noexcept;
	const wchar_t* getMessage() const noexcept;
	std::wstring getExceptionString() const noexcept;
};

#endif
