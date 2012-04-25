#pragma once

#include <c_lib/t_hud/_interface.hpp>

#include <c_lib/t_hud/texture.hpp>

#include <c_lib/SDL/SDL_functions.h>

namespace t_hud
{

extern InventoryUI* agent_inventory;
		//if(i == 0 && j == 0) continue;
	glDisable(GL_TEXTURE_2D);

	if(selected_slot_inventory == inventory_id)
	{	

		const float x = inventory_hud_x_off + border + selected_slot_x*(inc1+slot_size);
		const float y = inventory_hud_y_off + border + (ydim-1-selected_slot_y)*(inc1+slot_size);

		const float b = 2 + inc2;

		glColor4ub(0, 0, 128+64, 255);
		glLineWidth(2.0);

		glBegin(GL_LINES);

		glVertex3f(x-b, y+w+b, z);
		glVertex3f(x+w+b, y+w+b, z);

		glVertex3f(x+w+b, y+w+b, z);
		glVertex3f(x+w+b, y-b, z);

		glVertex3f(x+w+b, y-b, z);
		glVertex3f(x-b, y-b, z);

		glVertex3f(x-b, y-b, z);
		glVertex3f(x-b, y+w+b, z);

		glEnd();
		//extern int selected_slot_x;
		//extern int selected_slot_y;)

		glLineWidth(1.0);


	}


	glColor4ub(255, 255, 255, 255);

void init_agent_inventory_ui();


}
