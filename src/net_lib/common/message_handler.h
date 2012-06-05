#pragma once

typedef void (*pt2handler)(char*, int, int*, int);

void init_message_handler();

int process_packet_messages(char* buff, int *n, int max_n, int client_id);

int process_client_map_messages(char* buff, int *n, int max_n, int client_id);

int process_large_messages(char* buff, int *n, int max_n, int client_id);
void register_server_message_handler(int message_id, int size, pt2handler fptr);
void register_client_message_handler(int message_id, int size, pt2handler fptr);
