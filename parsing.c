#include "parsing.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void print_table(struct route_table_entry *table, int size) {
	printf("printing table...\n" );

	for (int i = 0; i < size; i++) {
		struct in_addr in;
		in.s_addr = table[i].prefix;
		printf("%s ", inet_ntoa(in));

		in.s_addr = table[i].next_hop;
		printf("%s ", inet_ntoa(in));

		in.s_addr = table[i].mask;
		printf("%s %d\n", inet_ntoa(in), table[i].interface);

	}
}

struct route_table_entry *get_best_route(struct route_table_entry *rtable, int rtable_size, uint32_t dest_ip) {
	int best_index = -1;
	for(int i = 0; i < rtable_size; i++) {
		if( (rtable[i].mask & dest_ip) == rtable[i].prefix) {
			if(best_index == -1 || ntohl(rtable[i].mask) > ntohl(rtable[best_index].mask)) {
				best_index = i;
			// return &rtable[i];
			}

		}
	}
	if(best_index > 0) {
		return &rtable[best_index];
	}
	else 
	return NULL;
}

int compare(const void *a, const void *b) {
	return ((struct route_table_entry *)a)->mask - ((struct route_table_entry *)b)->mask;

}
// consider remaking this for efficiency
// mske the list of entries into a sort of hash-table
// the key being a mask

struct route_table_entry *parse_route_table(char *file, int *size) {

	FILE *input = fopen(file, "r");
	int entries = 0, i = 0;
	size_t len = 0;
	size_t nread, n;
	char *line = NULL;

	while ( (n = getline(&line, &len, input)) != -1) {
		entries++;
	}
	rewind(input);

	struct route_table_entry *rtable = (struct route_table_entry *)malloc(sizeof(struct route_table_entry)*entries);

	while ( (nread = getline(&line, &len, input)) != -1) {
        // printf("%s", line);
		struct route_table_entry entry;

        char *token = strtok(line, " ");
        uint32_t prefix = inet_addr(token);
        token = strtok(NULL, " ");
        uint32_t nhop = inet_addr(token);
        token = strtok(NULL, " ");
        uint32_t mask = inet_addr(token);
        token = strtok(NULL, " ");
        int interface = atoi(token);

        entry.prefix = prefix;
        entry.next_hop = nhop;
        entry.mask = mask;
        entry.interface = interface;
       	
       	// printf("%d %d %d %d\n", entry->prefix, entry->next_hop, entry->mask, entry->interface);

		rtable[i++] = entry;
	}

	// print_table(rtable, entries);
	qsort(rtable, entries, sizeof(struct route_table_entry), compare);
	// print_table(rtable, entries);
	free(line);
    fclose(input);

	*size = entries;
	return rtable;
}
