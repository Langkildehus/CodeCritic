#include <iostream>

int main()
{
	int n, j, biggest = 0;
	std::cin >> n;
	for (int c = 0; c < n; c++)
	{
		std::cin >> j;
		if (j > biggest)
			biggest = j;
	}
	std::cout << biggest;
}
