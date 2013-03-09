#pragma once

#if GS_AWESOMIUM

#include <SDL/awesomium/viewport_class.hpp>

namespace Awesomium
{

int viewport_least_priority_sort(const void* _a, const void* _b)
{
    if (_a == NULL && _b != NULL)
        return 1;
    if (_a != NULL && _b == NULL)
        return -1;
    if (_a == NULL && _b == NULL)
        return 0;
    const class ChromeViewport* a = static_cast<const class ChromeViewport*>(_a);
    const class ChromeViewport* b = static_cast<const class ChromeViewport*>(_b);
    return (a->priority - b->priority);
}

class ViewportManager
{
    public:

        static const int MAX_VIEWPORTS = 8;

        class ChromeViewport* cva[MAX_VIEWPORTS]; //viewport class array
        int cvai; //viewport class array index

        int settings_window;
        int login_window;

    ViewportManager() :
        cvai(0), settings_window(-1), login_window(-1)
    {
        memset(this->cva, 0, sizeof(this->cva));
        this->add_settings_window();
        #if GS_AUTH
        this->add_login_window();
        #endif
    }

    ~ViewportManager()
    {
        for (int i=0; i<MAX_VIEWPORTS; i++)
            delete cva[i];
    }

    int add_viewport(class ChromeViewport* viewport)
    {
        IF_ASSERT(this->cvai >= MAX_VIEWPORTS) return -1;
        this->cva[this->cvai] = viewport;
        viewport->id = this->cvai;
        return this->cvai++;
    }

    void enable()
    {
        class ChromeViewport* settings = this->get_settings_window();
        class ChromeViewport* login = this->get_login_window();
        if (input_state.settings_window && settings != NULL)
            settings->focus();
        else
        if (input_state.login_window && login != NULL)
            login->focus();
    }

    void disable()
    {
        for (int i=0; i<this->cvai; i++)
            this->cva[i]->unfocus();
    }

    bool full()
    {
        return (this->cvai >= MAX_VIEWPORTS);
    }

    void add_settings_window()
    {
        GS_ASSERT(!this->is_valid_viewport(this->settings_window));
        class ChromeViewport* settings = new ChromeViewport;
        settings->load_file(GNOMESCROLL_SETTINGS_HTML);
        this->settings_window = this->add_viewport(settings);
    }

    void add_login_window()
    {
        GS_ASSERT(!this->is_valid_viewport(this->login_window));
        class ChromeViewport* login = new ChromeViewport;
        login->load_file(GNOMESCROLL_LOGIN_HTML);
        this->login_window = this->add_viewport(login);
    }

    bool is_valid_viewport(int vpi)
    {
        return (vpi >= 0 && vpi < MAX_VIEWPORTS);
    }

    class ChromeViewport* get_settings_window()
    {
        if (!this->is_valid_viewport(this->settings_window))
            return NULL;
        return this->cva[this->settings_window];
    }

    class ChromeViewport* get_login_window()
    {
        if (!this->is_valid_viewport(this->login_window))
            return NULL;
        return this->cva[this->login_window];
    }

    void update()
    {
        for (int i=0; i<this->cvai; i++)
            this->cva[i]->update();

        class ChromeViewport* settings = this->get_settings_window();
        class ChromeViewport* login = this->get_login_window();
        if (settings != NULL)
        {
            if (input_state.settings_window)
                settings->priority = 10;
            else
                settings->priority = 0;
        }
        if (login != NULL)
        {
            if (input_state.login_window)
                login->priority = 9;
            else
                login->priority = 0;
        }
  }

    class ChromeViewport* get_highest_priority_window()
    {
        class ChromeViewport* cv = NULL;
        for (int i=0; i<this->cvai; i++)
            if (cv == NULL || this->cva[i]->priority > cv->priority)
                cv = this->cva[i];
        return cv;
    }

    void draw()
    {
        class ChromeViewport* settings = this->get_settings_window();
        class ChromeViewport* login = this->get_login_window();
        if (settings != NULL && input_state.settings_window)
            settings->draw();
        else
        if (login != NULL && input_state.login_window)
            login->draw();

        MALLOX(class ChromeViewport*, vp, this->cvai);
        memcpy(vp, this->cva, this->cvai * sizeof(class ChromeViewport*));
        qsort(vp, this->cvai, sizeof(class ChromeViewport*), &viewport_least_priority_sort);
        for (int i=0; i<this->cvai; i++)
            if (vp[i] != login && vp[i] != settings)
                vp[i]->draw();
    }

    void webview_crash(awe_webview* webView)
    {
        IF_ASSERT(this->cvai <= 0) return;
        int i=0;
        for (; i<this->cvai; i++)
            if (this->cva[i]->webView == webView)
            {
                this->cva[i]->crash();
                break;
            }
        for (; i<this->cvai; i++)
            this->cva[i] = this->cva[i+1];
        this->cva[this->cvai--] = NULL;
    }

    void handle_mouse_event(const SDL_Event* event)
    {
        int x = 0;
        int y = 0;
        int sx = 0;
        int sy = 0;
        SDL_GetMouseState(&x, &y);

        class ChromeViewport* cv = NULL;
        for (int i=0; i<this->cvai; i++)
            if (this->cva[i]->position_in_window(x, y, &sx, &sy))
                if (cv == NULL || this->cva[i]->priority > cv->priority)
                    cv = this->cva[i];
        if (cv == NULL) return;

        if (event->type != SDL_MOUSEMOTION)
            cv->focus();
        cv->process_mouse_event(event);

        if (event->type != SDL_MOUSEMOTION)
        {   // defocus anything not clicked
            for (int i=0; i<this->cvai; i++)
                if (this->cva[i] != cv)
                    this->cva[i]->unfocus();
        }
    }

    void handle_keyboard_event(const SDL_Event* event)
    {
        for (int i=0; i<this->cvai; i++)
            if (this->cva[i]->in_focus)
                this->cva[i]->process_key_event(event);
    }

};

}   // Awesomium

#endif
