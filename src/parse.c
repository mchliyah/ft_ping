#include "../include/ft_ping.h"

static const t_option long_options[] = {
    {"help",      no_argument,       NULL, 'h'},
    {"version",   no_argument,       NULL, 'V'},
    {"count",     required_argument, NULL, 'c'},
    {"interval",  required_argument, NULL, 'i'},
    {"timeout",   required_argument, NULL, 'W'},
    {"deadline",  required_argument, NULL, 'w'},
    {"size",      required_argument, NULL, 's'},
    {"ttl",       required_argument, NULL, 't'},
    {"verbose",   no_argument,       NULL, 'v'},
    {"quiet",     no_argument,       NULL, 'q'},
    {"no-dns",    no_argument,       NULL, 'n'},
    {"source",    required_argument, NULL, 'I'},
    {NULL, 0, NULL, 0}
};


void parse_arguments(int ac, char **av, t_ping_config *config) {
    int opt;
    int option_index = 0;
    
    const char *short_options = "hVc:i:I:s:t:W:w:vqn";

    while ((opt = getopt_long(ac, av, short_options, 
                              (const struct option *)long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                config->help = 1;
                break;
            case 'V':
                config->version = 1;
                break;
            case 'c': 
                config->count = atoi(optarg);
                if (config->count <= 0) {
                    fprintf(stderr, "ft_ping: bad number of packets to transmit: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'i':
                config->interval = atof(optarg) * 1000000;
                if (config->interval <= 0) {
                    fprintf(stderr, "ft_ping: cannot set interval to %s seconds\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'I':
                config->source_ip = strdup(optarg);
                {
                    struct in_addr addr;
                    if (inet_pton(AF_INET, config->source_ip, &addr) != 1) {
                        fprintf(stderr, "ft_ping: invalid source address: %s\n", optarg);
                        exit(EXIT_FAILURE);
                    }
                }
                break;
            case 's':
                config->size = atoi(optarg);
                if (config->size <= 0) {
                    fprintf(stderr, "ft_ping: invalid packet size %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 't':
                config->ttl = atoi(optarg);
                if (config->ttl <= 0 || config->ttl > 255) {
                    fprintf(stderr, "ft_ping: ttl %s out of range\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'W':
                config->timeout = atoi(optarg);
                if (config->timeout <= 0) {
                    fprintf(stderr, "ft_ping: invalid timeout %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'w':
                config->deadline = atoi(optarg);
                if (config->deadline <= 0) {
                    fprintf(stderr, "ft_ping: invalid deadline %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                config->verbose++;
                break;
            case 'q':
                config->quiet = 1;
                break;
            case 'n':
                config->no_dns = 1;
                break;
            default:
                fprintf(stderr, "ft_ping: unknown option\n");
                exit(EXIT_FAILURE);
        }
    }
    if (optind < ac) {
        config->target = strdup(av[optind]);
    } else if (!config->target) {
        if (config->help) {
            print_usage();
            return;
        } else if (config->version) {
            print_version();
            return;
        } else {
            fprintf(stderr, "ft_ping: no target specified\n");
            exit(EXIT_FAILURE);
        }
    }
    validate_config(config);
}
