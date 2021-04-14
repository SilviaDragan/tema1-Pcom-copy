#pragma once
#include "skel.h"

typedef struct arp_entry {
	uint32_t ip;
	uint8_t mac[6];
} *arp_entry;

int in_arptable(arp_entry* arp_table, int no_entries, uint32_t ip);
void add_to_table(arp_entry* arp_table, int no_entries, uint32_t ip, uint8_t mac[6]);
arp_entry *get_arp_entry(arp_entry *arp_table, int no_entries, uint32_t ip);


