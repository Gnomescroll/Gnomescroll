#pragma once

//#include "verlet.hpp"

namespace VerletParticle
{

const float dt = 1/30.0f;
const float gravity = -9.8f;




class VerletParticle

{
	public:

	//struct Vec3 old_position;
	//struct Vec3 old_velocity;
	struct Vec3 position;
	struct Vec3 velocity;

	float dampening;

	bool bool_bounce();
	void bounce();

	void bounce_box(float radius);
};


}
