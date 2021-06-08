#ifndef CWF_CWFEXTENSION_H
#define CWF_CWFEXTENSION_h

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
	CwfException(CwfExceptionType t, const wchar_t* message, const char* filename, int lineNumber);
	~CwfException() = default;

	// no copy init/assign
	CwfException(const CwfException& o) = delete;
	CwfException& operator=(const CwfException& o) = delete;

	int getLine() const noexcept;
	const char* getFile() const noexcept;
	CwfExceptionType getType() const noexcept;
	const wchar_t* getTypeAsString() const noexcept;
	const wchar_t* getMessage() const noexcept;
	const wchar_t* getExceptionString() const noexcept;
};

#endif
