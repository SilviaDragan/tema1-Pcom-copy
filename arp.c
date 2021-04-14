#include "arp.h"

int in_arptable(arp_entry *arp_table, int no_entries, uint32_t ip) {
    for (int i = 0; i < no_entries; i++) {
        if (arp_table[i]->ip == ip) return 1;
    }
    return 0;
}

// function that adds entry to the table once the request is made
void add_to_table(arp_entry *arp_table, int no_entries, uint32_t ip, uint8_t mac[6]) {
    arp_entry new_entry = malloc(sizeof(arp_entry));
    new_entry->ip = ip;
    memcpy(new_entry->mac, &mac, 6);
    // for(int i = 0; i < 6; i++) new_entry->mac[i] = mac[i];
    // new_entry->mac = mac;
    arp_table[no_entries] = new_entry;
}

// get arp entry
arp_entry *get_arp_entry(arp_entry *arp_table, int no_entries, uint32_t ip) {
    for (int i = 0; i < no_entries; i++) {
    	if(arp_table[i]->ip == ip) {
    		return &arp_table[i];
    	}
    }
    return NULL;
}