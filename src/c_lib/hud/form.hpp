#pragma once

#include <hud/input.hpp>

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

    void check_all_loaded()
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
            case UI_INPUT_BUTTON:
                this->submit();
                break;
            case UI_INPUT_CHECKBOX:
                input->enter();
                break;
            case UI_INPUT_TEXT:
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

    virtual void save()
    {
        // write entered data to text file
    }

    virtual void load()
    {
        // load data from text file
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

    virtual void submit()
    {
        for (int i=0; i<this->n_inputs; i++)
            printf("%s: %s\n", this->inputs[i]->get_name(), this->inputs[i]->get_value());
    }

    LoginForm() :
        Form(4)
    {
        this->username = (InputTextBox*)this->register_text_input("username");
        this->password = (InputTextBox*)this->register_text_input("password");
        this->password->password = true;
        this->remember = (InputCheckBox*)this->register_checkbox_input("remember");
        this->submit_button = (Button*)this->register_button_input("submit");
        this->check_all_loaded();

        this->username->set_text("rdn");
        this->password->set_text("password");
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
    }

    CreateAccountForm() :
        Form(5)
    {
        this->username = (InputTextBox*)this->register_text_input("username");
        this->email = (InputTextBox*)this->register_text_input("email");
        this->password = (InputTextBox*)this->register_text_input("password");
        this->password->password = true;
        this->remember = (InputCheckBox*)this->register_checkbox_input("remember");
        this->submit_button = (Button*)this->register_button_input("submit");
        this->check_all_loaded();
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
