using System.Runtime.CompilerServices;

namespace CallStackLogger
{
    public class ConsoleProfilerInfoLogger : MsmqProfilerInfoMessageLogger
    {
        public ConsoleProfilerInfoLogger()
        {
            this.deserializer=new JsonDeserializer();
        }
        public override void Log(object info)
        {
            System.Console.WriteLine(info.ToString());
        }
    }
}