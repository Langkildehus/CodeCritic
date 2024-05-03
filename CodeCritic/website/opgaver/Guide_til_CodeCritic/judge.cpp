#include <iostream>

int main()
{
	int n, userN;
	std::cin >> n;
	std::cout << '\r';
	std::cin >> userN;
	std::cout << '\r' << (userN == n + 1);
}
