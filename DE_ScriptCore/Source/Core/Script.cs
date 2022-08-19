namespace DragonEngine
{
    public abstract class Script
    {
        private Entity entity;

        protected void AttachToEntity(ulong EntityID)
        {
            entity = new Entity(EntityID);
        }

        public Entity AttachedEntity
        {
            get { return entity; }
        }
    }
}
