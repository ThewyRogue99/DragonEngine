using System;

namespace DragonEngine
{
    public class Entity
    {
        protected Entity() { }

        internal Entity(string id)
        {
            ID = id;
        }

        public readonly string ID;
        public string Tag => GetComponent<TagComponent>().Tag;

        public Transform Transform
        {
            get
            {
                InternalCalls.TransformComponent_GetTransform(ID, out Transform result);
                return result;
            }
            set
            {
                InternalCalls.TransformComponent_SetTransform(ID, ref value);
            }
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = new T() { Entity = this };
            return component;
        }

        public override bool Equals(object obj)
        {
            if(obj != null)
            {
                if(obj is Entity)
                {
                    return ID == (obj as Entity).ID;
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
