#include "../include/ft_ping.h"

static const t_option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'V'},
    {"count", required_argument, NULL, 'c'},
    {"interval", required_argument, NULL, 'i'},
    {"timeout", required_argument, NULL, 'W'},
    {"deadline", required_argument, NULL, 'w'},
    {"size", required_argument, NULL, 's'},
    {"ttl", required_argument, NULL, 't'},
    {"verbose", no_argument, NULL, 'v'},
    {"quiet", no_argument, NULL, 'q'},
    {"no-dns", no_argument, NULL, 'n'},
    {"audible", no_argument, NULL, 'a'},
    {"flood", no_argument, NULL, 'f'},
    {"timestamp", no_argument, NULL, 'D'},
    {"pattern", required_argument, NULL, 'p'},
    {"type-of-service", required_argument, NULL, 'Q'},
    {"dont-fragment", no_argument, NULL, 'M'},
    {"record-route", no_argument, NULL, 'R'},
    {"statistics", no_argument, NULL, 'S'},
    {"continuous", required_argument, NULL, 'C'}, // Custom: continuous seconds
    {"source", required_argument, NULL, 'I'},     // -I for source IP
    {NULL, 0, NULL, 0}
};

void print_version(void) {
    printf("ft_ping custom implementation v1.0\n");
    printf("Based on UNIX ping utilities\n");
}

int is_valid_hex_pattern(const char *str) {
    if (!str || strlen(str) % 2 != 0) return 0;
    
    for (size_t i = 0; i < strlen(str); i++) {
        if (!((str[i] >= '0' && str[i] <= '9') || 
              (str[i] >= 'a' && str[i] <= 'f') || 
              (str[i] >= 'A' && str[i] <= 'F'))) {
            return 0;
        }
    }
    return 1;
}

int parse_hex_pattern(const char *str) {
    return (int)strtol(str, NULL, 16);
}


void parse_arguments(int ac, char **av, t_ping_config *config) {
    int opt;
    int option_index = 0;
    
    // Complete short options string
    const char *short_options = "hVc:i:I:s:t:W:w:vqnafp:DQ:MRSC:";
    
    while ((opt = getopt_long(ac, av, short_options, 
                             (const struct option *)long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h': config->help = 1; break;
            case 'V': config->version = 1; break;
            
            case 'c': 
                config->count = atoi(optarg);
                if (config->count <= 0) {
                    fprintf(stderr, "ft_ping: bad number of packets to transmit: %s\n", optarg);
                    exit(1);
                }
                break;
                
            case 'i':
                config->interval = atof(optarg) * 1000000; // Convert to microseconds
                if (config->interval <= 0) {
                    fprintf(stderr, "ft_ping: cannot set interval to %s seconds\n", optarg);
                    exit(1);
                }
                break;
                
            case 'I':
                config->source_ip = strdup(optarg);
                struct in_addr addr;
                if (inet_pton(AF_INET, config->source_ip, &addr) != 1) {
                    fprintf(stderr, "ft_ping: invalid source address: %s\n", optarg);
                    exit(1);
                }
                break;
                
            case 's':
                config->size = atoi(optarg);
                if (config->size <= 0) {
                    fprintf(stderr, "ft_ping: invalid packet size %s\n", optarg);
                    exit(1);
                }
                break;
                
            case 't':
                config->ttl = atoi(optarg);
                if (config->ttl <= 0 || config->ttl > 255) {
                    fprintf(stderr, "ft_ping: ttl %s out of range\n", optarg);
                    exit(1);
                }
                break;
                
            case 'W':
                config->timeout = atoi(optarg);
                if (config->timeout <= 0) {
                    fprintf(stderr, "ft_ping: 2 invalid timeout %s\n", optarg);
                    exit(1);
                }
                break;
                
            case 'w':
                config->deadline = atoi(optarg);
                if (config->deadline <= 0) {
                    fprintf(stderr, "ft_ping: invalid deadline %s\n", optarg);
                    exit(1);
                }
                break;
                
            case 'v': config->verbose++; break;
            case 'q': config->quiet = 1; break;
            case 'n': config->no_dns = 1; break;
            case 'a': config->audiable = 1; break;
            case 'f': config->flood = 1; break;
            case 'D': config->timestamp = 1; break;
            case 'R': config->route = 1; break;
            case 'S': config->statistics = 1; break;
                
            case 'p':
                if (!is_valid_hex_pattern(optarg)) {
                    fprintf(stderr, "ft_ping: pattern must be hex digits: %s\n", optarg);
                    exit(1);
                }
                config->pattern = parse_hex_pattern(optarg);
                break;
                
            case 'Q':
                config->tos = atoi(optarg);
                if (config->tos < 0 || config->tos > 255) {
                    fprintf(stderr, "ft_ping: invalid type of service %s\n", optarg);
                    exit(1);
                }
                break;
                
            case 'M':
                config->dont_fragment = 1;
                break;
                
            case 'C':
                config->continuous = atoi(optarg);
                if (config->continuous <= 0) {
                    fprintf(stderr, "ft_ping: continuous time must be positive\n");
                    exit(1);
                }
                break;
                
            default:
                fprintf(stderr, "ft_ping: unknown option\n");
                exit(1);
        }
    }
    if (optind < ac) {
        config->target = strdup(av[optind]);
    } else if (!config->target) {
        fprintf(stderr, "ft_ping: no target specified\n");
        exit(1);
    }
    
    validate_config(config);
}
