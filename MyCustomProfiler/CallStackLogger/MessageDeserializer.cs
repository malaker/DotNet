namespace CallStackLogger
{
    public abstract class MessageDeserializer
    {
        public abstract object Deserialize(System.Messaging.Message m);
    }
}