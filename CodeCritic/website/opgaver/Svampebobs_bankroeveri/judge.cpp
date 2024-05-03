#include <iostream>

int main()
{
	int ans, n, j, max1 = 0, max2 = 0;
	std::cin >> n;
	for (int c = 0; c < n; c++)
	{
		std::cin >> j;
		if (j > max1)
		{
			max2 = max1;
			max1 = j;
		}
		else if (j > max2)
		{
			max2 = j;
		}
	}
	std::cout << '\r';
	std::cin >> ans;
	std::cout << '\r' << (ans == max2);
}
