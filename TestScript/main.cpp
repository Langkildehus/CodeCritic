#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "Test script running...\n";

    int len;
    std::cin >> len;

    for (int c = 0; c < len; c++)
    {
        char b;
        std::cin >> b;
        std::cout << b << ' ';
    }

    std::cout << std::endl;
    return 0;
}
