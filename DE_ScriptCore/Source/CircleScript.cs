using DragonEngine;

public class CircleScript : Script
{
    private float Force = 200f;

    public void BeginPlay()
    {
            
    }

    public void Update(float DeltaTime)
    {
        Rigidbody2DComponent rb2dc = AttachedEntity.GetComponent<Rigidbody2DComponent>();

        if (Input.IsKeyDown(KeyCode.Key_Right))
        {
            rb2dc.ApplyLinearImpulse(Vector2.Right * (Force * DeltaTime), true);
        }
        if (Input.IsKeyDown(KeyCode.Key_Left))
        {
            rb2dc.ApplyLinearImpulse(Vector2.Left * (Force * DeltaTime), true);
        }
        if (Input.IsKeyDown(KeyCode.Key_Up))
        {
            rb2dc.ApplyLinearImpulse(Vector2.Up * (Force * DeltaTime), true);
        }
        if (Input.IsKeyDown(KeyCode.Key_Down))
        {
            rb2dc.ApplyLinearImpulse(Vector2.Down * (Force * DeltaTime), true);
        }
    }
}
