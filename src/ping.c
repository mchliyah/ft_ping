#include "../include/ft_ping.h"

static t_ping_config *g_config = NULL;

void int_handler(int dummy) {
    (void)dummy;
    if (g_config)
        g_config->keep_running = 0;
}

uint16_t checksum(void *b, int len) {    
    uint16_t *buf = b;
    uint32_t sum = 0;
    uint16_t result;

    for (; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(uint8_t *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void create_socket(t_ping_config *config) {
    config->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (config->sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set a receive timeout to avoid blocking indefinitely
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(config->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Setting receive timeout failed");
    }

    memset(&config->addr, 0, sizeof(config->addr));
    config->addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, config->target_ip, &config->addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid target IP address: %s\n", config->target_ip);
        exit(EXIT_FAILURE);
    }
    config->addr.sin_port = 0; // Ensure destination port is zero
    config->addr_len = sizeof(config->addr);

    if (config->ttl > 0) {
        if (setsockopt(config->sockfd, IPPROTO_IP, IP_TTL, &config->ttl, sizeof(config->ttl)) < 0) {
            perror("Setting TTL option failed");
        }
    }
    if (config->tos > 0) {
        if (setsockopt(config->sockfd, IPPROTO_IP, IP_TOS, &config->tos, sizeof(config->tos)) < 0) {
            perror("Setting TOS option failed");
        }
    }
    if (config->dont_fragment != -1) {
        if (setsockopt(config->sockfd, IPPROTO_IP, IP_MTU_DISCOVER, &config->dont_fragment, sizeof(config->dont_fragment)) < 0) {
            perror("Setting Don't Fragment option failed");
        }
    }

    if (config->source_ip) {
        struct sockaddr_in src_addr;
        memset(&src_addr, 0, sizeof(src_addr));
        src_addr.sin_family = AF_INET;
        if (inet_pton(AF_INET, config->source_ip, &src_addr.sin_addr) != 1) {
            fprintf(stderr, "Invalid source IP address: %s\n", config->source_ip);
            exit(EXIT_FAILURE);
        }
        src_addr.sin_port = 0;
        if (bind(config->sockfd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
            exit(EXIT_FAILURE);
        }
        if (setsockopt(config->sockfd, IPPROTO_IP, IP_PKTINFO, &src_addr, sizeof(src_addr)) < 0) {
            perror("Setting Source IP option failed");
        }
        printf("PING %s (%s) from (%s):\n", config->target, config->target_ip, config->source_ip);
    }
}

void send_icmp_echo_request(t_ping_config *config, int sequence) {
    char packet[sizeof(struct icmphdr) + config->size];
    struct icmphdr *icmp_hdr = (struct icmphdr *)packet;
    memset(packet, 0, sizeof(packet));

    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->un.echo.id = htons(getpid() & 0xFFFF);
    icmp_hdr->un.echo.sequence = htons(sequence);
    memset(packet + sizeof(struct icmphdr), config->pattern, config->size);

    icmp_hdr->checksum = 0;
    icmp_hdr->checksum = checksum((unsigned short *)packet, sizeof(packet));

    ssize_t bytes_sent = sendto(config->sockfd, packet, sizeof(packet), 0,
                                (struct sockaddr *)&config->addr, config->addr_len);
    if (bytes_sent < 0) {
        if (config->verbose)
            perror("Failed to send ICMP Echo Request");
        return;
    }
    if (config->verbose)
        printf("Sent %zd bytes to %s\n", bytes_sent, config->target_ip);
}

void receive_icmp_echo_reply(t_ping_config *config, struct timeval send_time) {
    char buffer[MAX_PACKET_SIZE];
    struct sockaddr_in reply_addr;
    socklen_t addr_len = sizeof(reply_addr);
    ssize_t bytes_received = recvfrom(config->sockfd, buffer, sizeof(buffer), 0,
                                      (struct sockaddr *)&reply_addr, &addr_len);
    if (bytes_received < 0) {
        if (config->verbose)
            perror("Failed to receive ICMP Echo Reply");
        return;
    }

    struct timeval recv_time;
    gettimeofday(&recv_time, NULL);
    double rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0 +
                 (recv_time.tv_usec - send_time.tv_usec) / 1000.0;

    struct iphdr *ip_hdr = (struct iphdr *)buffer;
    struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));
    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &reply_addr.sin_addr, addr_str, sizeof(addr_str));

    if (icmp_hdr->type == ICMP_ECHOREPLY) {
        if (!config->quiet) {
            if (config->timestamp) {
                struct timeval tv;
                gettimeofday(&tv, NULL);
                struct tm *tm_info = localtime(&tv.tv_sec);
                char time_buffer[64];
                strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", tm_info);
                printf("[%s.%06ld] ", time_buffer, tv.tv_usec);
            }
            if (config->verbose)
                printf("%zd bytes from %s: icmp_seq=%d ident=%d ttl=%d time=%.1f ms\n",
                       bytes_received - (ip_hdr->ihl * 4),
                       addr_str,
                       ntohs(icmp_hdr->un.echo.sequence),
                       ntohs(icmp_hdr->un.echo.id),
                       ip_hdr->ttl,
                       rtt);
            else
                printf("%zd bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n",
                       bytes_received - (ip_hdr->ihl * 4),
                       addr_str,
                       ntohs(icmp_hdr->un.echo.sequence),
                       ip_hdr->ttl,
                       rtt);
        }
        config->stats.received++;
        config->stats.rtt_sum += rtt;
        if (rtt < config->stats.rtt_min)
            config->stats.rtt_min = rtt;
        if (rtt > config->stats.rtt_max)
            config->stats.rtt_max = rtt;
    }
    else if (icmp_hdr->type == ICMP_TIME_EXCEEDED) {
        struct iphdr *inner_ip = (struct iphdr *)(buffer + (ip_hdr->ihl * 4) + sizeof(struct icmphdr));
        struct icmphdr *inner_icmp = (struct icmphdr *)((char *)inner_ip + (inner_ip->ihl * 4));
        int seq = ntohs(inner_icmp->un.echo.sequence);
        if (!config->quiet)
            printf("From %s icmp_seq=%d Time to live exceeded\n", addr_str, seq);
    }
    else {
        if (config->verbose)
            fprintf(stderr, "Received non-echo reply ICMP packet of type %d\n", icmp_hdr->type);
    }
}

void print_summary(t_ping_config *config) {
    struct timeval end_time;
    gettimeofday(&end_time, NULL);
    long total_time = (end_time.tv_sec - config->stats.start_time.tv_sec) * 1000 +
                      (end_time.tv_usec - config->stats.start_time.tv_usec) / 1000;
    int loss = config->stats.transmitted - config->stats.received;
    int loss_percent = config->stats.transmitted > 0 ? (loss * 100) / config->stats.transmitted : 0;

    printf("\n--- %s ping statistics ---\n", config->target_ip);
    printf("%d packets transmitted, %d received, %d%% packet loss, time %ldms\n",
           config->stats.transmitted, config->stats.received, loss_percent, total_time);
    if (config->stats.received > 0) {
        double avg = config->stats.rtt_sum / config->stats.received;
        printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n",
               config->stats.rtt_min, avg, config->stats.rtt_max);
    }
}

void check_limits(t_ping_config *config) {
    if (config->count > 0 && config->stats.transmitted >= config->count) {
        config->keep_running = 0;
    }
    if (config->deadline > 0) {
        struct timeval now;
        gettimeofday(&now, NULL);
        long elapsed = (now.tv_sec - config->stats.start_time.tv_sec);
        if (elapsed >= config->deadline) {
            config->keep_running = 0;
        }
    }
}

void ping(t_ping_config *config) {
    g_config = config;
    signal(SIGINT, int_handler);

    create_socket(config);
    config->stats.transmitted = 0;
    config->stats.received = 0;
    config->stats.rtt_min = 1e9;
    config->stats.rtt_max = 0;
    config->stats.rtt_sum = 0;
    gettimeofday(&config->stats.start_time, NULL);
    config->keep_running = 1;

    V_PRINT(config, 1, "ping: socket fd %d socket type %d protocol %d\n",
            config->sockfd, SOCK_RAW, IPPROTO_ICMP);

    int seq = 1;
    printf("PING %s (%s) %d(%zu) bytes of data.\n",
           config->target,
           config->target_ip,
           config->size,
           sizeof(struct icmphdr) + config->size);
    while (config->keep_running) {
        struct timeval send_time;
        gettimeofday(&send_time, NULL);
        send_icmp_echo_request(config, seq);
        config->stats.transmitted++;
        receive_icmp_echo_reply(config, send_time);
        seq++;
        usleep(config->interval);
        check_limits(config);
    }
    print_summary(config);
    close(config->sockfd);
}
