#pragma once

namespace Entities
{

typedef void (*entityLoad) (void);
typedef void (*entityReady) (class Entity*);
typedef void (*entityDie) (class Entity* entity);
typedef void (*entityTick) (class Entity* entity);
typedef void (*entityUpdate) (class Entity* entity);

}   // Entities
