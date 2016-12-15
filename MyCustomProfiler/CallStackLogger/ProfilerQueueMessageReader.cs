using System;
using System.ComponentModel;
using System.Messaging;
using System.Text.RegularExpressions;

namespace CallStackLogger
{
    public class ProfilerQueueMessageReader
    {      
        public event EventHandler<Message> OnMessage;
        public event EventHandler NoMessage;
        private static string QUEUE_NAME = @".\Private$\testq";
       
        private MessageQueue queue;
        private Message _lastMessage;
        private static ProfilerQueueMessageReader _instance;

        private ProfilerQueueMessageReader()
        {
            this.queue = new MessageQueue(QUEUE_NAME);
        }
      
        public static ProfilerQueueMessageReader Instance
        {
            get {  return _instance = _instance ?? new ProfilerQueueMessageReader(); }
        }
        public virtual void Read()
        {
            var m = queue.Peek(TimeSpan.FromMinutes(1));

            if (m != null)
            {
                OnMessage?.Invoke(this, m);
            }
            else
            {
                NoMessage(this,new EventArgs());
            }
        }

        public virtual void RemoveFirstMessageFromQueue()
        {
            _lastMessage = this.queue.Receive();
        }
    }
}