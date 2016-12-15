using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProfilerLauncher
{
    class Program
    {
        private static string PATH_EXE_TO_PROFILE = "EXE_TO_PROFILE.exe";
        private static string PROFILER_GUID = "{0ADAFE70-8F6C-4900-9B22-DEC4FAC5DD6A}";

        static void Main(string[] args)
        {
            ProcessStartInfo psi;
            if (args.Count() == 2)
            {
                PATH_EXE_TO_PROFILE = args[0];
                PROFILER_GUID = args[1];
            }
           psi = new ProcessStartInfo(PATH_EXE_TO_PROFILE);

           // ----- SET THE CLR ENVIRONMENT VARIABLES -------------------
           if (psi.EnvironmentVariables.ContainsKey("COR_ENABLE_PROFILING") == true)
               psi.EnvironmentVariables["COR_ENABLE_PROFILING"] = "1";
           else
               psi.EnvironmentVariables.Add("COR_ENABLE_PROFILING", "1");

           // set the COR_PROFILER env var. This indicates to the CLR which COM object to
           // instantiate for profiling.
           if (psi.EnvironmentVariables.ContainsKey("COR_PROFILER") == true)
               psi.EnvironmentVariables["COR_PROFILER"] = PROFILER_GUID;
           else
               psi.EnvironmentVariables.Add("COR_PROFILER", PROFILER_GUID);
          

           // ----- RUN THE PROCESS -------------------

           psi.UseShellExecute = false;
           Process p = Process.Start(psi);
        }
    }
}
