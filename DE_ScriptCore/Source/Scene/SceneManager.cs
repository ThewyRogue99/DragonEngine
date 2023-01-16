using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DragonEngine
{
    public static class SceneManager
    {
        public static Scene GetActiveScene()
        {
            string SceneName = InternalCalls.SceneManager_GetActiveSceneName();

            if (SceneName == null)
                return null;

            Scene result = new Scene(SceneName);
            result.SetEntityList();

            return result;
        }

        public static bool SetActiveScene(Scene scene)
        {
            return InternalCalls.SceneManager_SetActiveScene(scene.Name);
        }

        public static Scene LoadScene(string Path, string Name)
        {
            string SceneName = InternalCalls.SceneManager_LoadScene(Path, Name);

            if (SceneName == null)
                return null;

            Scene result = new Scene(SceneName);
            result.SetEntityList();

            return result;
        }
    }
}
