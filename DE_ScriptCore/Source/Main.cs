using DragonEngine;

namespace EngineScript
{
    public class MainScript : Script
    {
        public void BeginPlay()
        {
            
        }

        public void Update(float DeltaTime)
        {
            if (Input.IsKeyDown(KeyCode.Key_Space))
                Debug.Log("Hello");
        }
    }
}
