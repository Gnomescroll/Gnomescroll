#pragma once

namespace Entities
{

typedef void (*entityLoad) (void);
typedef class Entity* (*entityCreate) (void);
typedef void (*entityReady) (class Entity*);
typedef void (*entityDie) (class Entity* object);

}   // Entities
