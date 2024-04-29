#include <iostream>
#include <cmath>
#include <time.h>
typedef unsigned long long ull;

int main()
{
    int counter = 0;
    ull n;
    std::cin >> n;
    srand(time(NULL));
    const ull num = (rand() % (n - 1)) + 1;

    std::cout << '\r';

    ull q;
    while (true)
    {
        counter++;
        std::cin >> q;
        if (q > num)
            std::cout << "LOWER" << std::endl;
        else if (q < num)
            std::cout << "HIGHER" << std::endl;
        else
        {
            std::cout << "CORRECT" << std::endl;
            break;
        }
    }

    std::cout << '\r';

    if (counter > static_cast<int>(std::ceil(log2(n))))
        std::cout << 0;
    else
        std::cout << 1;
}
