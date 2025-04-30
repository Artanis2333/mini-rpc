#include <iostream>

#include "message_example.mrpc.h"

template<typename T>
void PrintType()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

int main(int argc, char* argv[])
{
    example::StlExample obj;
    PrintType<decltype(obj.vector_value)>();
    PrintType<decltype(obj.list_value)>();
    PrintType<decltype(obj.map_value)>();
    PrintType<decltype(obj.unordered_map_value)>();
    return 0;
}
