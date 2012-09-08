#include "server.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <c_lib/agent/agent.hpp>

namespace NetServer
{

unsigned int number_of_clients = 0;
unsigned int session_count = 0;
    
NetPeer** pool = NULL;
NetPeer** staging_pool = NULL;
class NetPeerManager** clients = NULL;

class Agent_state** agents = NULL;

class UserRecorder* users = NULL;

FILE* session_log_file = NULL;
FILE* population_log_file = NULL;

void assign_agent_to_client(int client_id, class Agent_state* a)
{
    GS_ASSERT(client_id >= 0 && client_id < HARD_MAX_CONNECTIONS);
    agents[client_id] = a;
}

void init_globals()
{
    GS_ASSERT(pool == NULL);
    GS_ASSERT(staging_pool == NULL);
    GS_ASSERT(clients == NULL);
    GS_ASSERT(agents == NULL);
    GS_ASSERT(users == NULL);
    pool = (NetPeer**)calloc(HARD_MAX_CONNECTIONS, sizeof(NetPeer*));
    staging_pool = (NetPeer**)calloc(HARD_MAX_CONNECTIONS, sizeof(NetPeer*));
    clients = (NetPeerManager**)calloc(HARD_MAX_CONNECTIONS, sizeof(NetPeerManager*));
    agents = (class Agent_state**)calloc(HARD_MAX_CONNECTIONS, sizeof(class Agent_state*));
    users = new UserRecorder;

    const char* time_str = get_time_str();

    // open sessions log file
    const char sessions_fn[] = "./log/sessions.log";
    session_log_file = fopen(sessions_fn, "a");
    GS_ASSERT(session_log_file != NULL);
    if (session_log_file != NULL)
        setvbuf(session_log_file, NULL, _IOLBF, 256);

    // open population log file
    const char pop_fn[] = "./log/population.log";
    population_log_file = fopen(pop_fn, "a");
    GS_ASSERT(population_log_file != NULL);
    // print startup time
    if (population_log_file != NULL)
    {
        setvbuf(population_log_file, NULL, _IOLBF, 256);
        fprintf(population_log_file, "%s Server started (build %d)\n", time_str, GS_VERSION);
    }
}

void teardown_globals()
{
    if (users != NULL) delete users;
    if (session_log_file != NULL) fclose(session_log_file);
    if (population_log_file != NULL) fclose(population_log_file);
    if (pool != NULL) free(pool);
    if (staging_pool != NULL) free(staging_pool);
    if (clients != NULL) free(clients);
    if (agents != NULL) free(agents);
}

class Session* begin_session(uint32_t ip_addr, int client_id)
{
    class Session* session = new Session(ip_addr);
    session->client_id = client_id;
    session_count++;
    session->id = session_count;
    session->login();
    printf("Client %03d connected. %02d clients connected\n", client_id, number_of_clients);
    GS_ASSERT(population_log_file != NULL);
    if (population_log_file != NULL)
    {
        const char* time_str = get_time_str();
        fprintf(population_log_file, "%s : Client %03d connected. %02d clients connected\n", time_str, client_id, number_of_clients);
        if (feof(population_log_file))
        {
            GS_ASSERT(false);
            fclose(population_log_file);
            population_log_file = NULL;
        }
    }
    return session;
}

void end_session(class Session* session)
{
    GS_ASSERT(session != NULL);
    if (session == NULL) return;
    session->logout();
    GS_ASSERT(session_log_file != NULL);
    if (session_log_file != NULL)
    {
        session->print(session_log_file); // unbuffered print to stdout
        if (feof(session_log_file))
        {
            GS_ASSERT(false);
            fclose(session_log_file);
            session_log_file = NULL;
        }
    }
    session->print(NULL);   // print to stdout
    printf("Client %d disconnected. %d clients connected\n", session->client_id, number_of_clients);
    GS_ASSERT(population_log_file != NULL);
    if (population_log_file != NULL)
    {
        const char* time_str = get_time_str();
        fprintf(population_log_file, "%s : Client %03d disconnected. %02d clients connected\n", time_str, session->client_id, number_of_clients);
        if (feof(population_log_file))
        {
            GS_ASSERT(false);
            fclose(population_log_file);
            population_log_file = NULL;
        }
    }
}

void client_authorized(int client_id, int user_id, time_t expiration_time, const char* username)
{
    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return;

    NetPeerManager* pm = clients[client_id];
    GS_ASSERT(pm != NULL);
    if (pm == NULL) return;

    // check if client is already authorized on another account
    // if so, kill that client
    // this prevents a user from being logged in multiple times
    // we kill the old client, because there's a good chance the original client is about to timeout
    // and we want to just let them back in, not wait for a timeout
    for (int i=0; i<HARD_MAX_CONNECTIONS; i++)
        if (clients[i] != NULL && clients[i] != pm && clients[i]->user_id == user_id)
        {
            ASSERT_VALID_CLIENT_ID(clients[i]->client_id);
            IF_INVALID_CLIENT_ID(clients[i]->client_id) continue;
            class NetPeer* peer = staging_pool[clients[i]->client_id];
            if (peer == NULL) peer = pool[clients[i]->client_id];
            GS_ASSERT(peer != NULL);
            if (peer == NULL) continue;
            kill_client(peer, DISCONNECT_LOGIN_ELSEWHERE);
        }

    pm->was_authorized(user_id, expiration_time, username);
}

void kill_client(int client_id, DisconnectType error_code)
{
    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return;
    GS_ASSERT(staging_pool != NULL);
    GS_ASSERT(pool != NULL);
    if (staging_pool == NULL || pool == NULL) return;
    
    class NetPeer* peer = staging_pool[client_id];
    if (peer == NULL) peer = pool[client_id];
    GS_ASSERT(peer != NULL);
    if (peer == NULL) return;

    kill_client(peer, error_code);
}

void check_client_authorizations()
{
    GS_ASSERT(pool != NULL);
    GS_ASSERT(staging_pool != NULL);
    if (pool == NULL || staging_pool == NULL) return;
    
    for (int i=0; i<HARD_MAX_CONNECTIONS; i++)
    {
        NetPeerManager* pm = clients[i];
        if (pm == NULL) continue;

        NetPeer* peer = staging_pool[i];
        if (peer == NULL) peer = pool[i];
        GS_ASSERT(peer != NULL);    // peer should not be NULL if manager is not NULL
        if (peer == NULL) continue;

        // remove peers who have authorized and it has expired,
        // or who have failed to authorize within a connection time limit
        DisconnectType disconnect_code = DISCONNECT_NONE;
        if (pm->authorization_expired())
            disconnect_code = DISCONNECT_AUTH_EXPIRED;
        else
        if (pm->failed_to_authorize())
            disconnect_code = DISCONNECT_AUTH_TIMEOUT;

        if (disconnect_code != DISCONNECT_NONE)
        {
            pm->authorized = false;
            kill_client(peer, disconnect_code);
        }
    }
}

}   // NetServer

