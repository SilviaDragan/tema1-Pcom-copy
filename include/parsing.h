#pragma once
#include "skel.h"

struct route_table_entry {
	uint32_t prefix;
	uint32_t next_hop;
	uint32_t mask;
	int interface;
};

// typedef struct route_table {
// 	route_table_entry *rtable;
// 	int size;
// } *route_table;

struct route_table_entry *parse_route_table(char *file, int *size);
void print_table(struct route_table_entry *rtable, int size);
int compare(const void *a, const void *b);
struct route_table_entry *get_best_route(struct route_table_entry *rtable, int rtable_size, uint32_t dest_ip);
