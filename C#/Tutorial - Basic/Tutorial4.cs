/*
* Access Specifiers Demonstration Code
*/


using System;


namespace accessSpecifier
{
	class Program
	{
		static void Main()
		{
			classTwo obj = new classTwo();
			obj.show();
		}
	}
	
	class classOne
	{
		private int a;
		public int b = 1;
		protected int c = 3;
		internal int d = 5;
		protected internal int e = 4;
		
	}
	
	class classTwo : classOne
	{
		public void show()
		{
			Console.WriteLine("Values are : ");
			
			
			Console.WriteLine(b);
			Console.WriteLine(c);
			Console.WriteLine(d);
			Console.WriteLine(e);
			
			Console.ReadLine();
		}
		
	}
}