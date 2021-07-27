#include "CwfException.h"
#include "Graphics.h"
#include "lib/dxerr.h"
#include <exception>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <variant>

#ifndef NDEBUG
#include <vector>
#endif

#include <Windows.h>

/* Nested class */
CwfException::DirectXErrorString::DirectXErrorString(HRESULT hr) noexcept
	: m_errorString{ DXGetErrorStringW(hr) }, m_errorDescription{ std::make_unique<wchar_t[]>(BUFFER_SIZE) }, 
	m_isDeviceRemoved{ hr == DXGI_ERROR_DEVICE_REMOVED } {
	DXGetErrorDescriptionW(hr, m_errorDescription.get(), BUFFER_SIZE);
}

CwfException::DirectXErrorString::DirectXErrorString(DirectXErrorString&& o) noexcept
	: m_errorString{ o.m_errorString }, m_errorDescription{ std::move(o.m_errorDescription) }, m_isDeviceRemoved{ o.m_isDeviceRemoved } {
	o.m_errorString = nullptr;
	o.m_isDeviceRemoved = false;
}

CwfException::DirectXErrorString& CwfException::DirectXErrorString::operator=(DirectXErrorString&& o) noexcept {
	if (&o == this) return *this;

	m_errorString = o.m_errorString;
	o.m_errorString = nullptr;
	m_errorDescription = std::move(o.m_errorDescription);
	m_isDeviceRemoved = o.m_isDeviceRemoved;
	o.m_isDeviceRemoved = false;

	return *this;
}

const wchar_t* CwfException::DirectXErrorString::getErrorString() const noexcept {
	return m_errorString;
}

const wchar_t* CwfException::DirectXErrorString::getErrorDescription() const noexcept {
	return m_errorDescription.get();
}

bool CwfException::DirectXErrorString::isDeviceRemovedError() const noexcept {
	return m_isDeviceRemoved;
}

/* Static functions */
std::wstring CwfException::getStandardExceptionString(const std::exception& e) noexcept {
	std::wostringstream builder{};
	builder << "[Type] Standard Exception\n"
		<< "[Message] " << e.what();
	return builder.str(); // should be moved
}

std::wstring CwfException::getWindowsErrorString(HRESULT hr) noexcept {
	if (FACILITY_WINDOWS == HRESULT_FACILITY(hr))
		hr = HRESULT_CODE(hr);
	wchar_t* szError{};
	if (FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
		static_cast<DWORD>(hr), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), reinterpret_cast<LPWSTR>(&szError),
		0, nullptr)
	) {
		std::wstring toReturn{ szError };
		LocalFree(szError);
		return toReturn; // should be moved or elided
	}
	return L"Unknown error code";
}

/* Constructors */
CwfException::CwfException(Type t, const wchar_t* message, const char* filename, int lineNumber) noexcept
	: m_line{ lineNumber }, m_file{ filename }, m_type{ t }, m_msg{ message } {}

CwfException::CwfException(Type t, std::wstring&& message, const char* filename, int lineNumber) noexcept
	: m_line{ lineNumber }, m_file{ filename }, m_type{ t }, m_msg{ message } {}

CwfException::CwfException(const Graphics& gfx, const DirectXErrorString& dxErr, const char* filename, int lineNumber) noexcept
	: m_line{ lineNumber }, m_file{ filename }, m_type{ Type::DIRECTX }, m_msg{} {
	std::wostringstream builder{};
	builder << dxErr.getErrorString() << ": " << dxErr.getErrorDescription();
	if (dxErr.isDeviceRemovedError()) {
		DirectXErrorString reason{ gfx.getDeviceRemovedReason() };
		builder << "\n[Reason] " << reason.getErrorDescription() << ": " << reason.getErrorDescription();
	}
#ifndef NDEBUG
	std::vector<std::wstring> messages{ gfx.info.getMessages() };
	auto end = messages.end();
	for (auto i{ messages.begin() }; i != end; i++)
		builder << "\n[Debug Msg] " << *i;
#endif
	m_msg = builder.str();
}

CwfException::CwfException(const DirectXErrorString& dxErr, const char* filename, int lineNumber) noexcept
	: m_line{ lineNumber }, m_file{ filename }, m_type{ Type::DIRECTX }, m_msg{} {
	std::wostringstream builder{};
	builder << dxErr.getErrorString() << ": " << dxErr.getErrorDescription();
	m_msg = builder.str();
}

#ifndef NDEBUG
CwfException::CwfException(const std::vector<std::wstring>& dbugInfo, const char* filename, int lineNumber) noexcept
	: m_line{ lineNumber }, m_file{ filename }, m_type{ Type::DIRECTX }, m_msg{} {
	std::wostringstream builder{};
	builder << "Debug information given below";
	auto end = dbugInfo.end();
	for (auto i = dbugInfo.begin(); i != end; i++)
		builder << "\n[Debug Msg] " << *i;
	m_msg = builder.str();
}
#endif

/* Member functions */
int CwfException::getLine() const noexcept {
	return m_line;
}

const char* CwfException::getFile() const noexcept {
	return m_file;
}

CwfException::Type CwfException::getType() const noexcept {
	return m_type;
}

const wchar_t* CwfException::getTypeAsString() const noexcept {
	switch (m_type) {
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
	if (std::holds_alternative<const wchar_t*>(m_msg)) return std::get<const wchar_t*>(m_msg);
	else return std::get<std::wstring>(m_msg).c_str();
}

std::wstring CwfException::getExceptionString() const noexcept {
	std::wostringstream builder{};
	builder << "[Type] " << getTypeAsString() << "\n"
		<< "[File] " << m_file << "\n"
		<< "[Line] " << m_line << "\n"
		<< "[Message] " << getMessage();
	return builder.str(); // should be moved
}