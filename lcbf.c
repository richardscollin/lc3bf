#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_LBL_SIZE 64
#define MAX_STACK_SIZE 100
#define MAX_ASM_LENGTH 200
#define MAX_FILE_NAME 50

void fatal_error(char* mesg);
int uniq_label_name();
int write_begin_boiler_plate(FILE *fd);
int write_end_boiler_plate(FILE *fd);

char *create_asm(char sym, char *assembly);
char *inc(char *assembly);
char *dec(char *assembly);
char *rshift(char *assembly);
char *lshift(char *assembly);
char *lbracket(char *assembly);
char *rbracket(char *assembly);
char *gen_print(char *assembly);
char *gen_read(char *assembly);


int stack_ptr = 0;
int stack[MAX_STACK_SIZE];

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fatal_error("Usage: lcbf progam.bf");
    }

    FILE *bf = fopen(argv[1], "r");
    if (bf == NULL) {
        fatal_error("Could not open brainfuck file");
    }

    FILE *lc3asm = fopen("bf.asm", "w");
    if (lc3asm == NULL) {
        fatal_error("Could not open file for writing.");
    }
     
    write_begin_boiler_plate(lc3asm);

    int c;
    while ((c = fgetc(bf)) != EOF) {
        char temp[MAX_ASM_LENGTH];
        char *asm_string = temp;
        asm_string = create_asm(c, asm_string);
        fputs(asm_string,lc3asm);
    }

    write_end_boiler_plate(lc3asm);

    fclose(bf);
    fclose(lc3asm);
   return 0;
}

void fatal_error(char * mesg) {
    puts(mesg);
    exit(1);
}

int uniq_label_name() {
    static int lbl_num = 0;
    return lbl_num++;
}

int write_begin_boiler_plate(FILE *fd) {
    fputs(".ORIG x3000\n", fd);
    fputs("LEA R1, PROGRAM_STACK\n", fd);
    return 0;
}

int write_end_boiler_plate(FILE *fd) {
    fputs("HALT\n", fd);
    fputs("PROGRAM_STACK .fill 0\n", fd);
    fputs(".END\n", fd);
    return 0;
}

char *create_asm(char sym, char *assembly) {
    switch (sym) {
        case '+':
            assembly = inc(assembly);
            break;
        case '-':
            assembly = dec(assembly);
            break;
        case '>':
            assembly = rshift(assembly);
            break;
        case '<':
            assembly = lshift(assembly);
            break;
        case '[':
            assembly = lbracket(assembly);
            break;
        case ']':
            assembly = rbracket(assembly);
            break;
        case '.':
            assembly = gen_print(assembly);
            break;
        case ',':
            assembly = gen_read(assembly);
            break;
        default:
            assembly = "";
            break;
    }
    return assembly;
}

char *inc(char *assembly) {
    assembly = "LDR R2, R1, 0\n"
               "ADD R2, R2, 1\n"
               "STR R2, R1, 0\n";
    return assembly;
}

char *dec(char *assembly) {
    assembly = "LDR R2, R1, 0\n"
               "ADD R2, R2, -1\n"
               "STR R2, R1, 0\n";
    return assembly;
}
char *rshift(char *assembly) {
    assembly = "ADD R1, R1, 1\n";
    return assembly;
}
char *lshift(char *assembly) {
    assembly = "ADD R1, R1, -1\n";
    return assembly;
}
char *lbracket(char *assembly) {
    int lbl_n = uniq_label_name();
    stack[stack_ptr++] = lbl_n;

    char temp1[MAX_LBL_SIZE];
    char *labelstart = temp1;
    sprintf(labelstart,"START%d",lbl_n);

    char temp2[MAX_LBL_SIZE];
    char *labelend = temp2;
    sprintf(labelend,"END%d",lbl_n);

    sprintf(assembly,
    "LDR R2, R1, 0\n"
    "ADD R2, R2, 0\n"
    "%s BRZ %s\n", labelstart, labelend);

    return assembly;
}
char *rbracket(char *assembly) {
    int lbl_n = stack[--stack_ptr];

    char temp1[MAX_LBL_SIZE];
    char *labelstart = temp1;
    sprintf(labelstart,"START%d",lbl_n);

    char temp2[MAX_LBL_SIZE];
    char *labelend = temp2;
    sprintf(labelend,"END%d",lbl_n);

    sprintf(assembly,
    "LDR R2, R1, 0\n"
    "ADD R2, R2, 0\n"
    "%s BRNP %s\n", labelend, labelstart);

    return assembly;
}
char *gen_print(char *assembly) {
    assembly = "LDR R0, R1, 0\n"
               "TRAP x21\n";
    return assembly;
}
char *gen_read(char *assembly) {
    assembly = "TRAP x20\n"
               "STR R0, R1, 0\n";
    return assembly;
}

