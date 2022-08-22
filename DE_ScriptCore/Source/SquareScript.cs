using DragonEngine;

public class SquareScript : Script
{
    public int IntValue = 0;

    public void BeginPlay()
    {
        Debug.Log(AttachedEntity.Tag);
    }

    public void Update(float DeltaTime)
    {
        if(Input.IsKeyDown(KeyCode.Key_Space))
            Debug.Log(++IntValue);
    }
}
