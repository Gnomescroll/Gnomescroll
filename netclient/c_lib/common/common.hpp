#pragma once

void address_from_string(char* ip, int address[4]);

// strcpy but dont add \0 to the end of a
void strcpy_no_null(char* dest, char* src);

unsigned int sanitize_player_name(char* name);
