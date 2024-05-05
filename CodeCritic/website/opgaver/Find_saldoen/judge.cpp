#include <iostream>
#include <cmath>
#include <time.h>
typedef unsigned long long ull;

int main()
{
    int maxPoints, optimal, counter = 0;
    std::cin >> maxPoints;
    ull n;
    std::cin >> n;
    srand(time(NULL));
    const ull num = (rand() % (n - 1)) + 1;

    ull l = 1, r = n, m;
    while (l < r)
    {
        counter++;
        m = (l + r) / 2;
        if (num < m)
            r = m;
        else if (num > m)
            l = m;
        else
        {
            optimal = counter;
            counter = 0;
            break;
        }
    }

    std::cout << '\r';

    ull q;
    while (q != num)
    {
        counter++;
        std::cin >> q;
        if (q > num)
            std::cout << "LOWER" << std::endl;
        else if (q < num)
            std::cout << "HIGHER" << std::endl;
        else
            std::cout << "CORRECT" << std::endl;
    }

    std::cout << '\r';

    double loss = static_cast<double>(counter - optimal) / static_cast<double>(n) / 2;
    loss *= loss;
    if (loss >= 1)
        std::cout << 0;
    else if (loss <= 0)
        std::cout << maxPoints;
    else
        std::cout << static_cast<int>(maxPoints * (1 - loss));
}
