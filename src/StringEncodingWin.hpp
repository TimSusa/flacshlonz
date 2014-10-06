#ifndef STRINGENCODINGWIN_HPP
#define STRINGENCODINGWIN_HPP

#include <QtGlobal> // to get access to Q_OS_WIN macro
#ifdef Q_OS_WIN
#include <windows.h>
#include <io.h>
#endif

#ifdef Q_OS_WIN
// http://stackoverflow.com/questions/1842445/how-to-convert-stdwstring-to-a-tchar
// TCHAR* is defined to be wchar_t* if UNICODE is defined,
// otherwise it's char*. So your code might look something like this:
wchar_t* src;
TCHAR* result;
#ifdef UNICODE
result = src;
#else
//I think W2A is defined in atlbase.h, and it returns a stack-allocated var.
//If that's not OK, look at the documenation for wcstombs.
//result = W2A(src);
#endif
#endif


/*
class StringEncodingWin
{
public:
    StringEncodingWin();
    char* utf16ToUtf8(const wchar_t*);
    inline std::wstring convert2widestr(const wchar_t* const psz)
    {
        return psz;
    }
    inline std::wstring convert2widestr(const char* const psz)
    {
        std::size_t len = std::strlen(psz);
        if( ::std::string(psz).empty() ) return std::wstring();
        std::vector<wchar_t> result;
        len = WideCharToMultiByte( CP_ACP
                                             , 0
                                             , reinterpret_cast<LPCWSTR>(psz)
                                             , static_cast<int>(len)
                                             , NULL
                                             , 0
                                             , NULL
                                             , NULL
                                             );

        result.resize( len );
        if(result.empty()) return std::wstring();
        const int cbytes = WideCharToMultiByte( CP_ACP
                                                , 0
                                                , reinterpret_cast<LPCWSTR>(psz)
                                                , static_cast<int>(len)
                                                , reinterpret_cast<LPSTR>(&result[0])
                , static_cast<int>(result.size())
                , NULL
                , NULL
                );
        assert(cbytes);
        return std::wstring( result.begin(), result.begin() + cbytes );
    }
};
*/
#endif // STRINGENCODINGWIN_HPP
