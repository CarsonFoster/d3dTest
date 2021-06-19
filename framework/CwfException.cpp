#include "CwfException.h"
#include "Graphics.h"
#include "lib/dxerr.h"
#include <exception>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <Windows.h>

/* Nested classes */
CwfException::WindowsErrorStringSmartPtr::WindowsErrorStringSmartPtr(wchar_t* p) noexcept : ptr{ p } {}
CwfException::WindowsErrorStringSmartPtr::~WindowsErrorStringSmartPtr() noexcept {
	if (ptr) LocalFree(ptr);
	ptr = nullptr;
}
CwfException::WindowsErrorStringSmartPtr::WindowsErrorStringSmartPtr(WindowsErrorStringSmartPtr&& o) noexcept : ptr{ o.ptr } {
	o.ptr = nullptr;
}
CwfException::WindowsErrorStringSmartPtr& CwfException::WindowsErrorStringSmartPtr::operator=(WindowsErrorStringSmartPtr&& o) noexcept {
	if (&o == this) return *this;
	ptr = o.ptr;
	o.ptr = nullptr;
	return *this;
}
const wchar_t* CwfException::WindowsErrorStringSmartPtr::get() const noexcept {
	return ptr;
}

CwfException::DirectXErrorString::DirectXErrorString(HRESULT hr) noexcept
	: errorString{ DXGetErrorStringW(hr) }, errorDescription{ std::make_unique<wchar_t[]>(BUFFER_SIZE) }, 
	isDeviceRemoved{ hr == DXGI_ERROR_DEVICE_REMOVED } {
	DXGetErrorDescriptionW(hr, errorDescription.get(), BUFFER_SIZE);
}

CwfException::DirectXErrorString::DirectXErrorString(DirectXErrorString&& o) noexcept
	: errorString{ o.errorString }, errorDescription{ std::move(o.errorDescription) }, isDeviceRemoved{ o.isDeviceRemoved } {
	o.errorString = nullptr;
	o.isDeviceRemoved = false;
}

CwfException::DirectXErrorString& CwfException::DirectXErrorString::operator=(DirectXErrorString&& o) noexcept {
	if (&o == this) return *this;

	errorString = o.errorString;
	o.errorString = nullptr;
	errorDescription = std::move(o.errorDescription);
	isDeviceRemoved = o.isDeviceRemoved;
	o.isDeviceRemoved = false;

	return *this;
}

const wchar_t* CwfException::DirectXErrorString::getErrorString() const noexcept {
	return errorString;
}

const wchar_t* CwfException::DirectXErrorString::getErrorDescription() const noexcept {
	return errorDescription.get();
}

bool CwfException::DirectXErrorString::isDeviceRemovedError() const noexcept {
	return isDeviceRemoved;
}

/* Static functions */
std::wstring CwfException::getStandardExceptionString(const std::exception& e) noexcept {
	std::wostringstream builder{};
	builder << "[Type] Standard Exception\n"
		<< "[Message] " << e.what();
	return builder.str(); // should be moved
}

std::optional<CwfException::WindowsErrorStringSmartPtr> CwfException::getWindowsErrorString(HRESULT hr) noexcept {
	if (FACILITY_WINDOWS == HRESULT_FACILITY(hr))
		hr = HRESULT_CODE(hr);
	wchar_t* szError{};
	if (FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
		hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), reinterpret_cast<LPWSTR>(&szError),
		0, nullptr
	)) return szError;
	return {};
}

/* Constructors */
CwfException::CwfException(Type t, const wchar_t* message, const char* filename, int lineNumber) noexcept
	: line{ lineNumber }, file{ filename }, type{ t }, msg{ message } {};

CwfException::CwfException(std::optional<WindowsErrorStringSmartPtr>&& p, const char* filename, int lineNumber) noexcept
	: line{ lineNumber }, file{ filename }, type{ Type::WINDOWS }, msg{} {
	if (p) {
		msg = std::move(*p);
	} else {
		msg = L"Unknown error code";
	}
};

CwfException::CwfException(const Graphics& gfx, const DirectXErrorString& dxErr, const char* filename, int lineNumber) noexcept
	: line{ lineNumber }, file{ filename }, type{ Type::DIRECTX }, msg{} {
	std::wostringstream builder{};
	builder << dxErr.getErrorString() << ": " << dxErr.getErrorDescription();
	if (dxErr.isDeviceRemovedError()) {
		DirectXErrorString reason{ gfx.getDeviceRemovedReason() };
		builder << "\n[Reason] " << reason.getErrorDescription() << ": " << reason.getErrorDescription();
	}
	msg = builder.str();
}

/* Member functions */
int CwfException::getLine() const noexcept {
	return line;
}

const char* CwfException::getFile() const noexcept {
	return file;
}

CwfException::Type CwfException::getType() const noexcept {
	return type;
}

const wchar_t* CwfException::getTypeAsString() const noexcept {
	switch (type) {
	case Type::WINDOWS:
		return L"Windows Failure";
	case Type::DIRECTX:
		return L"DirectX Failure";
	case Type::FRAMEWORK:
		return L"Framework Exception";
	case Type::OTHER:
		return L"Other Exception";
	default:
		return L"Unknown"; // means I forgot to update the switch
	}
}

const wchar_t* CwfException::getMessage() const noexcept {
	if (std::holds_alternative<const wchar_t*>(msg)) return std::get<const wchar_t*>(msg);
	else if (std::holds_alternative<WindowsErrorStringSmartPtr>(msg)) return std::get<WindowsErrorStringSmartPtr>(msg).get();
	else return std::get<std::wstring>(msg).c_str();
}

std::wstring CwfException::getExceptionString() const noexcept {
	std::wostringstream builder{};
	builder << "[Type] " << getTypeAsString() << "\n"
		<< "[File] " << file << "\n"
		<< "[Line] " << line << "\n"
		<< "[Message] " << getMessage();
	return builder.str(); // should be moved
}