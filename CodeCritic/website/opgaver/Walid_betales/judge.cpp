#include <iostream>

/*
Assignment: Given a list of length n, return the highest number in the list.
Input:
- line 1: n
- line 2: n numbers sepperated with spaces

Output:
- The biggest number
*/

int main()
{
    /*
    Step 1: solve the problem as the judge
    This happens before the test has started on the submission
    If the problem has multiple solutions, it might be better to just save the test inputs for now, and not solve the problem before the submission has sent their answer
    This way you can check if their solution is valid afterwards rather than having to calculate all possible solutions
    */
    int maxPoints, n, j, ans, biggest = 0;
    std::cin >> maxPoints;
    std::cin >> n;
    for (int c = 0; c < n; c++)
    {
        std::cin >> j;
        if (j > biggest)
            biggest = j;
    }

    // Step 2: Tell the server the judge is ready to listen for submission inputs
    std::cout << '\r';

    // Step 3: Take input from the submission
    std::cin >> ans;

    /*
    Step 4: Tell server the entire test has been completed
    All messages send to stdout before this '\r' will be forwarded to the submission
    This is necessary since interactive problems need to communicate with the submission before the test has been completed
    */
    std::cout << '\r';

    /*
    Step 5: Return result of test.
    The maximum amount of points a submission can give, is given in the first input from stdin, in this cas maxPoints
    */
    std::cout << maxPoints * (ans == biggest);
}
