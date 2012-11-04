#include "hud.hpp"



const int LUDICROUS_REZ = 4000; // fixme once players start using resolutions over this...... if not much sooner  8)



struct Quadticle {
	bool Alive;
	int x, y, x_speed, y_speed, y_dest; // destination, the point where it disappears

	Quadticle() {
		Alive = false;
		x = y = x_speed = y_speed = y_dest = 0;
	}
};



namespace Hud {
	class JetPackMeter {
		int prev_w; // previous width
		Quadticle quadticles[LUDICROUS_REZ]; 



	public:
		JetPackMeter() {
			prev_w = 0;
		}

		void draw(int _xresf, int _yresf, PlayerAgent_state* pa_s) {
			int h = 4; // height
			int y = _yresf-h*7;
			float w = _xresf * (float)pa_s->jetpack.fuel / JETPACK_FUEL_MAX;
			int xo /* x offset */ = (_xresf - w) / 2;
			draw_bound_texture(xo, y, w, h);
			for (int i = 0; i < prev_w - w; i++) {
				spawn_quadticle(
					xo + randrange(0, (w > 1) ? w - 1 : 1), 
					y,
					randrange(-2, 2), 
					-randrange(7, 11), 
					y - 80); // ..... somewhere random within current bar dimensions
			}
			prev_w = w;

			// health meter
			//Agent* a = pa_s->you;
			//if (a != NULL)
			//{
			//	w = _xresf * (float)a->status.health / (float)a->status.health_max;
			//	draw_bound_texture(	(_xresf - w) / 2, _yresf-h*9, w, h);
			//}



			// update & draw alive particles
			for (int i = 0; i < LUDICROUS_REZ; i++) {
				if (quadticles[i].Alive) {
					quadticles[i].x += quadticles[i].x_speed;
					quadticles[i].y += quadticles[i].y_speed;

					if (quadticles[i].y < quadticles[i].y_dest)
						quadticles[i].Alive = false;
					else
						draw_bound_texture(
							quadticles[i].x, 
							quadticles[i].y, h, h);
				}
			}
		}



		
		bool spawn_quadticle(int x, int y, int x_speed, int y_speed, int y_dest) {
			for (int i = 0; i < LUDICROUS_REZ; i++) {
				if (!quadticles[i].Alive) {
					quadticles[i].x = x;
					quadticles[i].y = y;
					quadticles[i].x_speed = x_speed;
					quadticles[i].y_speed = y_speed;
					quadticles[i].y_dest = y_dest;
					quadticles[i].Alive = true;
					return true;
				}
			}

			return false;
		}
	};
}