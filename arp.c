#include "arp.h"

int in_arptable(struct arp_entry *arp_table, int no_entries, uint32_t ip) {
    for (int i = 0; i < no_entries; i++) {
        if (arp_table[i].ip == ip) return 1;
    }
    return 0;
}

// function that adds entry to the table once the request is made
void add_to_table(struct arp_entry *arp_table, int no_entries, uint32_t ip, uint8_t mac[6]) {
    struct arp_entry new_entry;
    new_entry.ip = ip;
    // memcpy(new_entry.mac, &mac, 6);
    for(int i = 0; i < 6; i++) new_entry.mac[i] = mac[i];
    arp_table[no_entries] = new_entry;
}

// get arp entry
struct arp_entry *get_arp_entry(struct arp_entry *arp_table, int no_entries, uint32_t ip) {
    for (int i = 0; i < no_entries; i++) {
    	if(arp_table[i].ip == ip) {
    		return &arp_table[i];
    	}
    }
    return NULL;
}

void print_arp(struct arp_entry *arp_table, int len) {
    for (int i = 0; i < len; i++) {
        struct in_addr in;
        in.s_addr = arp_table[i].ip;
        printf("%s ", inet_ntoa(in));
        for(int j = 0; j < 6; j++) {
            printf("%x ", arp_table[i].mac[j]);
        }
        printf("\n");
    }
}