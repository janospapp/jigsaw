#include "Utils/Debug.hpp"
#include <iostream>

void debugPrint(std::string &&string)
{
#ifdef _DEBUG
    std::cout << string << std::endl;
#endif
}
