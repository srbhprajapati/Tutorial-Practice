/*
* Program for comparing two Dates
*/

using System;


namespace DateAndTime
{
	class Program
	{
		static int Main()
		{
			DateTime startDate = new DateTime(2012, 10, 12);
			Console.WriteLine("Starting Date : {0}", startDate);
			
			DateTime endDate = startDate.AddDays(10);
			Console.WriteLine("End Date : {0}", endDate);
			
			if(startDate < endDate)
			{
				Console.WriteLine("{0} is earlier date than {1}", startDate, endDate);
			}
			
			Console.Read();
			
			return 0;
		}
	}
}