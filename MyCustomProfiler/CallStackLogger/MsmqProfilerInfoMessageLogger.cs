using System;

namespace CallStackLogger
{
    public abstract class MsmqProfilerInfoMessageLogger
    {
        protected MessageDeserializer deserializer;
        private bool m_IsDone = false;
        public MsmqProfilerInfoMessageLogger()
        {
            ProfilerQueueMessageReader.Instance.OnMessage += MsMessageReader_MessageHandler;
            ProfilerQueueMessageReader.Instance.NoMessage += MsMessageReader_NoMessage;
        }

        public void Run()
        {
            while (!m_IsDone)
            {
                ProfilerQueueMessageReader.Instance.Read();
            }
        }

        private void MsMessageReader_NoMessage(object sender, EventArgs e)
        {
            m_IsDone = true;
        }

        private void MsMessageReader_MessageHandler(object sender, System.Messaging.Message e)
        {
            
            var result=deserializer.Deserialize(e);
            Log(result);
            (sender as ProfilerQueueMessageReader).RemoveFirstMessageFromQueue();
        }

        public abstract void Log(object result);
    }
}