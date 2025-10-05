#include "../include/ft_ping.h"

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS] destination\n\n", program_name);
    printf("Options:\n");
    printf("  -h, --help              Show this help message and exit\n");
    printf("  -V, --version           Show version information and exit\n");
    printf("  -c, --count NUMBER      Stop after sending NUMBER packets\n");
    printf("  -i, --interval SECONDS  Wait SECONDS between each packet (root only for <0.2)\n");
    printf("  -I, --source IP         Set source IP address\n");
    printf("  -s, --size SIZE         Send SIZE data bytes (default: 56)\n");
    printf("  -t, --ttl TTL           Set IP Time To Live\n");
    printf("  -W, --timeout SECONDS   Time to wait for response in seconds\n");
    printf("  -w, --deadline SECONDS  Stop after SECONDS regardless of packets sent/received\n");
    printf("  -q, --quiet             Quiet output (only summary)\n");
    printf("  -v, --verbose           Verbose output\n");
    printf("  -n, --no-dns            Do not resolve hostnames\n");
    printf("  -a, --audible           Audible ping (beep on reply)\n");
    printf("  -f, --flood             Flood ping (root only)\n");
    printf("  -p, --pattern PATTERN   Fill packet with pattern (hex)\n");
    printf("  -D, --timestamp         Print timestamp before each line\n");
    printf("  -Q, --type-of-service TOS Set Type Of Service\n");
    printf("  -M, --dont-fragment     Set Don't Fragment flag\n");
    printf("  -R, --record-route      Record route (not supported everywhere)\n");
    printf("  -S, --statistics        Print statistics\n");
    printf("  -C, --continuous SEC    Continuous ping for specified seconds\n");
    printf("\nExamples:\n");
    printf("  %s google.com\n", program_name);
    printf("  %s -c 5 -i 0.5 -s 1000 8.8.8.8\n", program_name);
    printf("  %s -v -n -t 64 example.com\n", program_name);
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
    printf("  Flood: %d\n", config->flood);
    printf("  Pattern: 0x%02X\n", config->pattern);
    printf("  Timestamp: %d\n", config->timestamp);
}