
#include <iostream>
#include "BinaryInspector.h"

int main()
{
    std::cout << "DurangoX Runtime\n";
    std::cout << "Initializing Binary Inspector...\n";

    BinaryInspector inspector;

    if (!inspector.inspect("build/DurangoX.exe"))
    {
        return 1;
    }

    return 0;
}
