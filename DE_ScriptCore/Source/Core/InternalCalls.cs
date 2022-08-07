using System.Runtime.CompilerServices;

namespace DragonEngine
{
    public static class InternalCalls
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_Native(string str);
    }
}
