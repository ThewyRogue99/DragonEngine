using System;

namespace DragonEngine
{
    public class Entity
    {
        protected Entity() { }

        internal Entity(uint Handle, Scene EScene)
        {
            EntityHandle = Handle;
            EntityScene = EScene;
        }

        private readonly uint EntityHandle;
        private readonly Scene EntityScene;

        public string Tag => GetComponent<TagComponent>().Tag;
        public string ID => GetComponent<IDComponent>().ID;

        internal InternalCalls.EntityInfo GetEntityInfo()
        {
            return new InternalCalls.EntityInfo(EntityHandle, EntityScene.Name);
        }

        public Transform Transform
        {
            get
            {
                InternalCalls.TransformComponent_GetTransform(GetEntityInfo(), out Transform result);
                return result;
            }
            set
            {
                InternalCalls.TransformComponent_SetTransform(GetEntityInfo(), ref value);
            }
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(GetEntityInfo(), componentType);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = new T() { Entity = this };
            return component;
        }

        public T AddComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>())
                return null;

            Type componentType = typeof(T);
            if (InternalCalls.Entity_AddComponent(GetEntityInfo(), componentType))
            {
                T component = new T() { Entity = this };
                return component;
            }

            return null;
        }

        public override bool Equals(object obj)
        {
            if(obj != null)
            {
                if(obj is Entity)
                {
                    Entity EntityObject = obj as Entity;
                    return EntityHandle == EntityObject.EntityHandle && EntityScene == EntityObject.EntityScene;
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
