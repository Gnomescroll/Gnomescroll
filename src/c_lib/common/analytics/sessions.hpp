#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <common/common.hpp>

const int USER_RECORDER_INITIAL_MAX_IP_ADDR = 256;
const int USER_INITIAL_MAX_SESSIONS = 2;

class Session
{
    public:
        uint32_t ip_addr;

        ClientID client_id;
        int user_id;
        char* username;

        time_t login_time;
        time_t logout_time;

        int number; // session count per user
        int id; // global session count

        int version;    // version sent by client
        bool killed;    // was force killed

    void login()
    {
        GS_ASSERT(this->client_id >= 0);
        this->login_time = utc_now();
        printf("Session began at %ld\n", this->login_time);
    }

    void logout()
    {
        this->logout_time = utc_now();
    }

    bool is_active()
    {
        return (this->logout_time == 0);
    }

    // returns true if the file is fine
    void print(FILE* f)
    {
        if (f == NULL) f = stdout;

        fprintf(f, "Session %04d; ", this->id);

        // version
        fprintf(f, "Version %04d; ", this->version);

        // time
        fprintf(f, "%05d seconds; ", (int)difftime(this->logout_time, this->login_time));

        // addr
        uint8_t address[4];
        address_from_uint32(this->ip_addr, address);
        fprintf(f, "IP %d.%d.%d.%d; ", address[0], address[1], address[2], address[3]);

        // count
        fprintf(f, "Login %d; ", this->number+1);

        //client id
        fprintf(f, "Client id %d; ", this->client_id);

        // username
        if (this->username == NULL)
            fprintf(f, "username was not set");
        else
            fprintf(f, "username: %s", this->username);

        // was killed
        if (this->killed)
            fprintf(f, "; NOTE: Was forcibly disconnected");

        fprintf(f, "\n");
    }

    void set_name(const char* username)
    {
        IF_ASSERT(this->username != NULL) free(this->username);
        size_t len = strlen(username);
        this->username = (char*)malloc((len+1)*sizeof(char));
        strcpy(this->username, username);
    }

    Session(uint32_t ip_addr) :
        ip_addr(ip_addr),
        client_id(NULL_CLIENT), user_id(0),
        username(NULL),
        login_time(0), logout_time(0),
        number(-1), id(-1), version(0),
        killed(false)
    {}

    ~Session()
    {
        free(this->username);
    }
};

class User
{
    private:
        int max_sessions;

    void grow_sessions()
    {
        GS_ASSERT(this->max_sessions > 0);
        this->max_sessions *= 2;
        this->sessions = (class Session**)realloc(this->sessions, this->max_sessions * sizeof(class Session*));
        for (int i=this->n_sessions; i<this->max_sessions; this->sessions[i++] = NULL);
    }

    public:
        class Session** sessions;
        int n_sessions;
        uint32_t ip_addr;

    void add_session(class Session* session)
    {
        if (this->n_sessions >= max_sessions)
            this->grow_sessions();
        GS_ASSERT(this->n_sessions < this->max_sessions);

        this->sessions[this->n_sessions] = session;
        session->number = this->n_sessions;
        this->n_sessions++;
    }

    class Session* get_latest_session(ClientID client_id)
    {
        if (this->n_sessions <= 0) return NULL;
        for (int i=this->n_sessions-1; i>=0; i--)
        {
            IF_ASSERT(this->sessions[i] == NULL) continue;
            if (this->sessions[i]->client_id == client_id) return this->sessions[i];
        }
        return NULL;
    }

    User(uint32_t ip_addr) :
        max_sessions(USER_INITIAL_MAX_SESSIONS), n_sessions(0),
        ip_addr(ip_addr)
    {
        GS_ASSERT(this->max_sessions > 0);
        this->sessions = (class Session**)calloc(max_sessions, sizeof(class Session*));
    }

    ~User()
    {
        if (sessions != NULL)
            for (int i=0; i<n_sessions; i++)
                delete sessions[i];
        free(sessions);
    }
};

class UserRecorder
{
    private:
        void grow_users()
        {
            GS_ASSERT(this->max_users > 0);
            this->max_users *= 2;
            this->users = (class User**)realloc(this->users, sizeof(class User*) * this->max_users);
            for (int i=this->n_users; i<this->max_users; this->users[i++] = NULL);
        }

    public:
        class User** users;
        int n_users;
        int max_users;

    // returns id of ip addr
    class User* get_or_create_user(uint32_t ip_addr)
    {
        // look for user with ip_addr
        class User* user = this->get_user(ip_addr);
        if (user != NULL) return user;

        // grow
        if (this->n_users >= this->max_users)
            this->grow_users();

        // else, create
        user = new User(ip_addr);
        this->users[this->n_users] = user;
        this->n_users++;
        return user;
    }

    class User* get_user(uint32_t ip_addr)
    {
        for (int i=0; i<this->n_users; i++)
        {
            IF_ASSERT(this->users[i] == NULL) continue;
            if (this->users[i]->ip_addr == ip_addr)
                return this->users[i];
        }
        return NULL;
    }

    void assign_session_to_user(class Session* session)
    {
        // find user with ip
        // if none found, create
        // attach

        IF_ASSERT(this->users == NULL) return;
        IF_ASSERT(session == NULL) return;
        IF_ASSERT(session->ip_addr == 0) return;

        // find an existing user
        class User* user = this->get_or_create_user(session->ip_addr);
        user->add_session(session);
    }

    class Session* get_active_session_for_client(ClientID client_id)
    {
        for (int i=0; i<this->n_users; i++)
        {
            IF_ASSERT(this->users[i] == NULL) continue;
            class Session* session = this->users[i]->get_latest_session(client_id);
            if (session == NULL) continue;
            IF_ASSERT(session->client_id != client_id) continue;
            if (!session->is_active()) continue;
            return session;
        }
        return NULL;
    }

    void set_name_for_client_id(ClientID client_id, const char* username)
    {
        class Session* session = get_active_session_for_client(client_id);
        IF_ASSERT(session == NULL) return;
        IF_ASSERT(!session->is_active()) return;
        session->set_name(username);
    }

    void record_client_force_disconnect(ClientID client_id)
    {
        class Session* session = get_active_session_for_client(client_id);
        IF_ASSERT(session == NULL) return;
        IF_ASSERT(!session->is_active()) return;
        session->killed = true;
    }

    void record_client_version(ClientID client_id, int version)
    {
        class Session* session = get_active_session_for_client(client_id);
        IF_ASSERT(session == NULL) return;
        IF_ASSERT(!session->is_active()) return;
        session->version = version;
    }

    UserRecorder() :
        users(NULL), n_users(0), max_users(USER_RECORDER_INITIAL_MAX_IP_ADDR)
    {
        this->users = (class User**)calloc(this->max_users, sizeof(class User*));
    }

    ~UserRecorder()
    {
        if (users != NULL)
            for (int i=0; i<this->n_users; i++)
            {
                GS_ASSERT(this->users[i] != NULL);
                delete this->users[i];
            }
        free(users);
    }
};
