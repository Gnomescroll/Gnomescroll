#pragma once

typedef void (*pt2handler)(char*, unsigned int, unsigned int* read_bytes, unsigned int client_id);

void init_message_handler();

int process_packet_messages(char* buff, unsigned int* n, unsigned int max_n, unsigned int client_id);
int process_client_map_messages(char* buff, unsigned int* n, unsigned int max_n, unsigned int client_id);
int process_large_messages(char* buff, unsigned int* n, unsigned int max_n, unsigned int client_id);

void register_server_message_handler(int message_id, unsigned int size, pt2handler fptr);
void register_client_message_handler(int message_id, unsigned int size, pt2handler fptr);
