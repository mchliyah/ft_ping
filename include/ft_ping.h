#ifndef FT_PING_H
#define FT_PING_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <getopt.h>
#include <netinet/in.h>

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


typedef struct s_ping_config {
    // Target information
    char        *target;
    char        *source_ip;

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
} while(0)


// Function prototypes
void    parse_arguments(int ac, char **av, t_ping_config *config);
void    print_usage(const char *program_name);
void    print_version(void);
// void    init_config(t_ping_config *config);
void    validate_config(t_ping_config *config);
void free_resources(t_ping_config *config);
void clean_exit(t_ping_config *config, int exit_code);
void    print_config(t_ping_config *config);

#endif