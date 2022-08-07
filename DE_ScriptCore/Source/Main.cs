using DragonEngine;

namespace EngineScript
{
    public class MainScript
    {
        public void BeginPlay()
        {
            Vector3 myVec = new Vector3(0.0f, 1.0f, 1.5f);

            Debug.Log("My Vec: {0}", myVec);
        }

        public void Update(float DeltaTime)
        {
            //Debug.Log($"Delta Time: {DeltaTime}");
        }
    }
}
