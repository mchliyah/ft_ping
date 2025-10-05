# include "../include/ft_ping.h"


// char* resolve_hostname(const char* hostname) {
//     struct addrinfo hints, *result;
//     char* ip_str = NULL;
    
//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_INET; // IPv4
//     hints.ai_socktype = SOCK_STREAM;
    
//     int status = getaddrinfo(hostname, NULL, &hints, &result);
//     if (status != 0) {
//         fprintf(stderr, "Error: Failed to resolve hostname '%s': %s\n", 
//                 hostname, gai_strerror(status));
//         return NULL;
//     }
    
//     if (result && result->ai_addr) {
//         struct sockaddr_in* addr_in = (struct sockaddr_in*)result->ai_addr;
//         ip_str = malloc(INET_ADDRSTRLEN);
//         if (ip_str) {
//             inet_ntop(AF_INET, &(addr_in->sin_addr), ip_str, INET_ADDRSTRLEN);
//             printf("Resolved %s to %s\n", hostname, ip_str);
//         }
//     }
    
//     freeaddrinfo(result);
//     return ip_str;
// }



int main(int ac, char **av) {
    t_ping_config config;
    
    INIT_CONFIG(&config);
    parse_arguments(ac, av, &config);

    if (config.verbose) {
        print_config(&config);
    }
    
    printf("PING %s: implementation in progress...\n", config.target);
    
    free_resources (&config);
    
    return 0;
}