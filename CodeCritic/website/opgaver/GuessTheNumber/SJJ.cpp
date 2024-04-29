#include <iostream>
#include <string>
typedef unsigned long long ull;

int main()
{
    std::ios_base::sync_with_stdio(0);
    std::cin.tie(0);std::cout.tie(0);
    std::string s;
    ull n;
    std::cin >> n;

    ull m, l = 1, r = n;
    while (l < r)
    {
        m = (l + r) / 2;
        std::cout << m << std::endl;
        std::cin >> s;
        if (s == "LOWER")
            r = m;
        else if (s == "HIGHER")
            l = m;
        else
            break;
    }
}
