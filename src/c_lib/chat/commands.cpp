#include "commands.hpp"

#include <agent/net_agent.hpp>

namespace Chat
{

/****************************
 ********* COMMANDS *********
 ****************************/

void cmd_chaton(const char* cmd, size_t cmdlen, char* args, size_t argslen)
{
    Options::player_chat = true;
}

void cmd_chatoff(const char* cmd, size_t cmdlen, char* args, size_t argslen)
{
    Options::player_chat = false;
}

void cmd_die(const char* cmd, size_t cmdlen, char* args, size_t argslen)
{
    killme_CtoS msg;
    msg.send();
}

void cmd_color(const char* cmd, size_t cmdlen, char* args, size_t argslen)
{
    const char* usage = "Usage: /color R G B (R G B must be between 0 and 255)";

    Color color = Color(0,0,0);
    bool valid = true;

    // split lines up
    int i = 0;
    int n = 1;
    const char delim = ' ';
    char c = '\0';
    while ((c = args[i++]) != '\0')
    {
        if (!isdigit(c) && c != delim)
            break;
        if (c == delim)
        {
            args[i-1] = '\0';
            n++;
        }
    }
    i = 0;

    if (c != '\0') valid = false;    // encountered invalid char
    if (n != 3) valid = false;       // invalid arg count

    if (!valid)
    {
        chat_client->send_system_message(usage);
        return;
    }

    char buf[3+1] = {'\0'};

    int j = 0;  // copy red
    while ((c = args[i++]) != '\0' && j < 3)
        buf[j++] = c;
    buf[j] = '\0';
    if (args[i-1] != '\0') valid = false;
    else
    {
        int r = atoi(buf);
        if (r > 0xFF) valid = false;
        else color.r = r;
    }
    printf("r: %s\n", buf);

    j = 0;      // copy green
    while ((c = args[i++]) != '\0' && j < 3)
        buf[j++] = c;
    buf[j] = '\0';
    if (args[i-1] != '\0') valid = false;
    else
    {
        int g = atoi(buf);
        if (g > 0xFF) valid = false;
        else color.g = g;
    }
    printf("g: %s\n", buf);

    j = 0;      // copy blue
    while ((c = args[i++]) != '\0' && j < 3)
        buf[j++] = c;
    buf[j] = '\0';
    if (args[i-1] != '\0') valid = false;
    else
    {
        int b = atoi(buf);
        if (b > 0xFF) valid = false;
        else color.b = b;
    }
    printf("b: %s\n", buf);

    if (!valid)
    {
        chat_client->send_system_message(usage);
        return;
    }

    class Agents::Agent* you = ClientState::player_agent.you();
    if (you != NULL && colors_equal(you->status.color, color))
    {
        static const char msgfmt[] = "Your color is already %d %d %d";
        static const size_t msg_len = sizeof(msgfmt) + 3*3 - 3*2;
        static char msg[msg_len+1] = {'\0'};

        snprintf(msg, msg_len+1, msgfmt, color.r, color.g, color.b);
        chat_client->send_system_message(msg);
    }

    colorme_CtoS msg;
    msg.color = color;
    msg.send();
}


void cmd_url(const char* cmd, size_t cmdlen, char* args, size_t argslen)
{
    if (!argslen) printf("Usage: /url <url to navigate to>");
    else Awesomium::open_url(args);
}

void cmd_home(const char* cmd, size_t cmdlen, char* args, size_t argslen)
{
    gohome_CtoS msg;
    msg.send();
}

void cmd_item(const char* cmd, size_t cmdlen, char* args, size_t arglen)
{
    #if !PRODUCTION
    const char USAGE[] = "/%s <item_name> [<stack_size>]";

    char item_name[DAT_NAME_MAX_LENGTH+1] = {'\0'};
    int i = 0;
    char c = '\0';
    while ((c = args[i]) && !isspace(c) && i <= DAT_NAME_MAX_LENGTH)
        item_name[i++] = c;
    item_name[i] = '\0';

    if (!i)
    {
        chat_client->send_system_messagef(USAGE, cmd);
        return;
    }

    int stack = 1;
    char stack_size[11+1] = {'\0'};
    if (c != '\0')
    {
        while ((c = args[i]) && isspace(c)) i++;
        if (c != '\0')
        {
            int j = 0;
            while ((c = args[i]) && !isspace(c) && j <= 11)
            {
                i++;
                stack_size[j++] = c;
            }
            stack_size[j] = '\0';
        }
        bool err = false;
        long long int _stack = parse_int(stack_size, err);
        if (err)
        {
            chat_client->send_system_messagef("Invalid stack size: %s", stack_size);
            return;
        }
        else
        {
            stack = (int)_stack;
            if (_stack > MAX_STACK_SIZE)
                stack = MAX_STACK_SIZE;
            if (_stack <= 0)
                stack = 1;
        }
    }

    if (!Item::request_item_create(item_name, stack))
        chat_client->send_system_messagef("Unknown item: %s", item_name);
    #endif
}

/****************************
 ******* REGISTRATION *******
 ****************************/

size_t n_commands = 0;
struct ChatCommand commands[MAX_COMMANDS];

static void add_command(const char* cmd, chatCommand action)
{
    IF_ASSERT(n_commands >= MAX_COMMANDS) return;
    GS_ASSERT(cmd[0] != '\0');
    GS_ASSERT(strlen(cmd) <= COMMAND_MAX_LENGTH);

    commands[n_commands].action = action;
    strncpy(commands[n_commands].name, cmd, COMMAND_MAX_LENGTH+1);
    commands[n_commands].name[COMMAND_MAX_LENGTH] = '\0';

    n_commands++;
}

static void verify_command_conf()
{
    GS_ASSERT(n_commands <= MAX_COMMANDS);
    if (!n_commands) return;

    // all commands have a name or action
    for (size_t i=0; i<n_commands; i++)
    {
        GS_ASSERT(commands[i].action != NULL);
        GS_ASSERT(commands[i].name[0] != '\0');
    }

    // no duplicate commands registered
    for (size_t i=0; i<n_commands-1; i++)
    for (size_t j=i+1; j<n_commands; j++)
        GS_ASSERT(strcmp(commands[i].name, commands[j].name) != 0);
}

void register_chat_commands()
{
    add_command("chaton", cmd_chaton);
    add_command("chatoff", cmd_chatoff);
    add_command("kill", cmd_die);
    add_command("die", cmd_die);
    add_command("color", cmd_color);
    add_command("colour", cmd_color);
    add_command("url", cmd_url);
    add_command("home", cmd_home);
    add_command("item", cmd_item);

    verify_command_conf();
}

}   // Chat
