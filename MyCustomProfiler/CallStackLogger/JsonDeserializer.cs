using System;
using System.IO;
using System.Linq;
using System.Messaging;
using System.Text;
using System.Text.RegularExpressions;

namespace CallStackLogger
{
    public class JsonDeserializer : MessageDeserializer
    {
        private static Regex reg = new Regex("\"Fname\":\"(?<fname>[a-zA-Z0-9]|.*)\".\"callStackLevel\":(?<callstack>[0-9]*)");
        public override object Deserialize(Message m)
        {
            string body=string.Empty;
            MemoryStream ms = new MemoryStream();
            m.BodyStream.CopyTo(ms);
            body = new UTF8Encoding().GetString(ms.GetBuffer(), 0, (int)ms.Length).Replace("\0", "");
            JsonProfilerInfoMessage msg = new JsonProfilerInfoMessage();
            if (reg.IsMatch(body))
            {
                var match = reg.Match(body);
                var fname = match.Groups["fname"].Value;
                msg.FunctionName = fname;
                var callstackStr = match.Groups["callstack"].Value;
                msg.CallStackLevel = System.Convert.ToInt32(callstackStr);
               
                return msg;
            }
            throw new ArgumentException("Cannot deserialize message with Json Deserializer");
        }
    }

    [Serializable]
    public class JsonProfilerInfoMessage
    {
        public string FunctionName { get; set; }
        public int CallStackLevel { get; set; }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            foreach (var i in Enumerable.Range(0, this.CallStackLevel))
            {
                sb.Append(" ");
            }
            sb.Append(FunctionName);
            return sb.ToString();
        }
    }
}