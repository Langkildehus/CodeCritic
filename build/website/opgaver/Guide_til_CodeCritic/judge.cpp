#include <iostream>

int main()
{
	int maxPoints, n, userN;
	std::cin >> maxPoints;
	std::cin >> n;
	std::cout << '\r';
	std::cin >> userN;
	std::cout << '\r' << maxPoints * (userN == n + 1);
}
