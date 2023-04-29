using System.Collections.Generic;

namespace DragonEngine
{
    public static class SceneManager
    {
        public static List<Scene> SceneList
        {
            get
            {
                Load();

                return _SceneList;
            }
        }

        public static Scene ActiveScene
        {
            get
            {
                Load();

                return _ActiveScene;
            }
        }

        private static List<Scene> _SceneList = null;
        private static Scene _ActiveScene = null;

        private static void Load()
        {
            // Load Scene List
            if (_SceneList == null)
            {
                string[] SceneNameList = InternalCalls.SceneManager_GetLoadedSceneNameArray();

                _SceneList = new List<Scene>();

                foreach (string SceneName in SceneNameList)
                {
                    _SceneList.Add(new Scene(SceneName));
                }
            }

            // Get Active Scene
            if(_ActiveScene == null)
            {
                string SceneName = InternalCalls.SceneManager_GetActiveSceneName();

                _ActiveScene = _SceneList.Find(scene => scene.Name == SceneName);
            }
        }

        public static Scene GetScene(string SceneName)
        {
            return SceneList.Find(scene => scene.Name == SceneName);
        }

        public static bool SetActiveScene(Scene scene)
        {
            Load();

            _ActiveScene = scene;

            return InternalCalls.SceneManager_SetActiveScene(scene.Name);
        }

        public static Scene LoadScene(string Path, string Name)
        {
            Load();

            string SceneName = InternalCalls.SceneManager_LoadScene(Path, Name);

            if (SceneName == null)
                return null;

            Scene result = new Scene(SceneName);

            _SceneList.Add(result);

            return result;
        }
    }
}
