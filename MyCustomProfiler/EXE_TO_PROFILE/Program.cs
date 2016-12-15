using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            string myParam = "Hello World!";
            int myIntParam = 1111;
            MyPrint(myParam,myIntParam);
        }

        public static void MyPrint(string stringParam,int myIntParam)
        {
            Console.WriteLine(stringParam + " " + myIntParam.ToString());
        }
    }
}
