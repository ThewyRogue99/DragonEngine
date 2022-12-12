using System;

namespace DragonEngine
{
    public enum Visibility
    {
        Visible,
        Hidden
    }

    public class EditorReflection : Attribute
    {
        public EditorReflection(Visibility visibility = Visibility.Visible)
            => _visibility = visibility;

        protected Visibility _visibility;
    }
}
