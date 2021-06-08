#include "CwfException.h"
#include <sstream>

CwfException::CwfException(CwfExceptionType t, const wchar_t* message, const char* filename, int lineNumber)
	: line{ lineNumber }, file{ filename }, type{ t }, msg{ message } {};

int CwfException::getLine() const noexcept {
	return line;
}

const char* CwfException::getFile() const noexcept {
	return file;
}

CwfException::CwfExceptionType CwfException::getType() const noexcept {
	return type;
}

const wchar_t* CwfException::getTypeAsString() const noexcept {
	switch (type) {
	case CwfExceptionType::WINDOWS:
		return L"Windows Failure";
	case CwfExceptionType::DIRECTX:
		return L"DirectX Failure";
	case CwfExceptionType::FRAMEWORK:
		return L"Framework Exception";
	case CwfExceptionType::OTHER:
		return L"Other Exception";
	default:
		return L"Unknown"; // means I forgot to update the switch
	}
}

const wchar_t* CwfException::getMessage() const noexcept {
	return msg;
}

const wchar_t* CwfException::getExceptionString() const noexcept {
	std::wostringstream builder{};
	builder << "[Type] " << getTypeAsString() << "\n"
		<< "[File] " << file << "\n"
		<< "[Line] " << line << "\n"
		<< "[Message] " << msg;
	return builder.str().c_str();
}