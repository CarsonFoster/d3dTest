#ifndef CWF_WSTRINGLITERAL_H
#define CWF_WSTRINGLITERAL_H

template <size_t N>
struct WStringLiteral {
	size_t size;
	wchar_t value[N];
	constexpr WStringLiteral(const wchar_t (&str)[N]) : size{ N } {
		for (int i{ 0 }; i < N; i++)
			value[i] = str[i];
	}
};

#endif