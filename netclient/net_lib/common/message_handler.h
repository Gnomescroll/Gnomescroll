#pragma once

#include <net_lib/common/type_pack.h>

//typedef int (*pt2handler)(int, char*);
typedef void (*pt2handler)(char*, int, int*, int);

//void default_handler_function(int message_id, char* buff);

void init_message_handler();
//void register_message_handler(int message_id, int size, pt2handler fptr);
//void process_packet_messages(char* buff, int n, int max_n, int client_id);
//int pop_message(char* buff, int *n, int max_n, int client_id);

int process_packet_messages(char* buff, int *n, int max_n, int client_id);

int process_python_messages(char* buff, int *n, int max_n, int client_id);
int process_large_messages(char* buff, int *n, int max_n, int client_id);
//typedef const void (*_pt2handler)(char*, int, int*);
void register_server_message_handler(int message_id, int size, pt2handler fptr);
void register_client_message_handler(int message_id, int size, pt2handler fptr);