#ifndef C_MAIN_H
#define C_MAIN_H



// CXX standard library

#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <format>
#include <iostream>

// Windows library

#include <windows.h>
#include <tlhelp32.h>




#define StrToWstr(string)			(std::wstring(string, &string[strlen(string)]))


#endif
