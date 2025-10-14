#ifndef FT_PING_H
#define FT_PING_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#define MAX_PACKET_SIZE 65536
#define DEFAULT_PING_SIZE 56
#define DEFAULT_TIMEOUT -1
#define DEFAULT_COUNT -1
#define DEFAULT_CONTINUOUS 0
#define DEFAULT_INTERVAL 1000000
#define DEFAULT_TTL 64

typedef struct s_option
{
    const char *name;
    int         has_arg;
    int        *flag;
    int         val;
}               t_option;

typedef struct s_ping_stats {
    int transmitted;
    int received;
    double rtt_min;
    double rtt_max;
    double rtt_sum;
    struct timeval start_time;
} t_ping_stats;

typedef struct s_ping_config {
    char        *target;
    char        *target_ip;
    char        *source_ip;

    int         sockfd;
    struct sockaddr_in addr;
    socklen_t   addr_len;

    int         count;          
    int         continuous;     
    int         interval;      
    int         timeout;
    int         deadline;     
    
    int         size;
    int         ttl;
    int         tos; 
    int         dont_fragment;
    int         pattern;
    
    int         verbose;
    int         quiet;
    int         no_dns;
    int         audiable;
    int         flood;
    int         statistics;

    int         help;
    int         version;
    int         timestamp;
    int         route;

    t_ping_stats stats;
    volatile sig_atomic_t keep_running;

    int         state;
} t_ping_config;

#define INIT_CONFIG(config) do { \
    memset(config, 0, sizeof(t_ping_config)); \
    (config)->target = NULL; \
    (config)->continuous = DEFAULT_CONTINUOUS; \
    (config)->count = DEFAULT_COUNT; \
    (config)->size = DEFAULT_PING_SIZE; \
    (config)->verbose = 0; \
    (config)->timeout = DEFAULT_TIMEOUT; \
    (config)->help = 0; \
    (config)->version = 0; \
    (config)->interval = DEFAULT_INTERVAL; \
    (config)->ttl = DEFAULT_TTL; \
    (config)->dont_fragment = -1; \
    (config)->state = 0; \
} while(0)

#define V_PRINT(config, level, fmt, ...) do { \
    if ((config)->verbose >= (level)) { \
        printf(fmt, ##__VA_ARGS__); \
    } \
} while(0)

void    parse_arguments(int ac, char **av, t_ping_config *config);
void    print_usage(void);
void    print_version(void);
void    validate_config(t_ping_config *config);
void    free_resources(t_ping_config *config);
void    clean_exit(t_ping_config *config, int exit_code);
void    print_config(t_ping_config *config);
void    ping(t_ping_config *config);
unsigned short checksum(void *b, int len);
void    print_version(void);

#endif