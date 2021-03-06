/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "SDL_functions.hpp"

#include <sound/sound.hpp>
#include <common/compat_gl.h>
#include <input/handlers.hpp>

/*
    Example code from
    http://www.opengl.org/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)
*/

/*
void setupwindow(SDL_WindowID *window, SDL_GLContext *context)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize SDL's Video subsystem
        sdldie("Unable to initialize SDL"); // Or die on error

    // Request an opengl 3.2 context.
   //SDL doesn't have the ability to choose which profile at this time of writing,
    //but it should default to the core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // Turn on double buffering with a 24bit Z buffer.
    // You may need to change this to 16 or 32 for your system
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //Create our window centered at 512x512 resolution
    *window = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!*window) // Die if creation failed
        sdldie("Unable to create window");

    //Create our opengl context and attach it to our window
    *context = SDL_GL_CreateContext(*window);

    // This makes our buffer swap syncronized with the monitor's vertical refresh
    SDL_GL_SetSwapInterval(1);
}
*/


int DisplayBox()
{
    #ifdef _WIN32
    const char title[] = "Error: check console log";
    const char msg[] = "Possible Error";
    int msgboxID = MessageBox(NULL,
                              (LPCTSTR)title,
                              (LPCTSTR)msg,
                              MB_OK | MB_ICONWARNING);
    switch (msgboxID)
    {
        case IDOK:
            break;
    }
    return msgboxID;
    #endif
    return 0;
}

int glVersionErrorPopup(const char* gl_version, const char* gl_gpu)
{
    const char msg_fmt[] =
        "Your graphics card does not support OpenGL 2.1.\n"
        "Try updating your drivers.\n"
        "Your OpenGL Version is %s\n"
        "Your GPU is %s\n"
        "Press OK to exit.";

    const size_t msg_len = sizeof(msg_fmt) + strlen(gl_version) + strlen(gl_gpu);
    char* msg = (char*)malloc((msg_len+1) * sizeof(char));
    snprintf(msg, msg_len+1, msg_fmt, gl_version, gl_gpu);
    msg[msg_len] = '\0';

    #ifdef _WIN32
    const char title[] = "Error";
    int msgboxID = MessageBox(NULL,
                              (LPCTSTR)title,
                              (LPCTSTR)msg,
                              MB_OK | MB_ICONWARNING);
    switch (msgboxID)
    {
        case IDOK:
            enable_quit();
            break;
    }
    return msgboxID;
    #else
    printf("%s\n", msg);
    #endif
    free(msg);
    return 0;
}

int VersionMismatchBox(int local_version, int server_version)
{
    #ifdef _WIN32
    char message_fmt[] = "Version out of date!\nYour version: %d\nServer version: %d\nInstall new version from \nhttp://gnomescroll.com";
    char* message = (char*)malloc((sizeof(message_fmt) + 1 + 20 - 4) * sizeof(char));
    sprintf(message, message_fmt, local_version, server_version);
    char title[] = "Version out of date!";
    int msgboxID = MessageBox(NULL,
                              (LPCTSTR)message,
                              (LPCTSTR)title,
                              MB_OK | MB_ICONWARNING);
    switch (msgboxID)
    {
        case IDOK:
            enable_quit();
            break;
    }
    free(message);
    return msgboxID;
    #endif
    return 0;
}

int _xres = 800;
int _yres = 600;
float _xresf = (float) _xres;
float _yresf = (float) _yres;
int _fullscreen = 0;

SDL_Surface *pSDLSurface;
SDL_VideoInfo *pSDLVideoInfo;

void _del_video()
{
    static int SDL_CLOSED = 0;
    if (SDL_CLOSED++)
    {
        printf("Warning! SDL un-init function called twiced! \n");
        return;
    }
    printf("Deconstructing SDL OpenGL Window\n");
    SDL_Quit();
}

void close_SDL()
{
    _del_video();
}

int _set_resolution(int xres, int yres, int fullscreen)
{
    _xres = xres;
    _yres = yres;
    _xresf = (float)_xres;
    _yresf = (float)_yres;
    _fullscreen = fullscreen;
    return 0;
}

int _multisampling = 0;

void ati_meminfo(void)
{

    //printf("OpenGL: %s \n", (char*)glGetString(GL_VERSION));
    //printf("GPU: \n", (char*)glGetString(GL_RENDERER));
    //printf("Extentions= %s \n", (char*)glGetString(GL_EXTENSIONS));

    printf("\n");

    GLint param[4];

    glGetIntegerv (GL_VBO_FREE_MEMORY_ATI, param);
    printf ("VBO_FREE_MEMORY_ATI\n");
    printf ("  Total memory free in the pool: %i Kbyte\n", param[0]);
    printf ("  Largest available free block in the pool: %i Kbyte\n", param[1]);
    printf ("  Total auxiliary memory free: %i Kbyte\n", param[2]);
    printf ("  Largest auxiliary free block: %i Kbyte\n", param[3]);
    printf ("\n");

    glGetIntegerv (GL_TEXTURE_FREE_MEMORY_ATI, param);
    printf ("TEXTURE_FREE_MEMORY_ATI\n");
    printf ("  Total memory free in the pool: %i Kbyte\n", param[0]);
    printf ("  Largest available free block in the pool: %i Kbyte\n", param[1]);
    printf ("  Total auxiliary memory free: %i Kbyte\n", param[2]);
    printf ("  Largest auxiliary free block: %i Kbyte\n", param[3]);
    printf ("\n");

    glGetIntegerv (GL_RENDERBUFFER_FREE_MEMORY_ATI, param);
    printf ("RENDERBUFFER_FREE_MEMORY_ATI\n");
    printf ("  Total memory free in the pool: %i Kbyte\n", param[0]);
    printf ("  Largest available free block in the pool: %i Kbyte\n", param[1]);
    printf ("  Total auxiliary memory free: %i Kbyte\n", param[2]);
    printf ("  Largest auxiliary free block: %i Kbyte\n", param[3]);
    printf ("\n");

    return;
}

void set_window_icon(const char* filename)
{
    // have to load as BMP to avoid IMG_Load which can't safely be called before SDL_SetVideoMode
    // and SDL_WM_SetIcon must be called before SDL_SetVideoMode
    SDL_Surface* icon = SDL_LoadBMP(filename);
    IF_ASSERT(icon == NULL) return;
    // set transparency mask (red)
    Uint32 colorkey = SDL_MapRGB(icon->format, 0xFF, 0, 0);
    int ret = SDL_SetColorKey(icon, SDL_SRCCOLORKEY, colorkey);
    GS_ASSERT(!ret);
    SDL_WM_SetIcon(icon, NULL);
    SDL_FreeSurface(icon);
}


int init_video()
{
    //DisplayBox();

    int nFlags;
    int value;

    printf("Creating SDL window\n");
    //SDL_Init(SDL_INIT_VIDEO); // Initialise the SDL Video bit

    //-1 on error, 0 on success
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1)
    {
       // Unrecoverable error, exit here.
       printf("SDL_Init failed: %s\n", SDL_GetError());
       DisplayBox();
       return 1;
    }

    //printf("SDL: %s\n", SDL_GetError());

    SDL_WM_SetCaption("Gnomescroll", NULL);

    set_window_icon(MEDIA_PATH "icons/window.bmp");

    const SDL_VideoInfo *pSDLVideoInfo = SDL_GetVideoInfo();
    //pSDLVideoInfo = SDL_GetVideoInfo();
    //printf("SDL_GetVideoInfo: %s\n", SDL_GetError());

    if (pSDLVideoInfo == NULL)
    {
        printf("SDL_GetVideoInfo() failed. SDL Error: %s\n", SDL_GetError());
        DisplayBox();
        close_SDL();
        return 1;
    }

    nFlags = SDL_OPENGL; // | SDL_FULLSCREEN; //| SDL_GL_DOUBLEBUFFER; // | SDL_HWPALETTE;

    if (_fullscreen)
        nFlags |= SDL_FULLSCREEN;
/*
    if (pSDLVideoInfo->hw_available) // Hardware surfaces enabled?
        nFlags |= SDL_HWSURFACE;
    else {
        nFlags |= SDL_SWSURFACE;
        printf("SDL_HWSURFACE Error: Hardware surfaces not enabled!\n");
    }
    if (pSDLVideoInfo->blit_hw) // Hardware supported blitting?
        nFlags |= SDL_HWACCEL;
    else
        printf("SDL_HWACCEL Error: Hardware blitting not enabled!\n");
*/

    //if (0)
    //{//When the window is resized by the user a SDL_VIDEORESIZE event is generated and SDL_SetVideoMode can be called again with the new size.
        //nFlags |= SDL_RESIZABLE;
    //}

    nFlags |= SDL_DOUBLEBUF; // http://sdl.beuc.net/sdl.wiki/SDL_Flip

    if (_multisampling)
    {
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Enable OpenGL Doublebuffering
    }
    ///multisampling: no effect
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); //multisampling?
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    /// vsync
    //SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
    //may only work in full screen mode
    //deprecated

    /// anti-aliasing
    //SDL_GL_MULTISAMPLEBUFFERS
    //SDL_GL_MULTISAMPLESAMPLES

    // Create our rendering surface
    ///SDL_Surface *pSDLSurface = SDL_SetVideoMode(800, 600, 32, nFlags);
    //pSDLSurface = SDL_SetVideoMode(800, 600, 32, nFlags);

    #ifdef linux
    if (_fullscreen != 0)
        _set_resolution(pSDLVideoInfo->current_w, pSDLVideoInfo->current_h, _fullscreen);
    pSDLSurface = SDL_SetVideoMode(_xres, _yres, 32, nFlags);
    #else
    pSDLSurface = SDL_SetVideoMode(_xres, _yres, 32, nFlags);
    #endif

    //printf("SDL_SetVideoMode: %s\n", SDL_GetError());

    if (!pSDLSurface)
    {
        printf("Call to SDL_SetVideoMode() failed! - SDL_Error: %s\n", SDL_GetError());
        DisplayBox();
        close_SDL();
        return 1;
    }

    #ifdef __GNUC__
    SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &value);
    if (value) {
        //printf("Harware Acceleration Enabled \n");
    } else {
        printf("Warning: Hardware Acceleration Not Enabled!\n");
    }
    #endif

    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &value);
    if (value) {
        //printf("Double Buffering Enabled \n");
    } else {
        printf("Warning: Double Buffering Not Enabled!\n");
    }

    if (_multisampling) {
        SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &value);
        printf("Multisampling: number of samples per pixel %i \n", value);
    } else {
        glDisable(GL_MULTISAMPLE);
    }
    SDL_ShowCursor(SDL_DISABLE);

    //printf("SDL: %s\n", SDL_GetError());

    //printf("glew init\n");
    glewInit();


    //const char* GLVersionString = (char *) glGetString(GL_VERSION);
    //printf("OpenGL version: %s \n", GLVersionString);

    const char* gl_version = (const char*)glGetString(GL_VERSION);
    const char* gl_gpu = (const char*)glGetString(GL_RENDERER);
    const char* gl_vendor = (const char*)glGetString(GL_VENDOR);
    printf("OpenGL: %s \n", gl_version);
    printf("GPU: %s \n", gl_gpu);
    printf("Driver: %s \n", gl_vendor);

    if (glewIsSupported("GL_VERSION_2_1"))
        printf("OpenGL 2.1 is available\n");
    else
    {
        printf("OpenGL 2.1 is not available\n");
        glVersionErrorPopup(gl_version, gl_gpu);
    }

    //printf("Extentions= %s \n", (char*)glGetString(GL_EXTENSIONS));

    //Or better yet, use the GL3 way to get the version number
    //int OpenGLVersion[2];
    //glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
    //glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

    if (GLEW_ARB_multisample) {
        //printf("ARB_MULTISAMPLE supported \n");
    } else {
        printf("ARB_MULTISAMPLE not supported \n");
    }
/*
    if (GLEW_EXT_provoking_vertex)
    {
        //glProvokingVertexEXT(GL_FIRST_VERTEX_CONVENTION_EXT);
    } else {
        printf("warning: ProvokingVertex extention not supported \n");
    }
*/
    if (GLEW_EXT_timer_query) {
        if (PRODUCTION) printf("GL_EXT_timer_query supported\n");
    } else {
        printf("GL_EXT_timer_query not supported\n");
    }

    if (GLEW_EXT_texture_array)
    {
        if (PRODUCTION) printf("GL_EXT_texture_array supported\n");
    } else
    {
        printf("GL_EXT_texture_array not supported\n");
    }

    //printf("SDL: %s\n", SDL_GetError());

    //if (GLEW_ATI_meminfo) { ati_meminfo(); }

    //printf("Finished OpenGL init\n");

    //glClearColor(0.0,0.0,0.0,0.0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    return 0;
}

const int _L = 64;

static int _s_buffer[_L];
static int _s2_buffer[_L];
static int _s_index = 0;

static int _last_frame_t;

const int _SDL_SWAP_DEBUG = 0;

/*
    Measure the time since last frame and if there is extra time before next flip, do map processing

*/

/*
    !!! Can assume fixed offsets for timing
    OpenGL by default syncs to the monitor refresh. You need to use wglSwapIntervalEXT() to turn it off

    Swap Interval
    http://www.opengl.org/wiki/Swap_Interval
*/

/*
!!!

A swap interval greater than 0 means that the GPU may force the CPU to wait due to previously issued buffer swaps.
For example, if the v-blank intervals come at 16.6ms intervals (60fps refresh), but the rendering of a frame only takes 4ms,
then buffer swaps can back up. Therefore, the CPU driver will stall the rendering thread in an OpenGL command
(it doesn't have to be in a buffer swapping command) if there are too many commands waiting for the v-blank.

Alternatively, if the renderer takes slightly longer than the v-blank intervals to render, say 18ms,
then a different problem can result. It will effectively take two full v-blank intervals to display an image to the user,
turning a 60fps program into a 30fps program. It will also induce stalls, for the same reason as above: the GPU has
to wait 15.2ms every other v-blank interval for a buffer swap. Rendering calls made in that time will
 back up, eventually forcing a stall to wait for the actual swap.

Note that the problem of OpenGL commands backing up due to a waiting v-blank happen because these commands are trying to affect the back buffer.
If they do not affect the back buffer, either by rendering to a framebuffer object, another form of off-screen buffer, or something else that isn't the
back or front buffers, then these rendering commands can be scheduled as normal. Assuming that there are no other issues that would prevent such
execution (trying to render to a buffer that is being read from, for example). Thus, one can ease the CPU burden on waiting for v-blanks by
rendering to a third buffer, then blitting that to the back buffer, and then doing a swap. This is commonly called "triple buffering".

*/


/*

SGI_video_sync
http://www.opengl.org/registry/specs/SGI/video_sync.txt

*/
int _swap_buffers()
{
    int _time1;

    if (_SDL_SWAP_DEBUG)
    {
       _time1 = SDL_GetTicks();
    }

    // Do not call glFlush() before SDL_GL_SwapBuffers since the latter implies the former.

    //SDL_GL_SwapBuffers();
    //glFlush();
    //glFinish();
    SDL_GL_SwapBuffers();

    if (_SDL_SWAP_DEBUG)
    {
        int _time2 = SDL_GetTicks();

        _s_index = (_s_index + 1)%_L;

        _s2_buffer[_s_index] = _time2 - _last_frame_t;
        _s_buffer[_s_index] = _time2 - _time1;

        _last_frame_t = _time2;
        int _flip_time = _time2 - _time1;


        if (_flip_time > 2) {
            printf("Warning: SDL buffer swap took %i ms\n", _flip_time);
            //can do stuff here!!!
            int i;
            for (i=0; i < 8; i++) {
                printf("frame %i: %i ms, %i ms\n", i, _s_buffer[(_L+_s_index-i)%_L], _s2_buffer[(_L+_s_index-i)%_L]);
                //printf("frame %i: %i ms, %i ms\n", i, _s_buffer[(_s_index+i)%_L], _s2_buffer[(_s_index+i)%_L]);
            }
        }
    }
    return 0;
}

void save_screenshot()
{
    // Compresses an image to a compressed PNG file in memory.
    // On entry:
    //  pImage, w, h, and num_chans describe the image to compress. num_chans may be 1, 2, 3, or 4.
    // On return:
    //  Function returns a pointer to the compressed data, or NULL on failure.
    //  *pLen_out will be set to the size of the PNG image file.
    //  The caller must free() the returned heap block (which will typically be larger than *pLen_out) when it's no longer needed.
    //void *tdefl_write_image_to_png_file_in_memory(const void *pImage, int w, int h, int num_chans, size_t *pLen_out);

    char* pbuffer = (char*) malloc(4*_xres*_yres);

    glReadPixels(0, 0, _xres, _yres, GL_RGBA, GL_UNSIGNED_BYTE, (void*) pbuffer);

    for (int i=0; i < _xres; i++)
    for (int j=0; j < _yres; j++)
        pbuffer[4*(_xres*j + i) + 3] = 255;

    void* temp_row = (void*)malloc(4*_xres);
    GS_ASSERT(temp_row != NULL);
    if (temp_row != NULL)
    {
        int pitch = _xres * 4;
        int h = _yres;

        int height_div_2 = (int) (_yresf * 0.5f);
        for (int index = 0; index < height_div_2; index++)
        {
            memcpy((Uint8*)temp_row, (Uint8*)(pbuffer) + pitch * index, pitch);
            memcpy((Uint8*)(pbuffer) + pitch * index, (Uint8*)pbuffer + pitch * (h - index-1), pitch);
            memcpy((Uint8*)(pbuffer) + pitch * (h - index-1), temp_row, pitch);
        }
        free(temp_row);
    }

    size_t png_size;
    char* png_image = (char*) tdefl_write_image_to_png_file_in_memory(
        (const char*) pbuffer, _xres, _yres, 4, &png_size);

    const int LAST_FILENAME_LEN = 128;
    static char last_filename[LAST_FILENAME_LEN] = {'\0'};
    static int ext_ct = 0;
    const char ext[] = "png";

    char* timestr = get_time_str();
    if (timestr == NULL) timestr = (char*)"notime";
    const char fmt[] = SCREENSHOT_PATH "%s";
    size_t len = strlen(timestr) + sizeof(fmt) + sizeof(ext) + 3 + 1; // +3 to fit name collision extension, +1 for ext separator
    char* filename = (char*)malloc(len * sizeof(char));
    int wrote = snprintf(filename, len, fmt, timestr);
    for (int i=0; i<wrote; i++) // convert : to _
        if (filename[i] == ':') filename[i] = '_';
    if (!strcmp(last_filename, filename))
    {
        ext_ct++;
        const char ext_fmt[] = "%s.%02d";
        GS_ASSERT(count_digits(ext_ct) <= 2);
        if (count_digits(ext_ct) > 2) ext_ct = 99;
        wrote = snprintf(filename, len, ext_fmt, filename, ext_ct);

        for (int i=0; i<wrote; i++) // convert : to _
            if (filename[i] == ':') filename[i] = '_';
    }
    else
        ext_ct = 0;
    strncpy(last_filename, filename, LAST_FILENAME_LEN);
    GS_ASSERT(!strcmp(last_filename, filename));

    const char final_fmt[] = "%s.%s";
    sprintf(filename, final_fmt, last_filename, ext);

    printf("Saving screenshot to %s\n", filename);

    FILE* pFile = fopen(filename , "wb");
    GS_ASSERT(pFile != NULL);
    if (pFile != NULL)
    {
        fwrite(png_image , 1 , png_size, pFile);
        fclose(pFile);
    }

    free(png_image);
    free(pbuffer);
    free(filename);
}

int check_gl_error()
{
    int error = glGetError();
    if (error)
        printf("GL_ERROR: %s\n", gluErrorString(error));
    return error;
}

int check_gl_error(const char* filename, const int line_no)
{
    int error = glGetError();
    if (error)
        printf("%s:%d - GL_ERROR: %s\n", filename, line_no, gluErrorString(error));
    return error;
}
