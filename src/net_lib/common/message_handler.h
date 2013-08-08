#pragma once

typedef void (*pt2handler_client)(char*, size_t, size_t* read_bytes);
typedef void (*pt2handler_server)(char*, size_t, size_t* read_bytes, ClientID client_id);

void init_message_handler();
void teardown_message_handler();

#if DC_SERVER
int process_packet_messages(char* buff, size_t* n, size_t max_n, ClientID client_id);
int process_client_map_messages(char* buff, size_t* n, size_t max_n, ClientID client_id);
int process_large_messages(char* buff, size_t* n, size_t max_n, ClientID client_id);
#endif
#if DC_CLIENT
int process_packet_messages(char* buff, size_t* n, size_t max_n);
int process_client_map_messages(char* buff, size_t* n, size_t max_n);
int process_large_messages(char* buff, size_t* n, size_t max_n);
#endif

void register_server_message_handler(int message_id, size_t size, pt2handler_server fptr, const char* name);
void register_client_message_handler(int message_id, size_t size, pt2handler_client fptr, const char* name);

const char* get_server_packet_name(uint8_t message_id);
const char* get_client_packet_name(uint8_t message_id);
