
#pragma once


namespace Hud
{

class HarvestBar
{
	public:

	int levelm;	//level max
	int level;

	HarvestBar()
	{

		levelm = 300;
		level = 0;	
	};

	void start(int _levelm)
	{
		levelm = _levelm;
		level = 0;
	}

	bool tick()
	{
		level++;

		if(level >= levelm)
		{
			levelm = 0;
			return true;
		}

		return false;
	}

	void end()
	{
		levelm = 0;
	}

	void draw(int x, int y)
	{
		//if(levelm == 0)
		//	return;

		level++;
		level = level % levelm;

		float l = level;
		float ll = levelm;
		float factor = l/ll;

		//border color and line width
		const float z = -0.5;

		const float border_height = 50;
		const float border_width = 200;
		const float border_line_width = 2.0f;

		glEnable(GL_BLEND);

		glColor4ub(0,0,255,127);
		glLineWidth(border_line_width);


		glBegin(GL_LINES);

		float xmin = x + 0 - border_line_width/2.0f;
		float xmax = x + border_width + border_line_width/2.0f;

		float ymin = y + 0 - border_line_width/2.0f;
		float ymax = y + border_height + border_line_width/2.0f;


		glVertex3f(xmin, ymax, z);
		glVertex3f(xmax, ymax, z);

		glVertex3f(xmax, ymax, z);
		glVertex3f(xmax, ymin, z);

		glVertex3f(xmax, ymin, z);
		glVertex3f(xmin, ymin, z);

		glVertex3f(xmin, ymin, z);
		glVertex3f(xmin, ymax, z);

		glEnd();



		glColor4ub(0,64,128,127);

		
/*
		glLineWidth(border_height - 2*border_line_width);

		glBegin(GL_LINES);


		float x0 = x + 2*border_line_width;
		float y0 = y + +border_height/2;

		float x1 = x + border_width - 4*border_line_width;
		float y1 = y + border_height/2;
*/

		glLineWidth(border_height);

		glBegin(GL_LINES);

		float x0 = x;
		float y0 = y + +border_height/2;

		float x1 = x + factor*border_width;
		float y1 = y + border_height/2;

		glVertex3f(x0,y0, z);
		glVertex3f(x1,y1, z);

		glEnd();

		glLineWidth(1.0f);
		glColor4ub(255,255,255,255);

		glDisable(GL_BLEND);

	}


};

/*
	Wrappers for singleton
*/

}
