#pragma once

typedef void (*pt2handler_client)(char*, unsigned int, unsigned int* read_bytes);
typedef void (*pt2handler_server)(char*, unsigned int, unsigned int* read_bytes, ClientID client_id);

void init_message_handler();

#if DC_SERVER
int process_packet_messages(char* buff, unsigned int* n, unsigned int max_n, ClientID client_id);
int process_client_map_messages(char* buff, unsigned int* n, unsigned int max_n, ClientID client_id);
int process_large_messages(char* buff, unsigned int* n, unsigned int max_n, ClientID client_id);
#endif
#if DC_CLIENT
int process_packet_messages(char* buff, unsigned int* n, unsigned int max_n);
int process_client_map_messages(char* buff, unsigned int* n, unsigned int max_n);
int process_large_messages(char* buff, unsigned int* n, unsigned int max_n);
#endif

void register_server_message_handler(int message_id, unsigned int size, pt2handler_server fptr);
void register_client_message_handler(int message_id, unsigned int size, pt2handler_client fptr);
