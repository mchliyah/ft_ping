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

/* Maximum packet size and default values */
#define MAX_PACKET_SIZE 65536
#define DEFAULT_PING_SIZE 56
#define DEFAULT_TIMEOUT 3
#define DEFAULT_COUNT 4
#define DEFAULT_CONTINUOUS 0
#define DEFAULT_INTERVAL 1
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

/* Updated config structure with a new "state" field */
typedef struct s_ping_config {
    // Target information
    char        *target;
    char        *target_ip;
    char        *source_ip;

    int         sockfd; // raw socket file descriptor
    struct sockaddr_in addr; // target address
    socklen_t   addr_len; // length of the address

    // Basic operation
    int         count;          // -c
    int         continuous;     // -t (time in seconds)
    int         interval;       // -i
    int         timeout;        // -W
    int         deadline;       // -w (seconds until timeout)
    
    // Packet options
    int         size;           // -s
    int         ttl;            // -t (TTL)
    int         tos;            // -Q (Type of Service)
    int         dont_fragment;  // -M (do/don't fragment)
    int         pattern;        // -p (pattern to fill)
    
    // Output control
    int         verbose;        // -v
    int         quiet;          // -q
    int         no_dns;         // -n
    int         audiable;       // -a
    int         flood;          // -f
    int         statistics;     // -S (print statistics)
    
    // Special modes
    int         help;           // -h
    int         version;        // -V
    int         timestamp;      // -D
    int         route;          // -R (Record route)

    // Runtime statistics and control
    t_ping_stats stats;
    volatile sig_atomic_t keep_running;

    // Added state for internal usage (0: idle, 1: active, etc.)
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
    (config)->dont_fragment = -1; /* -1 means not set */ \
    (config)->state = 0; /* Default state */ \
} while(0)

#define V_PRINT(config, level, fmt, ...) do { \
    if ((config)->verbose >= (level)) { \
        printf(fmt, ##__VA_ARGS__); \
    } \
} while(0)

// Function prototypes
void    parse_arguments(int ac, char **av, t_ping_config *config);
void    print_usage(const char *program_name);
void    print_version(void);
void    validate_config(t_ping_config *config);
void    free_resources(t_ping_config *config);
void    clean_exit(t_ping_config *config, int exit_code);
void    print_config(t_ping_config *config);
void    ping(t_ping_config *config);
unsigned short checksum(void *b, int len);

#endif