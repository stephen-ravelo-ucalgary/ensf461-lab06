#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0

int pid = 0;
int is_defined = FALSE;

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

uint32_t *mydefine(int off, int pfn, int vpn) {
    fprintf(output_file, "Current PID: %d. Memory instantiation complete. OFF bits: %d. PFN bits: %d. VPN bits: %d\n",
        pid,
        off,
        pfn,
        vpn
    );
    return (uint32_t *)calloc(pow(2, off + pfn), sizeof(uint32_t));
}

void myctxswitch(int newpid) {
    pid = newpid;
    fprintf(output_file, "Current PID: %d. Switched execution context to process: %d\n",
        pid,
        newpid
    );
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

    int is_defined = 0;

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
        char** tokens = tokenize_input(buffer);

        // TODO: Implement your memory simulator
        // int point = 1;

        // printf("DEBUG: Point %d\n", point++);
        if (tokens[0][0] == '%') {
            // printf("DEBUG: Point %d\n", point++);
            continue;
        }

        if (strcmp(tokens[0], "define") == 0) {
            // printf("DEBUG: Point %d\n", point++);
            if (is_defined) {
                fprintf(output_file, "Current PID: %d. Error: multiple calls to define in the same trace\n", pid);
                break;
            }
            mydefine(atoi(tokens[1]), atoi(tokens[2]), atoi(tokens[3]));
            is_defined = 1;
        }
        else if (strcmp(tokens[0], "ctxswitch") == 0) {
            int newpid = atoi(tokens[1]);
            if (newpid < 0 || newpid > 3) {
                fprintf(output_file, "Current PID: %d. Invalid context switch to process %d\n",
                    pid,
                    newpid
                );
                break;
            }
            myctxswitch(newpid);
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
