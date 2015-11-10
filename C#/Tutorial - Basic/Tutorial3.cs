/*
* Swap the contents of two numbers using XOR operation
*/


using System;

namespace SwapNumbers
{
	class Program
	{
		public static int Main()
		{
			int i,k;
			
			Console.WriteLine("Enter the two integers : \n");
			i = int.Parse(Console.ReadLine());
			k = int.Parse(Console.ReadLine());
			
			Console.WriteLine("The two Numbers are {0} and {1} before swapping", i, k);
			
			i = i^k;
			k = i^k;
			i = i^k;
			
			Console.WriteLine("The two Numbers are {0} and {1} after swapping", i, k);
			
			Console.Read();
			
			return 0;
		}
	}
}