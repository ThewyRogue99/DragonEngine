using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DragonEngine
{
    public class Scene
    {
        internal Scene(string SceneName)
        {
            Name = SceneName;
        }

        public readonly string Name;
        public List<Entity> Entities
        {
            get { return EntityList; }
        }

        private List<Entity> EntityList = new List<Entity>();

        public Entity CreateEntity(string tag = "Empty Entity") => new Entity(InternalCalls.Scene_CreateEntity(Name, tag));

        public bool DestroyEntity(Entity entity)
        {
            if (entity == null)
                return false;

            return InternalCalls.Scene_DestroyEntity(Name, entity.ID);
        }

        public Entity FindEntityByTag(string Tag)
        {
            return EntityList.Find(e => e.Tag == Tag);
        }

        public Entity FindEntityByID(string EntityID)
        {
            Entity result = new Entity(EntityID);
            if (!IsEntityValid(result))
                return null;

            return result;
        }

        public bool IsEntityValid(Entity entity)
        {
            return EntityList.Contains(entity);
        }

        internal void SetEntityList()
        {
            int ArrayLength = InternalCalls.Scene_GetEntityCount(Name);
            if(ArrayLength > 0)
            {
                string[] IDArray = new string[ArrayLength];

                InternalCalls.Scene_GetEntityIDList(Name, ref IDArray);

                foreach (string id in IDArray)
                    EntityList.Add(new Entity(id));
            }
        }
    }
}
