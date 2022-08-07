namespace DragonEngine
{
    public class Debug
    {
        public static void Log()
        {
            InternalCalls.Log_Native("");
        }

        public static void Log(bool value)
        {
            InternalCalls.Log_Native(value.ToString());
        }

        public static void Log(char value)
        {
            InternalCalls.Log_Native(value.ToString());
        }

        public static void Log(char[] buffer)
        {
            string str = new string(buffer);
            InternalCalls.Log_Native(str);
        }

        public static void Log(char[] buffer, int index, int count)
        {
            string str = new string(buffer, index, count);
            InternalCalls.Log_Native(str);
        }

        public static void Log(decimal value)
        {
            InternalCalls.Log_Native(value.ToString());
        }

        public static void Log(double value)
        {
            InternalCalls.Log_Native(value.ToString());
        }

        public static void Log(float value)
        {
            InternalCalls.Log_Native(value.ToString());
        }

        public static void Log(int value)
        {
            InternalCalls.Log_Native(value.ToString());
        }

        public static void Log(uint value)
        {
            InternalCalls.Log_Native(value.ToString());
        }

        public static void Log(long value)
        {
            InternalCalls.Log_Native(value.ToString());
        }

        public static void Log(ulong value)
        {
            InternalCalls.Log_Native(value.ToString());
        }

        public static void Log(object value)
        {
            InternalCalls.Log_Native(value.ToString());
        }

        public static void Log(string format, object arg0)
        {
            string str = string.Format(format, arg0);
            InternalCalls.Log_Native(str);
        }

        public static void Log(string format, params object[] args)
        {
            string str = string.Format(format, args);
            InternalCalls.Log_Native(str);
        }

        public static void Log(string str)
        {
            InternalCalls.Log_Native(str);
        }
    }
}
