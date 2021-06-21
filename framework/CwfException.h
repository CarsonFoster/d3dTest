#ifndef CWF_CWFEXTENSION_H
#define CWF_CWFEXTENSION_H

#include <exception>
#include <memory>
#include <string>
#include <variant>
#include <Windows.h>

#define CWF_EXCEPTION(type, message) CwfException{ type, message, __FILE__, __LINE__ }
#define CWF_LAST_EXCEPTION() CwfException{ CwfException::Type::WINDOWS, CwfException::getWindowsErrorString(HRESULT_FROM_WIN32(GetLastError())), __FILE__, __LINE__ }
#define CWF_DX_EXCEPTION(gfx, hr) CwfException{ gfx, CwfException::DirectXErrorString{ hr }, __FILE__, __LINE__ }
#define CWF_DX_EXCEPTION_NOGFX(hr) CwfException{ CwfException::DirectXErrorString{ hr }, __FILE__, __LINE__ }

class Graphics;

class CwfException {
public:
	enum class Type {
		WINDOWS, DIRECTX, FRAMEWORK, OTHER
	};
	class DirectXErrorString {
	public:
		static const constexpr size_t BUFFER_SIZE = 512;
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
	std::variant<const wchar_t*, std::wstring> msg;
public:

	CwfException(Type t, const wchar_t* message, const char* filename, int lineNumber) noexcept;
	CwfException(Type t, std::wstring&& message, const char* filename, int lineNumber) noexcept;
	CwfException(const Graphics& gfx, const DirectXErrorString& dxErr, const char* filename, int lineNumber) noexcept;
	CwfException(const DirectXErrorString& dxErr, const char* filename, int lineNumber) noexcept;
	~CwfException() = default;

	static std::wstring getStandardExceptionString(const std::exception& e) noexcept;
	static std::wstring getWindowsErrorString(HRESULT hr) noexcept;

	int getLine() const noexcept;
	const char* getFile() const noexcept;
	Type getType() const noexcept;
	const wchar_t* getTypeAsString() const noexcept;
	const wchar_t* getMessage() const noexcept;
	std::wstring getExceptionString() const noexcept;
};

#endif
