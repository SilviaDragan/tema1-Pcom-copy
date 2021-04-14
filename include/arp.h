#pragma once
#include "skel.h"

struct arp_entry {
	uint32_t ip;
	uint8_t mac[6];
};

int in_arptable(struct arp_entry* arp_table, int no_entries, uint32_t ip);
void add_to_table(struct arp_entry* arp_table, int no_entries, uint32_t ip, uint8_t mac[6]);
struct arp_entry *get_arp_entry(struct arp_entry *arp_table, int no_entries, uint32_t ip);
void print_arp(struct arp_entry *arp_table, int len);


