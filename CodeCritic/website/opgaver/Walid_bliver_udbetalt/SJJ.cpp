#include <iostream>
int main(){
std::ios_base::sync_with_stdio(0);
std::cin.tie(0);std::cout.tie(0);
int n, j, max = 0;
std::cin >> n;
for (int c = 0; c < n; c++)
{
std::cin >> j;
if (j > max)
max = j;
}
std::cout << max;}