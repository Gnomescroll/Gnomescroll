#pragma once


namespace t_map
{
	int T_MAP_BACKUP_SHADER = 0;
	
    int T_MAP_3D_TEXTURES = 1;
    int T_MAP_MAG_FILTER = 0;  //T_MAP_MAG_FILTER ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR

    int T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 1;
    int T_MAP_EXTURE_2D_ARRAY_MIPMAP_LEVELS = 6;

//if(gltIsExtSupported("GL_EXT_texture_filter_anisotropic"))
    // Set Flag that extension is supported

    int ANISOTROPIC_FILTERING = 1;
    float ANISOTROPY_LARGEST_SUPPORTED = 0.0f;

    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ANISOTROPY_LARGEST)

}
