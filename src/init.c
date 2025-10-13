#include "../include/ft_ping.h"


void resolve_ip(t_ping_config *config) {
    struct addrinfo hints, *res;
    int errcode;
    char addrstr[100];
    void *ptr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    if ((errcode = getaddrinfo(config->target, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "ft_ping: error: cannot resolve %s: %s\n", config->target, gai_strerror(errcode));
        exit(1);
    }

    ptr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    inet_ntop(res->ai_family, ptr, addrstr, 100);
    freeaddrinfo(res);
    config->target_ip = strdup(addrstr);
    if (!config->target_ip) {
        fprintf(stderr, "ft_ping: error: memory allocation failed\n");
        exit(1);
    }
}

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
    else {
        resolve_ip(config);
        struct in_addr addr;
        if (inet_pton(AF_INET, config->target_ip, &addr) != 1) {
            fprintf(stderr, "ft_ping: error: invalid IP address '%s'\n", config->target_ip);
            exit(1);
        }
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