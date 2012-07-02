#pragma once

#include <common/common.hpp>

const int USER_RECORDER_INITIAL_MAX_IP_ADDR = 256;
const int USER_INITIAL_MAX_SESSIONS = 2;
const int SESSION_INITIAL_MAX_NAMES = 2;

class Session
{
    private:
        void grow_names()
        {
            GS_ASSERT(this->max_names > 0)
            this->max_names *= 2;
            this->names = (char**)realloc(this->names, this->max_names * sizeof(char*));
            for (int i=this->n_names; i<this->max_names; this->names[i++] = NULL);
        }
        
    public:
        uint32_t ip_addr;
        int client_id;
        char** names;
        int n_names;
        int max_names;
        time_t login_time;
        time_t logout_time;
        int number; // per user basis
        int id; // global basis
        int version;

    void login()
    {
        GS_ASSERT(this->client_id >= 0);
        this->login_time = time(NULL);        
        printf("Session began at %ld\n", this->login_time);
    }

    void logout()
    {
        this->logout_time = time(NULL);
    }

    bool is_active()
    {
        return (this->logout_time == 0);
    }

    // returns true if the file is fine
    void print(FILE* f)
    {
        if (f == NULL) f = stdout;
        
        fprintf(f, "Session %d; ", this->id);

        // version
        fprintf(f, "Version %d; ", this->version);
        
        // time
        fprintf(f, "%ld seconds; ", this->logout_time - this->login_time);

        // addr
        uint8_t address[4];
        address_from_uint32(this->ip_addr, address);
        fprintf(f, "IP %d.%d.%d.%d; ", address[0], address[1], address[2], address[3]);

        // count
        fprintf(f, "Login %d; ", this->number+1);

        //client id
        fprintf(f, "Client id %d; ", this->client_id);

        // names
        fprintf(f, "Names: ");
        for (int i=0; i<this->n_names; i++)
        {
            GS_ASSERT(this->names[i] != NULL);
            if (this->names[i] == NULL) continue;
            fprintf(f, "%s ", this->names[i]);
        }

        fprintf(f, "\n");
    }

    void add_name(char* name)
    {
        // check if the latest name is this name
        if (this->n_names > 0 && strcmp(this->names[this->n_names-1], name) == 0)
            return;
        
        if (this->n_names >= this->max_names)
            this->grow_names();
        GS_ASSERT(this->n_names < this->max_names);
        
        this->names[this->n_names] = (char*)malloc(sizeof(char) * (strlen(name) + 1));
        strcpy(this->names[this->n_names], name);
        this->n_names++;
    }

    Session(uint32_t ip_addr)
    : ip_addr(ip_addr), client_id(-1), n_names(0), max_names(SESSION_INITIAL_MAX_NAMES),
    login_time(0), logout_time(0), number(-1), id(-1), version(0)
    {
        GS_ASSERT(this->max_names > 0);
        this->names = (char**)calloc(this->max_names, sizeof(char*));
    }

    ~Session()
    {
        if (names != NULL)
        {
            for (int i=0; i<n_names; i++)
                if (names[i] != NULL)
                    free(names[i]);
            free(names);
        }
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

    class Session* get_latest_session(int client_id)
    {
        if (this->n_sessions <= 0) return NULL;
        for (int i=this->n_sessions-1; i>=0; i--)
        {
            GS_ASSERT(this->sessions[i] != NULL);
            if (this->sessions[i] == NULL) continue;
            if (this->sessions[i]->client_id == client_id) return this->sessions[i];
        }
        return NULL;
    }
        
    User(uint32_t ip_addr)
    : max_sessions(USER_INITIAL_MAX_SESSIONS), n_sessions(0),
    ip_addr(ip_addr)
    {
        GS_ASSERT(this->max_sessions > 0);
        this->sessions = (class Session**)calloc(max_sessions, sizeof(class Session*));
    }

    ~User()
    {
        if (sessions != NULL)
        {
            for (int i=0; i<n_sessions; i++)
                delete sessions[i];
            free(sessions);
        }
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
            GS_ASSERT(this->users[i] != NULL);
            if (this->users[i] == NULL) continue;
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

        GS_ASSERT(this->users != NULL);
        if (this->users == NULL) return;
        
        GS_ASSERT(session != NULL);
        if (session == NULL) return;
        
        GS_ASSERT(session->ip_addr != 0);
        if (session->ip_addr == 0) return;

        // find an existing user
        class User* user = this->get_or_create_user(session->ip_addr);
        user->add_session(session);
    }

    class Session* get_active_session_for_client(int client_id)
    {
        for (int i=0; i<this->n_users; i++)
        {
            GS_ASSERT(this->users[i] != NULL);
            if (this->users[i] == NULL) continue;
            class Session* session = this->users[i]->get_latest_session(client_id);
            if (session == NULL) continue;
            GS_ASSERT(session->client_id == client_id);
            if (session->client_id != client_id) continue;
            if (!session->is_active()) continue;
            return session;
        }
        return NULL;
    }

    void add_name_to_client_id(int client_id, char* name)
    {
        class Session* session = get_active_session_for_client(client_id);
        GS_ASSERT(session != NULL);
        if (session == NULL) return;
        GS_ASSERT(session->is_active());
        if (!session->is_active()) return;
        session->add_name(name);
    }

    void record_client_version(int client_id, int version)
    {
        class Session* session = get_active_session_for_client(client_id);
        GS_ASSERT(session != NULL);
        if (session == NULL) return;
        GS_ASSERT(session->is_active());
        if (!session->is_active()) return;
        session->version = version;
    }

    UserRecorder()
    : users(NULL), n_users(0), max_users(USER_RECORDER_INITIAL_MAX_IP_ADDR)
    {
        this->max_users = max_users;
        this->users = (class User**)calloc(this->max_users, sizeof(class User*));
    }

    ~UserRecorder()
    {
        if (users != NULL)
        {
            for (int i=0; i<this->n_users; i++)
            {
                GS_ASSERT(this->users[i] != NULL);
                if (this->users[i] == NULL) continue;
                delete this->users[i];
            }
            free(users);
        }
    }
};
