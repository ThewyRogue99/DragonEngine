namespace DragonEngine
{
	public enum MouseButton
	{
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Left = Button0,
		Right = Button1,
		Middle = Button2
	}

	public class Input
    {
        public static bool IsKeyDown(KeyCode Key) => InternalCalls.Input_IsKeyDown(Key);
        public static bool IsMouseButtonDown(MouseButton Button) => InternalCalls.Input_IsMouseButtonDown(Button);
    }
}
