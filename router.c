#include <queue.h>
#include "skel.h"
#include "parsing.h"
#include "arp.h"

int main(int argc, char *argv[])
{
	setvbuf( stdout , NULL , _IONBF , 0) ;
	packet m;
	int rc;
	init(argc - 2, argv + 2);

	int rtable_size = 0;
	struct route_table_entry *rtable = parse_route_table(argv[1], &rtable_size);
	// print_table(rtable, rtable_size);
	// printf("%d\n", rtable_size);

	int arpt_entries = 0;
	struct arp_entry *arp_table = malloc(sizeof(struct arp_entry)*100);
	if(! arp_table) return -1;


	// initilase queue;
	queue q = queue_create();

	// add element in q: queue_enq (q , packet ) ;

	while (1) {
		rc = get_packet(&m);
		DIE(rc < 0, "get_message");
		/* Students will write code here */

		// extract ether header
		struct ether_header *ether_hdr = (struct ether_header *)m.payload;
		struct iphdr *ip_hdr = (struct iphdr *)(m.payload + sizeof(struct ether_header));
		struct arp_header *arp_hdr = parse_arp(m.payload);
		struct icmphdr *icmp_header = parse_icmp(m.payload);
		
		

		// get the mac address of the interface the packet came through
		uint8_t *if_mac = calloc(6, sizeof(uint8_t));
		get_interface_mac(m.interface, if_mac);

		// if (icmp_header) {
		// 	if (icmp_header->type == ICMP_ECHO) {
		// 		// verific daca e pt router
		// 		if(ip_hdr->daddr == inet_addr(get_interface_ip(m.interface))) {
		// 			send_icmp(ip_hdr->saddr, ip_hdr->daddr, if_mac, ether_hdr->ether_shost, ICMP_ECHOREPLY, icmp_header->code, m.interface, icmp_header->un.echo.id, icmp_header->un.echo.sequence );
		// 			continue;
		// 		}
				
		// 	}
		// }
		// if (if_mac != ether_hdr->ether_dhost && if_mac != 0xffffff) {
		// 	// drop packet
		// 	continue;
		// }
		//check if arp
		if(arp_hdr) {
			// chech if request or reply
			if(htons(arp_hdr->op) == ARPOP_REQUEST) {
				printf("request\n");
				memcpy(ether_hdr->ether_dhost, ether_hdr->ether_shost, 6);
				memcpy(ether_hdr->ether_shost, if_mac, 6);

				send_arp(arp_hdr->spa, arp_hdr->tpa, ether_hdr, m.interface,  htons(ARPOP_REPLY));
			}
			else if (htons(arp_hdr->op) == ARPOP_REPLY) {
				// printf("reply\n");
				// cauta in tabela arp
				// daca nu gasesc, adauga
				// trimite arp request 
				if (in_arptable(arp_table, arpt_entries, arp_hdr->spa) == 0) {
					// printf("da \n");
					add_to_table(arp_table, arpt_entries, arp_hdr->spa, arp_hdr->sha);
					arpt_entries++;	
					print_arp(arp_table, arpt_entries);
				}

				// ia pachetul din coada si trimite-l acum
				// completeaza headerul eth, pune la sursa adresa pe care ai primit-o; 
				// trimite pachetul cu functia int send_packet(int interface, packet *m);
				// verifica daca host ul la care trebuie sa trimit este local sau nu
				// gasesc adresa IP unde trb sa trimit (find best route)
				// daca nu gaseste, idk

				if (queue_empty(q)) {
					printf("coada goala\n");
					continue;
				}
				uint8_t var[6];
				memcpy(var, ether_hdr->ether_shost, 6);
				// printf("if mac: ");
				// for(int i =0; i<6; i++) {
				// 	printf("%x ", if_mac[i]);					

				// }
				// printf("\n");

				packet to_send = *(packet *)queue_deq(q);
				ether_hdr = (struct ether_header *)(to_send.payload);
				// for(int i =0; i<6; i++) {
				// 	printf("%x ", ether_hdr->ether_dhost[i]);					

				// }
				// printf("\n");

				// for(int i =0; i<6; i++) {
				// 	printf("%x ", ether_hdr->ether_shost[i]);
				// }
				// printf("\n");
				memcpy(ether_hdr->ether_dhost, var, 6);

				get_interface_mac(m.interface, if_mac);

				memcpy(ether_hdr->ether_shost, if_mac, 6);

				


				send_packet(m.interface, &to_send);
			}
		}
		else {

			uint16_t my_check = ip_hdr->check;
			ip_hdr->check = 0;


			// check if checksum is correct
			if (ip_checksum(&ip_hdr, sizeof(struct iphdr)) != my_check) {
				printf("wrong checksum, drop packet");
				continue;
			}
			// update checksum
			ip_hdr->check = ip_checksum(ip_hdr ,sizeof(struct iphdr));
			
			ip_hdr->ttl--;

			// check if ttl is > 0
			if (ip_hdr->ttl < 1) {
				printf("ttl < 1, dropping packet\n");
				// host unreachable
				send_icmp_error(ip_hdr->saddr, ip_hdr->daddr, ether_hdr->ether_dhost, ether_hdr->ether_shost, ICMP_TIME_EXCEEDED, 0, m.interface);
				continue;
			}
			//decrement ttl
			
			

			struct route_table_entry *nh = get_best_route(rtable, rtable_size, ip_hdr->daddr);
			if (!nh) {
				printf("drop packet");
				// send icmp message host unreachable
				send_icmp_error(ip_hdr->saddr, ip_hdr->daddr, ether_hdr->ether_dhost, ether_hdr->ether_shost, ICMP_UNREACH, 0, m.interface);
				
				continue;
			}
			// make the source the interface mac address
			// check if we know the mac adress locally
			// search for it in the arp table
			struct arp_entry *aentry = get_arp_entry(arp_table, arpt_entries, ip_hdr->daddr);
			if (!aentry) {
				printf("aici\n");
				// the mac address of the next hop is not known
				// send arp request to find it
				// push packet in queue

				// MESAJUL PE CARE IL PUN IN COADA SA FIE POINTER
				// memcpy(pachet nou, adresa m, sizeof m) + alocare.

				packet *remember_me = malloc(sizeof(packet));
				memcpy(remember_me, &m, sizeof(packet));

				queue_enq(q, remember_me);

				// get the interface mac addr of the next hop

				uint8_t *nh_imac = calloc(6, sizeof(uint8_t));
				get_interface_mac(nh->interface, nh_imac);
				memcpy(ether_hdr->ether_shost, nh_imac, 6);
				for (int i =0; i<6; i++)
					ether_hdr->ether_dhost[i] = 0xff;
				// memcpy(ether_hdr->ether_dhost, 0xff, 6);
				ether_hdr->ether_type = htons(ETHERTYPE_ARP);

				uint32_t if_ip = inet_addr(get_interface_ip(nh->interface));
				send_arp(nh->next_hop, if_ip, ether_hdr, nh->interface, htons(ARPOP_REQUEST));
				continue;
			}

			memcpy(ether_hdr->ether_dhost, aentry->mac, 6);
			// uint8_t *nh_imac = calloc(6, sizeof(uint8_t));
			// get_interface_mac(nh->interface, nh_imac);
			// memcpy(ether_hdr->ether_shost, nh_imac, 6);
			send_packet(nh->interface, &m);
		}
	}
	
	return 0;
}
