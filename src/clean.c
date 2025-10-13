#include "../include/ft_ping.h"


void free_resources(t_ping_config *config) {
    if (config->target) free(config->target);
    if (config->target_ip) free(config->target_ip);
    if (config->source_ip) free(config->source_ip);
    // if (config->resolved_host) free(config->resolved_host);
}

void clean_exit(t_ping_config *config, int exit_code) {
    free_resources(config);
    exit(exit_code);
}
