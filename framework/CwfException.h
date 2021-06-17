#ifndef CWF_CWFEXTENSION_H
#define CWF_CWFEXTENSION_H

#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <Windows.h>

#define CWF_EXCEPTION(type, message) CwfException{ type, message, __FILE__, __LINE__ }
#define CWF_LAST_EXCEPTION() CwfException{ CwfException::getWindowsErrorString(HRESULT_FROM_WIN32(GetLastError())), __FILE__, __LINE__ }
#define CWF_DX_EXCEPTION(gfx, hr) CwfException{ gfx, CwfException::DirectXErrorString{ hr }, __FILE__, __LINE__ }

class Graphics;

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
	class DirectXErrorString {
	public:
		static const constexpr size_t BUFFER_SIZE = 256;
	private:
		const wchar_t* errorString;
		std::unique_ptr<wchar_t[]> errorDescription;
		bool isDeviceRemoved;
	public:
		DirectXErrorString(HRESULT hr) noexcept;
		~DirectXErrorString() = default;
		DirectXErrorString(DirectXErrorString&& o) noexcept;
		DirectXErrorString& operator=(DirectXErrorString&& o) noexcept;
		DirectXErrorString(const DirectXErrorString& o) = delete;
		DirectXErrorString& operator=(const DirectXErrorString& o) = delete;
		const wchar_t* getErrorString() const noexcept;
		const wchar_t* getErrorDescription() const noexcept;
		bool isDeviceRemovedError() const noexcept;
	};
private:
	int line;
	const char* file;
	Type type;
	std::variant<const wchar_t*, WindowsErrorStringSmartPtr, std::wstring> msg;
public:

	CwfException(Type t, const wchar_t* message, const char* filename, int lineNumber) noexcept;
	CwfException(std::optional<WindowsErrorStringSmartPtr>&& ptr, const char* filename, int lineNumber) noexcept;
	CwfException(const Graphics& gfx, const DirectXErrorString& dxErr, const char* filename, int lineNumber) noexcept;
	~CwfException() = default;

	static std::wstring getStandardExceptionString(const std::exception& e) noexcept;
	static std::optional<WindowsErrorStringSmartPtr> getWindowsErrorString(HRESULT hr) noexcept;

	int getLine() const noexcept;
	const char* getFile() const noexcept;
	Type getType() const noexcept;
	const wchar_t* getTypeAsString() const noexcept;
	const wchar_t* getMessage() const noexcept;
	std::wstring getExceptionString() const noexcept;
};

#endif
