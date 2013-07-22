#pragma once

#include <stdio.h>
#include <hud/input.hpp>
#include <auth/web.hpp>

namespace Hud
{

class Form
{
    protected:
        InputBox** inputs;
        int input_index;
        int n_inputs;
        int focused;

    #define REGISTER_INPUT(TYPE, LABEL) \
        TYPE* register_##LABEL##_input(const char* name) \
        { \
            IF_ASSERT(this->input_index >= this->n_inputs) return NULL; \
            TYPE* input = new TYPE; \
            this->inputs[this->input_index] = input; \
            input->set_name(name); \
            if (this->input_index == 0) \
                input->focus(); \
            this->input_index++; \
            return input; \
        }

    REGISTER_INPUT(Button, button)
    REGISTER_INPUT(InputCheckBox, checkbox)
    REGISTER_INPUT(InputTextBox, text)

    #undef REGISTER_INPUT

    void sanity_check()
    {
        for (int i=0; i<this->n_inputs; i++)
            GS_ASSERT(this->inputs[i] != NULL);
    }

    virtual void draw_label()
    {
        if (!this->n_inputs) return;
        this->label.set_text(this->name());
        this->label.set_color(Color(180, 220, 180));
        Vec2 p = this->position;
        p.y += this->label.get_height();
        p.y += this->inputs[0]->dims.y + this->inputs[0]->border.y * 0.5f;
        p.x += (this->inputs[0]->dims.x - this->label.get_width()) * 0.5f;
        this->label.set_position(p);
        this->label.draw();
    }

    public:

        Vec2 position;
        HudText::Text label;

    virtual void added_to_screen()
    {
    }

    void tab()
    {
        this->cycle();
    }

    void enter()
    {
        InputBox* input = this->get_focused_input();
        if (input == NULL) return;
        switch (input->type)
        {
            case UI_INPUT_TEXT:
            case UI_INPUT_BUTTON:
                this->submit();
                break;
            case UI_INPUT_CHECKBOX:
                input->enter();
                break;
            case UI_INPUT_NULL:
            default:
                break;
        }
    }

    void focus()
    {
        if (this->focused >= 0) return;
        this->focused = 0;
        InputBox* input = this->get_focused_input();
        IF_ASSERT(input == NULL) return;
        input->focus();
    }

    void unfocus()
    {
        for (int i=0; i<this->n_inputs; i++)
            this->inputs[i]->unfocus();
        this->focused = -1;
    }

    void backspace()
    {
        InputBox* input = this->get_focused_input();
        if (input != NULL)
            input->backspace();
    }

    void insert(char c)
    {
        InputBox* input = this->get_focused_input();
        if (input != NULL)
            input->insert(c);
    }

    void erase_save(const char* name)
    {
        char* path = this->get_save_file_path(name);
        IF_ASSERT(path == NULL) return;
        remove(path);
        free(path);
    }

    char* get_save_file_path(const char* name)
    {
        if (name == NULL) name = this->name();
        using ClientState::active_system_data_path;
        IF_ASSERT(name == NULL || name[0] == '\0')
            return NULL;
        const char fmt[] = "%s/%s";
        char* path = (char*)malloc(strlen(active_system_data_path) + strlen(name) + sizeof(fmt));
        sprintf(path, fmt, active_system_data_path, name);
        return path;
    }

    FILE* get_save_file(const char* fn, const char* mode)
    {
        char* path = this->get_save_file_path(fn);
        IF_ASSERT(path == NULL) return NULL;
        FILE* f = fopen(path, mode);
        free(path);
        return f;
    }

    void click(const Vec2i& cursor)
    {
        this->focused = -1;
        for (int i=0; i<this->n_inputs; i++)
        {
            this->inputs[i]->click(cursor);
            if (this->inputs[i]->focused)
                this->focused = i;
            if (this->inputs[i]->type == UI_INPUT_BUTTON &&
                reinterpret_cast<Button*>(this->inputs[i])->activated)
            {
                this->submit();
                break;
            }
        }
    }

    void hover(const Vec2i& cursor)
    {
        for (int i=0; i<this->n_inputs; i++)
            this->inputs[i]->hover(cursor);
    }

    void cursor_left()
    {
        InputBox* input = this->get_focused_input();
        if (input != NULL)
            input->cursor_left();
    }

    void cursor_right()
    {
        InputBox* input = this->get_focused_input();
        if (input != NULL)
            input->cursor_right();
    }

    void cycle()
    {
        InputBox* input = this->get_focused_input();
        if (input == NULL) return;
        input->unfocus();
        this->focused = (this->focused + 1) % this->n_inputs;
        input = this->get_focused_input();
        if (input != NULL)
            input->focus();
    }

    InputBox* get_focused_input()
    {
        if (this->focused < 0) return NULL;
        IF_ASSERT(this->focused >= this->n_inputs) return NULL;
        return this->inputs[this->focused];
    }

    InputBox* get_input_by_name(const char* name)
    {
        for (int i=0; i<this->n_inputs; i++)
            if (strcmp(this->inputs[i]->get_name(), name) == 0)
                return this->inputs[i];
        return NULL;
    }

    virtual void save(const char* fn)
    {
        FILE* f = this->get_save_file(fn, "w");
        IF_ASSERT(f == NULL) return;

        for (int i=0; i<this->n_inputs; i++)
            if (this->inputs[i]->save)
            {
                const char* name = this->inputs[i]->get_name();
                IF_ASSERT(name == NULL || name[0] == '\0') continue;
                const char* value = this->inputs[i]->get_value();
                size_t wrote = fwrite(name, sizeof(*name), strlen(name), f);
                GS_ASSERT(wrote == strlen(name));
                wrote = fwrite("=", sizeof(char), sizeof("=") - 1, f);
                GS_ASSERT(wrote == sizeof("=") - 1);
                if (value != NULL)
                {
                    wrote = fwrite(value, sizeof(*value), strlen(value), f);
                    GS_ASSERT(wrote == strlen(value));
                }
                wrote = fwrite("\n", sizeof(char), sizeof("\n") - 1, f);
                GS_ASSERT(wrote == sizeof("\n") - 1);
            }

        fclose(f);
    }

    void load(const char* fn)
    {
        char* path = this->get_save_file_path(fn);
        IF_ASSERT(path == NULL) return;
        size_t len = 0;
        char* data = read_file_to_buffer(path, &len);
        if (data == NULL) return;

        const size_t iiv_len = 63;
        char* input_name = (char*)malloc(InputBox::name_max_length + 1);
        char* value = (char*)malloc(iiv_len + 1);
        size_t iin = 0;
        size_t iiv = 0;
        bool on_input_name = true;
        for (size_t i=0; i<len; i++)
        {
            char c = data[i];
            if (c == '=')
            {
                iiv = 0;
                on_input_name = false;
                input_name[iin] = '\0';
            }
            else if (c == '\n')
            {
                value[iiv] = '\0';
                on_input_name = true;
                iin = 0;

                InputBox* input = this->get_input_by_name(input_name);
                IF_ASSERT(input == NULL) break;
                input->set_value(value);
            }
            else if (on_input_name)
            {
                IF_ASSERT(iin >= InputBox::name_max_length) break;
                input_name[iin++] = c;
            }
            else
            {
                IF_ASSERT(iiv >= iiv_len) break;
                value[iiv++] = c;
            }
        }

        free(input_name);
        free(value);
        free(data);
        free(path);
    }

    void draw()
    {
        for (int i=0; i<this->n_inputs; i++)
            if (this->inputs[i] != NULL)
                this->inputs[i]->draw();
    }

    void draw_text()
    {
        for (int i=0; i<this->n_inputs; i++)
            if (this->inputs[i] != NULL)
                this->inputs[i]->draw_text();
        this->draw_label();
    }

    void set_position(const Vec2& position)
    {
        this->position = position;
        Vec2 p = position;
        for (int i=0; i<this->n_inputs; i++)
        {
            this->inputs[i]->set_position(p);
            p.y -= this->inputs[i]->dims.y;
        }
    }

    int width() const
    {
        int w = -1;
        for (int i=0; i<this->n_inputs; i++)
            if (this->inputs[i]->dims.x > w)
                w = this->inputs[i]->dims.x;
        return w;
    }

    int height() const
    {
        int h = -1;
        for (int i=0; i<this->n_inputs; i++)
            if (this->inputs[i]->dims.y > h)
                h = this->inputs[i]->dims.y;
        return h;
    }

    virtual const char* name() = 0;
    virtual void submit() = 0;

    explicit Form(int n_inputs) :
        inputs(NULL), input_index(0), n_inputs(n_inputs), focused(-1),
        position(vec2_init(0))
    {
        IF_ASSERT(this->n_inputs <= 0) return;
        this->inputs = (InputBox**)calloc(this->n_inputs, sizeof(*this->inputs));
    }

    virtual ~Form()
    {
        if (this->inputs != NULL)
            for (int i=0; i<this->n_inputs; i++)
                delete this->inputs[i];
        free(this->inputs);
    }
};

class LoginForm: public Form
{
    public:
        InputTextBox* username;
        InputTextBox* password;
        InputCheckBox* remember;
        Button* submit_button;

    virtual const char* name()
    {
        return "Login";
    }

    virtual void added_to_screen()
    {
        this->load("credentials");
    }

    virtual void submit()
    {
        for (int i=0; i<this->n_inputs; i++)
            printf("%s: %s\n", this->inputs[i]->get_name(), this->inputs[i]->get_value());
        if (Auth::login(this->username->get_value(), this->password->get_value()))
        {
            if (this->remember->get_value()[0] == 't')
                this->save("credentials");
            else
                this->erase_save("credentials");
        }
    }

    LoginForm() :
        Form(4)
    {
        this->username = (InputTextBox*)this->register_text_input("username");
        this->username->save = true;
        this->password = (InputTextBox*)this->register_text_input("password");
        this->password->password = true;
        this->password->save = true;
        this->remember = (InputCheckBox*)this->register_checkbox_input("remember");
        this->remember->save = true;
        this->remember->set_value("true");
        this->submit_button = (Button*)this->register_button_input("submit");
        this->sanity_check();
    }

    virtual ~LoginForm() {}
};

class CreateAccountForm: public Form
{
    public:
        InputTextBox* username;
        InputTextBox* email;
        InputTextBox* password;
        InputCheckBox* remember;
        Button* submit_button;

    virtual const char* name()
    {
        return "Create Account";
    }

    virtual void submit()
    {
        for (int i=0; i<this->n_inputs; i++)
            printf("%s: %s\n", this->inputs[i]->get_name(), this->inputs[i]->get_value());
        if (Auth::create_account(this->username->get_value(),
                                 this->email->get_value(),
                                 this->password->get_value()))
        {
            if (this->remember->get_value()[0] == 't')
                this->save("credentials");
            else
                this->erase_save("credentials");
        }
    }

    CreateAccountForm() :
        Form(5)
    {
        this->username = (InputTextBox*)this->register_text_input("username");
        this->username->save = true;
        this->email = (InputTextBox*)this->register_text_input("email");
        this->password = (InputTextBox*)this->register_text_input("password");
        this->password->save = true;
        this->password->password = true;
        this->remember = (InputCheckBox*)this->register_checkbox_input("remember");
        this->remember->save = true;
        this->remember->set_value("true");
        this->submit_button = (Button*)this->register_button_input("submit");
        this->sanity_check();
    }

    virtual ~CreateAccountForm() {}
};


class FormScreen
{
    public:
        Form** forms;
        int n_forms;
        int max_forms;

    #define ARGS(A...) A

    #define FORWARD(METHOD, A, C) \
        void METHOD(A) \
        { \
            for (int i=0; i<this->n_forms; i++) \
                this->forms[i]->METHOD(C); \
        }

    FORWARD(click, ARGS(Vec2i cursor), ARGS(cursor));
    FORWARD(hover, ARGS(Vec2i cursor), ARGS(cursor));
    FORWARD(insert, ARGS(char c), ARGS(c));
    FORWARD(enter, ARGS(), ARGS());
    FORWARD(tab, ARGS(), ARGS());
    FORWARD(backspace, ARGS(), ARGS());
    FORWARD(cursor_right, ARGS(), ARGS());
    FORWARD(cursor_left, ARGS(), ARGS());
    FORWARD(draw, ARGS(), ARGS());
    FORWARD(draw_text, ARGS(), ARGS());

    #undef FORWARD
    #undef ARGS

    void add_form(Form* form)
    {
        IF_ASSERT(this->forms == NULL) return;
        IF_ASSERT(this->n_forms >= this->max_forms) return;
        this->forms[this->n_forms++] = form;
        form->added_to_screen();
    }

    void init(int max_forms)
    {
        this->max_forms = max_forms;
        IF_ASSERT(max_forms <= 0) return;
        this->forms = (Form**)calloc(max_forms, sizeof(*this->forms));
    }

    FormScreen() :
        forms(NULL), n_forms(0), max_forms(0)
    {
    }
};

}   // Hud
