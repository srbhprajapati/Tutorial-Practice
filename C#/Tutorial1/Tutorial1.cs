/*
* Program to check whether the entered number is even or odd
*/


using System.IO;
using System;

class Program
{
    static void Main()
    {
        int i;
        Console.WriteLine("Enter a number: ");
        i = int.Parse(Console.ReadLine());
        
        if(i%2==0)
        {
            Console.WriteLine("Entered Number is Even");
            Console.ReadLine();
        }
        else
        {
            Console.WriteLine("Entered Number is Odd");
            Console.ReadLine();
        }
        
    }
}
