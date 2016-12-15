namespace CallStackLogger
{
    public class NlogProfilerInfoLogger:MsmqProfilerInfoMessageLogger
    {
        public NlogProfilerInfoLogger()
        {
            ProfilerQueueMessageReader.Instance.OnMessage += Instance_OnMessage;
            ProfilerQueueMessageReader.Instance.NoMessage += Instance_NoMessage;
        }

        private void Instance_NoMessage(object sender, System.EventArgs e)
        {
            
        }

        private void Instance_OnMessage(object sender, System.Messaging.Message e)
        {
           
        }

        public override void Log(object result)
        {
            
        }
    }
}