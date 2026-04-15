#include <Windows.h>
#include <sstream>
#include <string>
#include <iostream>

namespace nam
{
    template<typename... Ts>
    void Print(Ts... args)
    {
        ((std::cout << args), ...);
        std::cout << std::endl;

        std::wstringstream ss;

        (ss << ... << args);
        ss << L"\n";
        OutputDebugString(ss.str().c_str());
    }

    template<typename... Ts>
    void FatalError(Ts... args)
    {
        ((std::cerr << args), ...);

        std::cerr << std::endl;

        _ASSERT(false && "Fatal Error called");
        exit(-1);
    }
}
