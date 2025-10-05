#include "../include/ft_ping.h"


void validate_config(t_ping_config *config) {
    if (config->help) {
        print_usage("ft_ping");
        exit(0);
    }
    
    if (!config->target) {
        fprintf(stderr, "ft_ping: error: destination required\n");
        print_usage("ft_ping");
        exit(1);
    }
    
    if (config->count < -1 || config->count == 0) {
        fprintf(stderr, "ft_ping: bad number of packets to transmit: %d\n", config->count);
        exit(1);
    }
    
    if (config->size < 0 || config->size > 65535) {
        fprintf(stderr, "ft_ping: invalid packet size %d\n", config->size);
        exit(1);
    }
    
    if (config->timeout <= 0) {
        fprintf(stderr, "ft_ping: 1 invalid timeout %d\n", config->timeout);
        exit(1);
    }
    
    if (config->continuous < 0) {
        fprintf(stderr, "ft_ping: invalid continuous time %d\n", config->continuous);
        exit(1);
    }
}