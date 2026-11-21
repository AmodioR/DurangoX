
#include <iostream>
#include <string>

#include "durangox/BinaryInspector.h"

int main(int argc, char* argv[])
{
    std::cout << "DurangoX Runtime\n";
    std::cout << "Initializing Binary Inspector...\n";

    std::string filePath = "build/DurangoX.exe";

    if (argc > 1)
    {
        filePath = argv[1];
    }

    BinaryInspector inspector;

    if (!inspector.inspect(filePath))
    {
        return 1;
    }

    return 0;
}
