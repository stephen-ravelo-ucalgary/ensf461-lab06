#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0

#define NUM_PROCESSES 4

typedef struct _page_table_entry {
    int PFN;
    int PID;
    int valid;
} _page_table_entry;

// Output file
FILE* output_file;

// TLB replacement strategy (FIFO or LRU)
char* strategy;

char** tokenize_input(char* input) {
    char** tokens = NULL;
    char* token = strtok(input, " ");
    int num_tokens = 0;

    while (token != NULL) {
        num_tokens++;
        tokens = realloc(tokens, num_tokens * sizeof(char*));
        tokens[num_tokens - 1] = malloc(strlen(token) + 1);
        strcpy(tokens[num_tokens - 1], token);
        token = strtok(NULL, " ");
    }

    num_tokens++;
    tokens = realloc(tokens, num_tokens * sizeof(char*));
    tokens[num_tokens - 1] = NULL;

    return tokens;
}

int mypow(int x, int n) {
    int total = x;
    for (int i=0; i<n-1; i++) {
        total *= x;
    }
    return total;
}

int main(int argc, char* argv[]) {
    const char usage[] = "Usage: memsym.out <strategy> <input trace> <output trace>\n";
    char* input_trace;
    char* output_trace;
    char buffer[1024];

    // Parse command line arguments
    if (argc != 4) {
        printf("%s", usage);
        return 1;
    }
    strategy = argv[1];
    input_trace = argv[2];
    output_trace = argv[3];

    // Open input and output files
    FILE* input_file = fopen(input_trace, "r");
    output_file = fopen(output_trace, "w");  

    int pid = 0;
    int is_defined = FALSE;
    uint32_t *memory;
    _page_table_entry *page_tables[NUM_PROCESSES];

    while ( !feof(input_file) ) {
        // Read input file line by line
        char *rez = fgets(buffer, sizeof(buffer), input_file);
        if ( !rez ) {
            fprintf(stderr, "Reached end of trace. Exiting...\n");
            return -1;
        } else {
            // Remove endline character
            buffer[strlen(buffer) - 1] = '\0';
        }
        char** tokens = tokenize_input(buffer)
        char *instruction = tokens[0];

        // TODO: Implement your memory simulator
        if (instruction[0] == '%') {
            continue;
        }

        if (strcmp(instruction, "define") == 0) {
            if (is_defined) {
                fprintf(output_file, "Current PID: %d. Error: multiple calls to define in the same trace\n", pid);
                break;
            }

            int offset = atoi(tokens[1]);
            int pfn = atoi(tokens[2]);
            int vpn = atoi(tokens[3]);
            memory = (uint32_t *)calloc(mypow(2, offset + pfn), sizeof(uint32_t));

            int pages_per_process = mypow(2, vpn) / NUM_PROCESSES;

            for (int process_number = 0; process_number < NUM_PROCESSES; process_number++) {
                page_tables[process_number] = (_page_table_entry *)malloc(pages_per_process * sizeof(_page_table_entry));
                for (int page = 0; page < pages_per_process; page++) {
                    page_tables[process_number][page].PFN = 0;
                    page_tables[process_number][page].PID = process_number;
                    page_tables[process_number][page].valid = 0;
                }
            }

            fprintf(output_file, "Current PID: %d. Memory instantiation complete. OFF bits: %d. PFN bits: %d. VPN bits: %d\n", pid, offset, pfn, vpn);
            is_defined = 1;
        }
        else if (strcmp(instruction, "ctxswitch") == 0) {
            int newpid = atoi(tokens[1]);

            if (newpid < 0 || newpid > 3) {
                fprintf(output_file, "Current PID: %d. Invalid context switch to process %d\n", pid, newpid);
                break;
            }

            pid = newpid;
            fprintf(output_file, "Current PID: %d. Switched execution context to process: %d\n", pid, newpid);
        }
        else if (strcmp(instruction, "map") == 0) {
            
        }

        // Deallocate tokens
        for (int i = 0; tokens[i] != NULL; i++)
            free(tokens[i]);
        free(tokens);
    }

    // Close input and output files
    fclose(input_file);
    fclose(output_file);

    return 0;
}
