#pragma once

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
        int id; // on a per-user basis

    void login()
    {
        GS_ASSERT(this->client_id >= 0);
        this->login_time = time(NULL);        
        printf("Session began at %ld\n", this->login_time);
    }

    void logout()
    {
        this->logout_time = time(NULL);
        printf("Session %d ended at %ld. Lasted %ld seconds\n", this->id+1, this->logout_time, this->logout_time - this->login_time);
    }

    void print()
    {
        // TODO
    }

    void add_name(char* name)
    {
        if (this->n_names >= this->max_names)
            this->grow_names();
        GS_ASSERT(this->n_names < this->max_names);
        
        this->names[this->n_names] = (char*)malloc(sizeof(char) * (strlen(name) + 1));
        strcpy(this->names[this->n_names], name);
        this->n_names++;
    }

    Session(uint32_t ip_addr)
    : ip_addr(ip_addr), client_id(-1), n_names(0), max_names(SESSION_INITIAL_MAX_NAMES),
    id(-1)
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

class Session* begin_session(uint32_t ip_addr, int client_id)
{
    class Session* session = new Session(ip_addr);
    session->client_id = client_id;
    session->login();
    return session;
}

void end_session(class Session* session)
{
    GS_ASSERT(session != NULL);
    if (session == NULL) return;
    session->logout();
}

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
        session->id = this->n_sessions;
        this->n_sessions++;
    }

    class Session* get_current_session()
    {
        if (this->n_sessions <= 0) return NULL;
        return this->sessions[this->n_sessions-1];
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
