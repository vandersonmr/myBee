/*
 * repa_header.h
 *
 *  Created on: 10/04/2013
 *      Author: Héberte Fernandes de Moraes
 */

#include <stdint.h>

#ifndef __REPA_HEADER_H__
#define __REPA_HEADER_H__

#ifndef PREFIX_ADDRESS
#define PREFIX_ADDRESS
typedef uint32_t prefix_addr_t;
#endif

/** Type definition of repa header
 * The __attribute__((__packed__)) is used to say for compiler do not
 * waste memory. In this way the struct repahdr has the same size in
 * any architecture.
 */
typedef struct repahdr {
	uint8_t version:4,		// 4 bits /* Protocol Version */
			hide_flag:1,	// 1 bits /* Hide the interest */
			cripto:1,		// 1 bits /* Indicate if interest and data is encrypt */
			other_flags:2;	// 2 bits /* Don't used yet */
	uint8_t ttl;  			// 8 bits /* Time to live (hop count) */
	uint16_t hlen; 			// 16 bits /* Header length */
	uint32_t seq_number; 	// 32 bits /* Sequence number */
	prefix_addr_t prefix_dst; 	// 32 bits /* Destination prefix */
	prefix_addr_t prefix_src; 	// 32 bits /* Source prefix */
	uint64_t timestamp; 	// 64 bits Timestamp in nanoseconds since Epoch
} __attribute__((__packed__)) repa_header_t;

#endif /* __REPA_HEADER_H__ */
