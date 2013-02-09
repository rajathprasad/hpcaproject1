#include <cstdio>
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "cachesim.hpp"

void print_help_and_exit(void) {
    printf("cachesim [OPTIONS] < traces/file.trace\n");
    printf("  -c C\t\tTotal size in bytes is 2^C\n");
    printf("  -b B\t\tSize of each block in bytes is 2^B\n");
    printf("  -s S\t\tNumber of blocks per set is 2^S\n");
    printf("  -f E|B\tFetch policy\n");
    printf("  -r L|N\tReplacement policy\n");
    printf("  -h\t\tThis helpful output\n");
    exit(0);
}

void print_statistics(cache_stats_t* p_stats);

int main(int argc, char* argv[]) {
    int opt;
    uint64_t c = DEFAULT_C;
    uint64_t b = DEFAULT_B;
    uint64_t s = DEFAULT_S;
    char f     = DEFAULT_F;
    char r     = DEFAULT_R;

    /* Read arguments */ 
    while(-1 != (opt = getopt(argc, argv, "c:b:s:f:r:h"))) {
        switch(opt) {
        case 'c':
            c = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'f':
            if(optarg[0] == BLOCKING || optarg[0] == EAGER) {
                f = optarg[0];
            }
            break;
        case 'r':
            if(optarg[0] == LRU || optarg[0] == NMRU_FIFO) {
                r = optarg[0];
            }
            break;
        case 'h':
            /* Fall through */
        default:
            print_help_and_exit();
            break;
        }
    }

    printf("Cache Settings\n");
    printf("C: %" PRIu64 "\n", c);
    printf("B: %" PRIu64 "\n", b);
    printf("S: %" PRIu64 "\n", s);
    printf("F: %s\n", f == BLOCKING ? "BLOCKING" : "EAGER");
    printf("R: %s\n", r == LRU ? "LRU" : "NMRU_FIFO");
    printf("\n");

    /* Setup the cache */
    setup_cache(c, b, s, f, r);

    /* Setup statistics */
    cache_stats_t stats;
    memset(&stats, 0, sizeof(cache_stats_t));

    /* Begin reading the file */ 
    char rw;
    uint64_t address;
    while (!feof(stdin)) { 
        int ret = fscanf(stdin, "%c %" PRIx64 "\n", &rw, &address); 
        if(ret == 2) {
            cache_access(rw, address, &stats); 
        }
    }

    complete_cache(&stats);

    print_statistics(&stats);

    return 0;
}

void print_statistics(cache_stats_t* p_stats) {
    printf("Cache Statistics\n");
    printf("Accesses: %" PRIu64 "\n", p_stats->accesses);
    printf("Reads: %" PRIu64 "\n", p_stats->reads);
    printf("Read hits: %" PRIu64 "\n", p_stats->read_hits);
    printf("Read misses: %" PRIu64 "\n", p_stats->read_misses);
    printf("Writes: %" PRIu64 "\n", p_stats->writes);
    printf("Write hits: %" PRIu64 "\n", p_stats->write_hits);
    printf("Write misses: %" PRIu64 "\n", p_stats->write_misses);
    printf("Misses: %" PRIu64 "\n", p_stats->misses);
    printf("Hit Time: %" PRIu64 "\n", p_stats->hit_time);
    printf("Miss Penalty: %" PRIu64 "\n", p_stats->miss_penalty);
    printf("Miss rate: %f\n", p_stats->miss_rate);
    printf("Average access time (AAT): %f\n", p_stats->avg_access_time);
    printf("Storage Overhead: %" PRIu64 "\n", p_stats->storage_overhead);
    printf("Storage Overhead Ratio: %f\n", p_stats->storage_overhead_ratio);
}

