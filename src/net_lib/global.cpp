#include "global.hpp"

#include <c_lib/agent/agent.hpp>

NetMessageArray_pool net_message_array_pool;

namespace NetClient
{

NetPeer Server;

}   // NetClient

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
        fprintf(population_log_file, "%s Server started (build %d)\n", time_str, DC_VERSION);
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

}   // NetServer
