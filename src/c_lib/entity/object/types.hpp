#pragma once

namespace Entities
{

typedef void (*entityLoad) (void);
typedef void (*entityReady) (class Entity*);
typedef void (*entityDie) (class Entity* object);
typedef void (*entityTick) (class Entity* object);
typedef void (*entityUpdate) (class Entity* object);

}   // Entities
