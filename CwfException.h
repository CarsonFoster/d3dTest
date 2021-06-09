#ifndef CWF_CWFEXTENSION_H
#define CWF_CWFEXTENSION_H

#include <exception>
#include <string>

#define CWF_EXCEPTION(type, message) CwfException{ type, message, __FILE__, __LINE__ }

class CwfException {
public:
	enum class CwfExceptionType {
		WINDOWS, DIRECTX, FRAMEWORK, OTHER
	};
private:
	int line;
	const char* file;
	CwfExceptionType type;
	const wchar_t* msg;
public:
	CwfException(CwfExceptionType t, const wchar_t* message, const char* filename, int lineNumber) noexcept;
	~CwfException() = default;

	static std::wstring getStandardExceptionString(std::exception e) noexcept;

	int getLine() const noexcept;
	const char* getFile() const noexcept;
	CwfExceptionType getType() const noexcept;
	const wchar_t* getTypeAsString() const noexcept;
	const wchar_t* getMessage() const noexcept;
	std::wstring getExceptionString() const noexcept;
};

#endif
