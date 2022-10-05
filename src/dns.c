#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <inttypes.h>
#include <bearssl.h>

#include "errors.h"
#include "http.h"
#include "uri.h"
#include "address.h"
#include "utils.h"
#include "socket.h"
#include "ssl.h"
#include "callbacks.h"

/* Structure of the bytes for a DNS header */
typedef struct {
  unsigned short xid;
  unsigned short flags;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short nscount;
  unsigned short arcount;
} DNSHeader;

/* Structure of the bytes for a DNS question */
typedef struct
{
  unsigned char *name;
  unsigned short dnstype;
  unsigned short dnsclass;
} DNSQuestion;

/* Structure of the bytes for an IPv4 answer */
typedef struct {
  unsigned short compression;
  unsigned short type;
  unsigned short class;
  unsigned int ttl;
  unsigned short length;
  struct in_addr addr;
} __attribute__((packed)) dns_record_a_t;

void print_dns_response (unsigned char *response)
{
  /* First, check the header for an error response code */
  DNSHeader *header = (DNSHeader *)response;
  if ((ntohs (header->flags) & 0xf) != 0)
    {
      fprintf (stderr, "Failed to get response\n");
      return;
    }

  /* Reconstruct the question */
  unsigned char *start_of_question = response + sizeof(DNSHeader);
//  start_of_question--;
  printf("kkkk %i\n", header->ancount);
  DNSQuestion *questions = calloc (sizeof(DNSQuestion), header->ancount);
  for (int i = 0; i < ntohs (header->ancount); i++)
    {
      questions[i].name = (char *)start_of_question;
      puts(start_of_question);
      unsigned char total = 0;
      unsigned char *field_length = (unsigned char *)questions[i].name;
      while (*field_length != 0)
        {
          total += *field_length + 1;
          *field_length = '.';
          field_length = (unsigned char *)questions[i].name + total;
        }
      questions[i].name++;
      /* Skip null byte, qtype, and qclass */
      start_of_question = field_length + 5;
    }

  /* The records start right after the question section. For each record,
     confirm that it is an A record (only type supported). If any are not
     an A-type, then return. */
  dns_record_a_t *records = (dns_record_a_t *)start_of_question;
  for (int i = 0; i < ntohs (header->ancount); i++)
    {
      printf ("Record for %s\n", questions[i].name);
      printf ("  TYPE: %" PRId16 "\n", ntohs (records[i].type));
      printf ("  CLASS: %" PRId16 "\n", ntohs (records[i].class));
      printf ("  TTL: %" PRIx32 "\n", ntohl (records[i].ttl));
      printf ("  IPv4: %08" PRIx32 "\n",
              ntohl ((unsigned int)records[i].addr.s_addr));
      printf ("  IPv4: %s\n", inet_ntoa (records[i].addr));
    }
}


enum DNSQType {
	A = 1,
	AAAA = 28
};

enum DNSSpecification {
	DNS_OVER_HTTPS,
	DNS_OVER_TLS,
	DNS_OVER_PLAIN_UDP,
	DNS_OVER_PLAIN_TCP,
};

struct DNSRequest {
	enum DNSQType qtype;
	const char* domain_name;
	enum DNSSpecification specification;
};

static const enum DNSQType QTYPES[] = {A, AAAA};

static const char TCP_LENGTH_FIELD[] = {
	0x00,
	0x00
};

static const char DNS_BUFFER_PREFIX[] = {
	0x00, 0x00, 0x01, 0x00,
	0x00, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};

static const char DNS_BUFFER_SUFFIX[] = {
	0x00, 0x00, 0x00, 0x00, 0x01
};

static const size_t IPV4_ADDRESS_SIZE = 4;
static const size_t IPV6_ADDRESS_SIZE = 16;

int dns_request_stringify(const struct DNSRequest obj, char** dst, size_t* dst_size) {
	(void) QTYPES;
	
	const int length_field_required = obj.specification == DNS_OVER_PLAIN_TCP || obj.specification == DNS_OVER_TLS;
	
	const size_t domain_size = strlen(obj.domain_name) + 1;
	const size_t buffer_size = (length_field_required ? sizeof(TCP_LENGTH_FIELD) : 0) + sizeof(DNS_BUFFER_PREFIX) + domain_size + sizeof(DNS_BUFFER_PREFIX);
	
	char* buffer = malloc(buffer_size);
	
	if (buffer == NULL) {
		return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
	}
	
	int buffer_offset = 0;
	
	if (length_field_required) {
		memcpy(buffer, TCP_LENGTH_FIELD, sizeof(TCP_LENGTH_FIELD));
		buffer_offset += sizeof(TCP_LENGTH_FIELD);
		buffer[buffer_offset - 1] = (char) buffer_size - buffer_offset;
	}
	
	memcpy(buffer + buffer_offset, DNS_BUFFER_PREFIX, sizeof(DNS_BUFFER_PREFIX));
	buffer_offset += sizeof(DNS_BUFFER_PREFIX);
	
	const char* label_start = obj.domain_name;
	
	// Encode domain name using the DNS name notation
	for (size_t index = 0; index < domain_size; index++) {
		const char* ch = &obj.domain_name[index];
		
		if (*ch == '.' || *ch == '\0') {
			const size_t label_length = ch - label_start;
			
			buffer[buffer_offset++] = label_length;
			memcpy(buffer + buffer_offset, label_start, label_length);
			buffer_offset += label_length;
			
			label_start = ch + 1;
		}
	}
	
	memcpy(buffer + buffer_offset, DNS_BUFFER_SUFFIX, sizeof(DNS_BUFFER_SUFFIX));
	buffer_offset += sizeof(DNS_BUFFER_SUFFIX);
	buffer[buffer_offset - 3] = (char) obj.qtype;
	
	*dst = buffer;
	*dst_size = buffer_size;
	
	return UNALIXERR_SUCCESS;
	
}

int dns_query(
	const char* domain_name,
	const int port,
	const struct URI dns_server,
	struct Address* res
) {
	
	struct DNSRequest dns_request = {.domain_name = domain_name};
	
	if (strcmp(dns_server.scheme, "https") == 0) {
		dns_request.specification = DNS_OVER_HTTPS;
	} else if (strcmp(dns_server.scheme, "tls") == 0) {
		dns_request.specification = DNS_OVER_TLS;
	} else if (strcmp(dns_server.scheme, "udp") == 0) {
		dns_request.specification = DNS_OVER_PLAIN_UDP;
	} else if (strcmp(dns_server.scheme, "tcp") == 0) {
		dns_request.specification = DNS_OVER_PLAIN_TCP;
	} else {
		return UNALIXERR_DNS_UNSUPPORTED_PROTOCOL;
	}
	
	const char* sa = dns_server.hostname;
	int sa_port = dns_server.port;
	
	if (sa_port < MIN_PORT_NUMBER) {
		switch (dns_request.specification) {
			case DNS_OVER_HTTPS:
				sa_port = 443;
				break;
			case DNS_OVER_TLS:
				sa_port = 853;
				break;
			case DNS_OVER_PLAIN_UDP:
			case DNS_OVER_PLAIN_TCP:
				sa_port = 53;
				break;
		}
	}
	
	struct Address address = {};
	
	if (address_parse(&address, sa, sa_port) != UNALIXERR_SUCCESS) {
		struct addrinfo hints = {.ai_family = AF_UNSPEC};
		struct addrinfo* res = NULL;
		
		char value[intlen(sa_port) + 1];
		snprintf(value, sizeof(value), "%i", sa_port);
		
		if (getaddrinfo(sa, value, &hints, &res) != 0) {
			return UNALIXERR_DNS_GAI_FAILURE;
		}
		
		memcpy(&address.addr_storage, res->ai_addr, res->ai_addrlen);
		
		address.af = res->ai_family;
		address.addr_storage_size = res->ai_addrlen;
		
		freeaddrinfo(res);
	}
	
	const int domain = (address.af);
	const int type = (dns_request.specification == DNS_OVER_PLAIN_UDP ? SOCK_DGRAM : SOCK_STREAM);
	const int protocol = (type == SOCK_DGRAM ? IPPROTO_UDP : IPPROTO_TCP);
	
	for (size_t index = 0; index < sizeof(QTYPES) / sizeof(*QTYPES); index++) {
		dns_request.qtype = QTYPES[index];
		
		char* request = NULL;
		size_t request_size = 0;
		
		const int code = dns_request_stringify(dns_request, &request, &request_size);
		
		if (code != UNALIXERR_SUCCESS) {
			return code;
		}
		
		if (dns_request.specification == DNS_OVER_HTTPS) {
			struct HTTPRequest http_request = {
				.version = HTTP10,
				.method = POST
			};
			
			http_request_set_uri(&http_request, dns_server);
			
			http_headers_add(&http_request.headers, "Accept", "application/dns-udpwireformat");
			http_headers_add(&http_request.headers, "Accept-Encoding", "identity");
			http_headers_add(&http_request.headers, "Connection", "close");
			http_headers_add(&http_request.headers, "Content-Type", "application/dns-udpwireformat");
			
			http_body_set(&http_request.body, request, request_size);
			
			free(request);
			
			const int code = http_request_stringify(&http_request, &request, &request_size);
			
			http_request_free(&http_request);
			
			if (code != UNALIXERR_SUCCESS) {
				return code;
			}
		}
		
		char response[MAX_BUFFER_SIZE + 1];
		size_t response_size = sizeof(response);
		
		struct sockaddr* addr = (struct sockaddr*) &address.addr_storage;
		socklen_t addrlen = address.addr_storage_size;
		
		int fd = socket_create(domain, type, protocol);
		
		if (protocol == IPPROTO_TCP) {
			if (socket_connect(fd, addr, addrlen) != UNALIXERR_SUCCESS) {
				socket_close(fd);
				return code;
			}
			
			if (dns_request.specification == DNS_OVER_PLAIN_TCP) {
				const ssize_t ssize = socket_send(fd, request, request_size, NULL, 0);
				
				free(request);
				
				if (ssize != (ssize_t) request_size) {
					socket_close(fd);
					return UNALIXERR_SOCKET_SEND_ERROR;
				}
				
				const ssize_t rsize = socket_recv(fd, response, response_size, NULL, 0);
				
				socket_close(fd);
				
				if (rsize < (ssize_t) MIN_BUFFER_SIZE) {
					return UNALIXERR_SOCKET_RECV_ERROR;
				}
				
				if (rsize > (ssize_t) MAX_BUFFER_SIZE) {
					return UNALIXERR_SOCKET_MAX_BUFFER_SIZE_EXCEEDED;
				}
				
				response_size = rsize;
			} else {
				br_ssl_client_context sc = {};
				br_x509_minimal_context xc = {};
				unsigned char iobuf[BR_SSL_BUFSIZE_BIDI];
				br_sslio_context ioc = {};
				
				br_ssl_client_init_full(&sc, &xc, TAs, TAs_NUM);
				br_ssl_engine_set_buffer(&sc.eng, iobuf, sizeof(iobuf), 1);
				br_ssl_client_reset(&sc, sa, 0);
				br_sslio_init(&ioc, &sc.eng, sock_read, &fd, sock_write, &fd);
				
				br_sslio_write_all(&ioc, request, request_size);
				br_sslio_flush(&ioc);
				
				free(request);
				
				const size_t rsize = br_sslio_read(&ioc, response, response_size);
				
				br_sslio_close(&ioc);
				socket_close(fd);
				
				const int code = br_ssl_engine_last_error(&sc.eng);
				
				if (code != BR_ERR_OK && code != BR_ERR_IO) {
					return UNALIXERR_SSL_FAILURE;
				}
				
				if (rsize > MAX_BUFFER_SIZE) {
					return UNALIXERR_SOCKET_MAX_BUFFER_SIZE_EXCEEDED;
				}
				
				response_size = rsize;
			}
		} else {
			const size_t ssize = socket_send(fd, request, request_size, addr, addrlen);
			
			free(request);
			
			if (ssize != request_size) {
				socket_close(fd);
				return UNALIXERR_SOCKET_SEND_ERROR;
			};
			
			const size_t rsize = socket_recv(fd, response, response_size, addr, &addrlen);
			
			socket_close(fd);
			
			if (rsize < MIN_BUFFER_SIZE) {
				return UNALIXERR_SOCKET_RECV_ERROR;
			}
			
			if (rsize > MAX_BUFFER_SIZE) {
				return UNALIXERR_SOCKET_MAX_BUFFER_SIZE_EXCEEDED;
			}
			
			response_size = rsize;
		}
		
		if (dns_request.specification == DNS_OVER_HTTPS) {
			struct HTTPResponse http_response = {};
			
			const int code = http_response_parse(&http_response, response, response_size);
			
			if (code != UNALIXERR_SUCCESS) {
				return code;
			}
			
			if (http_response.status.code != OK) {
				return UNALIXERR_DOH_BAD_STATUS_CODE;
			}
			
			memcpy(response, http_response.body.content, http_response.body.size);
			response_size = http_response.body.size;
			
			http_response_free(&http_response);
		}
		
		if (memcmp(response, "\x00\x00", 2) != 0) {
			return UNALIXERR_DNS_MALFORMED_RESPONSE;
		}
		
		if (memcmp(response + 2, "\x81\x80", 2) != 0) {
			return UNALIXERR_DNS_NXDOMAIN;
		}
		print_dns_response((unsigned char*) response);
		for (int i =0; i<response_size;i++) {
			printf("0x%X ", response[i]);
			printf("%c ", response[i]);
		}
		exit (0);
		printf("qt %i\n", dns_request.qtype);
		switch (dns_request.qtype) {
			case A: {
				const size_t rdlength = response[response_size - 5];
				
				if (rdlength != IPV4_ADDRESS_SIZE) {
					puts("a");
					continue;
				}
				
				char address[MAX_IPV4_ADDRESS_SIZE + 1];
				inet_ntop(AF_INET, &response[response_size - IPV4_ADDRESS_SIZE], address, sizeof(address));
				
				printf("k %i\n", address_parse(res, address, port));
				
				return UNALIXERR_SUCCESS;
			}
			case AAAA: {
				const int rdlength = (int) response[response_size - (IPV6_ADDRESS_SIZE + 1)];
				printf("rdlength = %i\n", rdlength);
				if (rdlength != IPV6_ADDRESS_SIZE) {
					puts("b");
					continue;
				}
				
				char address[MAX_IPV6_ADDRESS_SIZE + 1];
				inet_ntop(AF_INET6, &response[response_size - IPV6_ADDRESS_SIZE], address, sizeof(address));
				
				printf("k %i\n", address_parse(res, address, port));
				
				return UNALIXERR_SUCCESS;
			}
		}
	}
	
	return UNALIXERR_SUCCESS;
	
}

int main() {
	struct URI uri = {};
	uri_parse(&uri, "https://doh-ch.blahdns.com/dns-query");
	struct Address address = {};
	printf("%i\n", dns_query("g.co", 80, uri, &address));
	
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];
	getnameinfo((struct sockaddr*) &address.addr_storage, address.addr_storage_size, host, sizeof(host), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);
	
	printf("Got connection from %s on port %s\n", host, port);
	
}