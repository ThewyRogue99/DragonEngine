using DragonEngine;

public class CircleScript : Script
{
    public void BeginPlay()
    {
            
    }

    public void Update(float DeltaTime)
    {
        Rigidbody2DComponent rb2dc = AttachedEntity.GetComponent<Rigidbody2DComponent>();

        if (Input.IsKeyDown(KeyCode.Key_Right))
        {
            rb2dc.ApplyLinearImpulse(new Vector2(1f, 0f), true);
        }
        if (Input.IsKeyDown(KeyCode.Key_Left))
        {
            rb2dc.ApplyLinearImpulse(new Vector2(-1f, 0f), true);
        }
        if (Input.IsKeyDown(KeyCode.Key_Up))
        {
            rb2dc.ApplyLinearImpulse(new Vector2(0f, 1f), true);
        }
        if (Input.IsKeyDown(KeyCode.Key_Down))
        {
            rb2dc.ApplyLinearImpulse(new Vector2(0f, -1f), true);
        }
    }
}
