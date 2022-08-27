namespace DragonEngine
{
    public abstract class Script
    {
        private Entity entity;

        protected void AttachToEntity(string EntityID)
        {
            entity = new Entity(EntityID);
        }

        public Entity AttachedEntity
        {
            get { return entity; }
        }
    }
}
