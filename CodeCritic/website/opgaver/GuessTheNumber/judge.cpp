#include <iostream>
#include <cmath>

int main()
{
    int n;
    std::cin >> n;
    const int num = (rand() % (n - 1)) + 1;

    std::cout << '\r';

    int q, counter = 0;
    while (true)
    {
        counter++;

        std::cin >> q;
        if (q > num)
        {
            std::cout << "LOWER";
        }
        else if (q < num)
        {
            std::cout << "HIGHER";
        }
        else
        {
            std::cout << "CORRECT";
            break;
        }
    }

    std::cout << '\r';

    if (counter > static_cast<int>(std::ceil(log2(n))))
    {
        std::cout << 0;
    }
    else
    {
        std::cout << 1;
    }
    return 0;
}
