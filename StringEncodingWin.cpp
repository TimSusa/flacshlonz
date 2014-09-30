//#include "StringEncodingWin.hpp"
#include <iostream>
#include <fcntl.h>
/*
StringEncodingWin::StringEncodingWin()
{
    //_setmode(_fileno(stdout), _O_U16TEXT);
}

#ifdef Q_OS_WIN
char* StringEncodingWin::utf16ToUtf8(const wchar_t* input)
{
    char* buffer;
    int buffSize, result;
    buffSize = WideCharToMultiByte(CP_UTF8, 0, input, -1, NULL, 0, NULL, NULL);
    buffer = (char*) malloc(sizeof(char) * buffSize);
    result = WideCharToMultiByte(CP_UTF8, 0, input, -1, buffer, buffSize, NULL, NULL);
    return (result > 0 && result <= buffSize) ? buffer : NULL;
}
#else
char* StringEncodingWin::utf16ToUtf8(const wchar_t* input)
{
    return NULL;
}

#endif
*/
