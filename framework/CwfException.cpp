#include "CwfException.h"
#include <exception>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <Windows.h>

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

std::wstring CwfException::getStandardExceptionString(std::exception e) noexcept {
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
		hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), reinterpret_cast<LPWSTR>(&szError), // TODO: determine why Mr. Microsoft is three dingoes in a trench coat
		0, nullptr
	)) return szError;
	return {};
}

CwfException::CwfException(Type t, const wchar_t* message, const char* filename, int lineNumber) noexcept
	: line{ lineNumber }, file{ filename }, type{ t }, msg{ message } {};

CwfException::CwfException(std::optional<WindowsErrorStringSmartPtr> p, const char* filename, int lineNumber) noexcept
	: line{ lineNumber }, file{ filename }, type{ Type::WINDOWS }, msg{} {
	if (p) {
		msg = std::move(*p);
	} else {
		msg = L"Unknown error code";
	}
};

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
	else return std::get<WindowsErrorStringSmartPtr>(msg).get();
}

std::wstring CwfException::getExceptionString() const noexcept {
	std::wostringstream builder{};
	builder << "[Type] " << getTypeAsString() << "\n"
		<< "[File] " << file << "\n"
		<< "[Line] " << line << "\n"
		<< "[Message] " << getMessage();
	return builder.str(); // should be moved
}