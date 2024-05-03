using System;

class Submission
{ 
    static public void Main(String[] args)
    {
        int j, max = 0, n = int.Parse(Console.ReadLine());
        string[] s = Console.ReadLine().Split(' ');
        for (int c  = 0; c < s.Length; c++)
        {
            j = int.Parse(s[c]);
            if (j > max)
            {
                max = j;
            }
        }
        Console.Write(max);
    }
}
