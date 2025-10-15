#include "../include/ft_ping.h"

void print_usage() {
    printf("Usage: Ft_ping [OPTIONS] destination\n\n");
    printf("Options:\n");
    printf("  -h, --help              Show this help message and exit\n");
    printf("  -V, --version           Show version information and exit\n");
    printf("  -c, --count NUMBER      Stop after sending NUMBER packets\n");
    printf("  -i, --interval SECONDS  Wait SECONDS between each packet\n");
    printf("  -I, --source IP         Set source IP address\n");
    printf("  -s, --size SIZE         Send SIZE data bytes (default: 56)\n");
    printf("  -t, --ttl TTL           Set IP Time To Live\n");
    printf("  -W, --timeout SECONDS   Time to wait for response in seconds\n");
    printf("  -w, --deadline SECONDS  Stop after SECONDS regardless of packets sent/received\n");
    printf("  -v, --verbose           Verbose output\n");
    printf("  -q, --quiet             Quiet output (only summary)\n");
    printf("  -n, --no-dns            Do not resolve hostnames\n");
    printf("\nExamples:\n");
    printf("  Ft_ping google.com\n");
    printf("  Ft_ping -c 5 -i 0.5 -s 1000 8.8.8.8\n");
    printf("  Ft_ping -v -n -t 64 example.com\n");
}

void print_config(t_ping_config *config) {
    printf("PING configuration:\n");
    printf("  Target: %s\n", config->target);
    printf("  Count: %d\n", config->count);
    printf("  Interval: %d microseconds\n", config->interval);
    printf("  Size: %d bytes\n", config->size);
    printf("  TTL: %d\n", config->ttl);
    printf("  Timeout: %d seconds\n", config->timeout);
    printf("  Verbose: %d\n", config->verbose);
    printf("  Quiet: %d\n", config->quiet);
    printf("  No DNS: %d\n", config->no_dns);
}

void print_version(void) {
    printf("ft_ping custom implementation v1.0\n");
    printf("Based on UNIX ping utilities\n");
    exit(EXIT_SUCCESS);
}