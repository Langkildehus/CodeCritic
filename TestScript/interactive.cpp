#include <iostream>
#include <string>

int main()
{
    int n;
    std::cin >> n;

    int l = 0;
    int r = n;
    int m = (l + r) / 2;

    while (true)
    {
        std::cout << m << "\n";

        std::string res;
        std::cin >> res;

        if (res == "CORRECT")
        {
            break;
        }
        else if (res == "HIGHER")
        {
            l = m + 1;
        }
        else if (res == "LOWER")
        {
            r = m - 1;
        }

        m = (l + r) / 2;
    }

    return 0;
}
