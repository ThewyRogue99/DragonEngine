namespace DragonEngine
{
    public abstract class Script
    {
        private Entity entity;

        protected void AttachToEntity(uint EntityHandle, string SceneName)
        {
            Scene EntityScene = SceneManager.GetScene(SceneName);

            entity = new Entity(EntityHandle, EntityScene);
        }

        public Entity AttachedEntity
        {
            get { return entity; }
        }
    }
}
