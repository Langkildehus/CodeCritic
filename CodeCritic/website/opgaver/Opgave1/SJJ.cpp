#include <iostream>
typedef unsigned long long ull;

int main()
{
    ull n, j, max = 0;
    std::cin >> n;
    for (int c = 0; c < n; c++)
    {
        std::cin >> j;
        if (j > max)
            max = j;
    }
    std::cout << max;
}
