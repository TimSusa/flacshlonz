#ifndef STRINGENCODINGWIN_HPP
#define STRINGENCODINGWIN_HPP

#include <QtGlobal> // to get access to Q_OS_WIN macro
#ifdef Q_OS_WIN
#include <windows.h>
#include <io.h>
#endif



class StringEncodingWin
{
public:
    StringEncodingWin();
    char* utf16ToUtf8(const wchar_t*);
};

#endif // STRINGENCODINGWIN_HPP
