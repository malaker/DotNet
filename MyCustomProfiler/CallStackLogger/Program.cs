using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using CallStackLogger;

namespace CallStackLogger
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                new ConsoleProfilerInfoLogger().Run();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            Console.ReadKey();        
        }
    }
}

