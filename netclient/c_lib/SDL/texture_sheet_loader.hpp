#pragma once

namespace TextureSheetLoader
{


class TextureSheet
{
	public:
	int w; //width
	int h; //height

	unsigned int* pixels;
};

const int TEXTURE_SHEET_LIST_MAX 64;

class TextureSheetList
{
	public:

	int texture_num;
	TextureSheet textures[TEXTURE_SHEET_LIST_MAX];

	TextureSheetList()
	{
		texture_num=0;

	}

	int add_texture(char* filename)
	{
		if(texture_num == TEXTURE_SHEET_LIST_MAX)
		{
			printf("TextureSheetList Error: max texture sheets reached\n");
			return -1;
		}

		//load texture
	}

	void blit()
};


}