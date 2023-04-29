using System.Collections.Generic;

namespace DragonEngine
{
    public class Scene
    {
        protected Scene() { }

        internal Scene(string SceneName)
        {
            Name = SceneName;
        }

        public readonly string Name;
        public List<Entity> EntityList
        {
            get
            {
                Load();

                return _EntityList;
            }
        }

        private List<Entity> _EntityList = null;

        public Entity CreateEntity(string tag = "Empty Entity") => new Entity(InternalCalls.Scene_CreateEntity(Name, tag), this);

        private void Load()
        {
            if(_EntityList == null)
            {
                uint[] HandleArray = InternalCalls.Scene_GetEntityHandleArray(Name);

                _EntityList = new List<Entity>();

                foreach (uint Handle in HandleArray)
                {
                    _EntityList.Add(new Entity(Handle, this));
                }
            }
        }

        public bool DestroyEntity(Entity entity)
        {
            if (EntityList.Contains(entity) && entity != null)
            {
                return InternalCalls.Scene_DestroyEntity(entity.GetEntityInfo());
            }

            return false;
        }

        public Entity FindEntityByTag(string Tag)
        {
            return EntityList.Find(e => e.Tag == Tag);
        }

        public Entity FindEntityByID(string EntityID)
        {
            return EntityList.Find(e => e.ID == EntityID);
        }

        public bool IsEntityValid(Entity entity)
        {
            return EntityList.Contains(entity);
        }

        public override bool Equals(object obj)
        {
            if (obj != null)
            {
                if (obj is Scene)
                {
                    Scene SceneObject = (obj as Scene);
                    return Name == SceneObject.Name;
                }
            }

            return false;
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }
    }
}
