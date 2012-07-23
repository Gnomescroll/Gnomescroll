#pragma once

namespace Objects
{

typedef void (*objectLoad) (void);
typedef class Object* (*objectCreate) (void);
typedef void (*objectReady) (class Object*);
typedef void (*objectDie) (class Object* object);

}   // Objects
