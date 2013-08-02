/*
 * util.h
 *
 *  Created on: 26/07/2011
 *      Author: heberte
 */

#include "repa.h"

#include <unistd.h>
#include <sys/socket.h>

#include <linux/if_ether.h>

#ifndef UTIL_H_
#define UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define REPA_DEFAULT_ADHOC_NAME		"repad"

#define BUFFER_LEN		ETH_FRAME_LEN
#define MSG_MEMORY_LEN	BUFFER_LEN + 50
#define MAX_TIMESTAMP	60E9 // 60 000 second in microseconds

#define IPV4_LLBASE		0xa9fe0000 /* 169.254.0.0 */
#define IPV4_NETMASK 	0xffff0000 /* 255.255.0.0 */

/* Defines macros */
#define ETH_P_REPA		0x3145	/* Repa packet */

/* Paths */
#define REPA_SCK_NAME		"/dev/repa"
//#define PROC_NET_WIRELESS	"/proc/net/wireless" // Don't work in Android
#define PROC_NET_DEV		"/proc/net/dev"

/* Parameter */
#define REPA_WAIT_SECONDS	2
#define REPA_LOCK_FILE  	REPA_SCK_NAME
#define REPA_PREFIX_LENGTH	8 // in fields
#define REPA_FIELD_LENGTH	3 // in bits
#define REPA_MAX_HTL		10 // Max HTL
#define MAX_BUFFER_MSG		100 // Max messages in application buffer
#define MAX_RECEIVED_MSG	1 // Max message in node memory

#define MAX_U_INT_24 		16777215

/* Some usefull constants */
#define KILO	1e3
#define MEGA	1e6
#define GIGA	1e9

#define MIN(a,b) (a>b?b:a)

#ifndef PREFIX_ADDRESS
#define PREFIX_ADDRESS
typedef u_int32_t prefix_addr_t;
#endif

struct repahdr {
	u_int8_t version:4;	// 4 bits
	unsigned hide_flag:1;	// 1 bits
	unsigned other_flags:3;	// 3 bits
	u_int8_t hop_to_live;  // 8 bits
	u_int16_t header_len; // 16 bits
	u_int32_t message_id; // 32 bits /* Identifier for message */
	prefix_addr_t c_dst; // 32 bits
	prefix_addr_t c_src; // 32 bits
	u_int64_t timestamp; // Timestamp in microseconds since Epoch
};

typedef struct { // For lib-repa
	size_t 		data_len;
	prefix_addr_t prefix_addr;
	char		*interest;
	void		*data;
} message_t;

typedef struct { // For repad
	prefix_addr_t prefix_addr;
	uint32_t message_id;
	uint64_t timestamp;
} memory_message_t;

/**
 *
 * Statistics collector struct
 *
 */
#define CLEAR(s) bzero(s,sizeof(s))
#define CALC_MEAN(qtyVal, newVal, result) result=(result*(qtyVal-1)+newVal)/(qtyVal==0?1:qtyVal)

typedef struct statistics {
	u_int32_t qtyMessageUniqueReceive; // Unique messages received after matching filter
	u_int32_t qtyMessageTotalReceived; // Messages received before matching filter
	u_int32_t qtyMessageAcceptInterest; // Unique messages accepted by interest

	u_int32_t qtyMessageDroppedPrefixC; // Messages dropped by unmatched prefix C
	u_int32_t qtyMessageDroppedMaxHTL; // Messages dropped by achieve maximum htl
	u_int32_t qtyMessageDroppedMemory; // Messages dropped by memory strategy (to filter unique messages)

	u_int32_t qtyMessageSent; // Messages forward (node not match interest)
	u_int32_t qtyMessageSentCollaborative; // Messages forward Collaboratively

	u_int32_t meanHtl; // Mean Hop to live for accepted interest messages
	u_int64_t meanLatency; // Mean latency for accepted interest messages
	u_int64_t meanPropagationTime; // Mean Propagation time

	u_int32_t meanHtlForAll; // Mean Hop to live for all messages
} statistics_t;


struct interest {
	char *interest; // Interest viewed
	u_int64_t timestamp;
};

struct node {
	prefix_addr_t prefix; // Prefix viewed
	u_int64_t timestamp;
};

struct dllist;

extern void repa_print_interests(struct dllist *list_interests);
extern void repa_print_nodes(struct dllist *list_nodes);
extern void repa_print_statistics(bool verbose, u_int32_t address, statistics_t *statistics);

/**
 *
 * O filtro de casamento desenvolvido avalia os campos do prefixo do
 * nó e verifica se ocorre o casamento de pelo menos um dos campos. O prefixo tem
 * 32bits em nosso caso e para isso é necessário dividir estes 32 bits pelos campos que compõe
 * o prefixo. As macros abaixo, fazem esta divisão:
 *
 * REPA_MASK_FIELD(size,nField,field,length)
 * REPA_GET_FIELD(prefix,nField,field,length)
 *
 * A macro REPA_MASK_FIELD  cria uma máscara de bits 1 para o campo requisitado:
 *
 * Supondo o prefixo com 32bits abaixo:
 *
 * P = 10011011101011010110010101011010
 *
 * Supondo que temos 8 campos no prefixo cada qual composto de 4 bits, ou seja:
 *
 * Campos =  C0   C1   C2   C3   C4   C5   C6   C7
 *      P = 1001 1011 1010 1101 0110 0101 0101 1010
 *
 * A macro REPA_MASK_FIELD gera uma máscara para pegar apenas o campo desejado:
 * Por exemplo, querendo pegar o campo 3 (C2, dado que começa a contagem desde 0 até 7)
 *
 * REPA_MASK_FIELD(32,8,3,4) , onde os parâmetros são:
 * 		size = 32 ; 32 bits é o tamanho das características
 * 		nField = 8 ; Estamos dividindo os 32 bits em 8 campos
 * 		field = 3 ; O número do campo que desejamos gerar a máscara
 * 		length = 4 ; A quantidade de bits de cada campo
 *
 * Logo o resultado desta chamada da macro será:
 *
 *                              Campos =  C0   C1   C2   C3   C4   C5   C6   C7
 *                                   P = 1001 1011 1010 1101 0110 0101 0101 1010
 *           REPA_MASK_FIELD(32,8,3,4) = 0000 0000 1111 0000 0000 0000 0000 0000
 *
 * Com isso, ao se fazer um AND binário entre P e REPA_MASK_FIELD temos apenas os bits do campo C2.
 *
 * A macro REPA_GET_FIELD pega o valor contido no campo desejado, ele utiliza a máscara
 * criada pela macro REPA_MASK_FIELD para pegar o valor do campo desejado.
 *
 * REPA_GET_FIELD(P,8,3,4), onde os parâmetros são:
 * 		prefix = Ao campo de características que desejamos pegar um campo, neste caso usamos P = 1001 1011 1010 1101 0110 0101 0101 1010
 * 		nField = 8 ; Estamos dividindo os 32 bits em 8 campos
 * 		field = 3 ; O número do campo que desejamos pegar o valor
 * 		length= 4 ; A quantidade de bits de cada campo
 *
 * Logo, esta chamada irá criar a máscara, realizar o AND binários entre a máscara e P, por fim deslocar
 * o valor para o bit menos significativo. Assim:
 *
 *
 *
 *
 *
 *                          Bit MAIS significativo              Bit MENOS significativo
 *                                   ↓                                     ↓
 *                          Campos =  C0   C1   C2   C3   C4   C5   C6   C7
 *                               P = 1001 1011 1010 1101 0110 0101 0101 1010
 *       REPA_MASK_FIELD(32,8,3,4) = 0000 0000 1111 0000 0000 0000 0000 0000
 *  P && REPA_MASK_FIELD(32,8,3,4) = 0000 0000 1010 0000 0000 0000 0000 0000
 *                 Valor deslocado = 0000 0000 0000 0000 0000 0000 0000 1010
 *
 *
 * Campo desejado = 1010
 *
 */
#define REPA_MASK_FIELD(size,nField,field,length) (((1<<length)-1)<<(field*(size/nField)))
#define REPA_GET_FIELD(prefix,nField,field,length) ((prefix&REPA_MASK_FIELD(8*sizeof(prefix),nField,field,length))>>(field*(8*sizeof(prefix)/nField)))

#define MAC_FORMAT "%02X:%02X:%02X:%02X:%02X:%02X"
#define MAC_BYTES(p)  p[0],p[1],p[2],p[3],p[4],p[5]

/* Messages types to communicate with repa daemon */
#define TMSG_SEND 				1
#define TMSG_INTEREST_REG 		2
#define TMSG_INTEREST_UNREG 	3
#define TMSG_ASK_ADDR 			4
#define TMSG_SETUP_ADHOC		5
#define TMSG_SETDOWN_ADHOC		6
#define TMSG_IS_ADHOC_ACTIVED	7
#define TMSG_RECV				8
#define TMSG_GET_INTERESTS		9
#define TMSG_SEND_TIMESTAMP		10
#define TMSG_SEND_PING			11
#define TMSG_CLEAR_STATISTICS	12
#define TMSG_GET_NODES			13
#define TMSG_WPA_CREATE_ADHOC	14
#define TMSG_SEND_HIDE 			15

/* Define types */
typedef struct repahdr repa_header_t;

struct ifreq;
struct iwreq;
struct ifconf;
struct iw_freq;
struct timeval;

struct sock_param_t {
	int sock_fd;
	socklen_t address_len;
	struct sockaddr address;
};

/* Repa header length without interest */
//#define REPA_HDR_LEN_WOI	sizeof(u_int8_t) + sizeof(u_int8_t) + sizeof(u_int16_t) + sizeof(u_int32_t) + sizeof(u_int32_t) + sizeof(u_int32_t)
#define REPA_HDR_LEN_WOI	sizeof(repa_header_t)

/* Util */
extern float rand_gauss(float m, float s);
/* Return 1 if the difference is negative, otherwise 0.  */
extern int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1);
extern pid_t* find_pid_by_name(char* pidName);
extern inline uint64_t get_microseconds(void);
extern inline uint64_t get_formated_time(char *time_formated);

/* Wireless commands  */
#define ieee80211mhz2chan(x) \
	(((x) == 2484) ? 14 : \
	(((x) < 2484) ? ((x)-2407)/5 : \
	((x)/5 - 1000)))
#define ieee80211chan2mhz(x) \
	(((x) <= 14) ? \
	(((x) == 14) ? 2484 : ((x) * 5) + 2407) : \
	((x) + 1000) * 5)
extern void find_wifi_ifaces(char* ifname);
extern int wifi_load(void);
extern int wifi_unload(void);
extern int wpa_connect(void);
extern void wpa_disconnect(void);
extern int wpa_create_adhoc(const char *essid, uint32_t channel);
extern int wpa_verifies_exist(const char *essid, uint32_t channel);
extern void wpa_call_command(char* command, char** result);
extern void try_create_adhoc(char *repa_adhoc_name, u_int32_t repa_adhoc_freq);
extern void iw_float2freq(double freq, struct iw_freq *out);
extern double iw_freq2float(const struct iw_freq *in);
extern void print_iw_freq_value(char *	buffer, int	buflen,	double freq);
extern int set_iw_mode(int s, struct iwreq *iwr, u_int32_t mode);
extern int set_iw_channel(int s, struct iwreq *iwr, u_int32_t channel);
extern int set_iw_essid(int s, struct iwreq *iwr, const char *essid);
extern int get_iw_essid(int s, struct iwreq *iwr, char *essid);
extern int get_iw_freq(int s, struct iwreq *iwr, int32_t *freq);

/* Debug command */
extern int open_repad_out(void);
extern int close_repad_out(void);
extern int repad_printf(int priority, const char *format, ...);
extern void repad_print_mem(const char *data, size_t data_len);
extern void repad_print_char(const char *data, size_t data_len);
extern void repa_random_prefix(prefix_addr_t *address);

extern void print_mem(FILE *where, char *data, size_t data_len);

/* Iface commands */
extern int if_nametoindex(const char*);
extern void generate_ipaddr(u_int8_t mac[], u_int32_t *ipaddr);
extern int get_iface_attr(char* name, u_char* localMac, bool* is_up, bool* is_running, bool* is_loopback, bool* is_wireless);
extern int get_iface_list(struct ifconf *ifconf);
extern int set_iface_addr(int s, struct ifreq *ifr, const char *addr);
extern int get_iface_mac(int s, struct ifreq *ifr);
extern inline char* get_iface_name(char *name,	int nsize, char *buf);
extern int set_iface_flags(int s, struct ifreq *ifr, int set, int clr);
extern int set_iface_netmask(int s, struct ifreq *ifr, const char *addr);

/* Serialize and Deserialize message */
extern inline void __msg_serialize(char *out_buffer, size_t *out_buffer_size, const prefix_addr_t in_prefix_addr, const u_int8_t in_interest_len, const char *in_interest, const u_int16_t in_data_len, const char *in_data);
#define msg_serialize(out_buffer, out_buffer_size, in_prefix_addr, in_interest_len, in_interest, in_data_len, in_data)  \
	__msg_serialize((char*)out_buffer, &out_buffer_size, in_prefix_addr, in_interest_len, in_interest, in_data_len, in_data)

extern inline void __msg_deserialize(const char *in_data, prefix_addr_t *out_prefix_addr, u_int8_t *out_interest_len, char *out_interest, u_int16_t *out_data_len, char *out_data);
#define msg_deserialize(in_data, out_prefix_addr, out_interest_len, out_interest, out_data_len, out_data) __msg_deserialize(in_data, &out_prefix_addr, &out_interest_len, out_interest, &out_data_len, (char*)out_data)

#ifdef __cplusplus
}
#endif
#endif /* UTIL_H_ */
