#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PIPE_NAME "pipe"

uint8_t number1;
uint8_t number2;
char operation;

void get_operation() {
    number1 = rand() % 101;
    number2 = rand() % 101;
    int op = rand() % 4;
    switch(op) {
        case 0: operation = '+'; break;
        case 1: operation = '-'; break;
        case 2: operation = '*'; break;
        case 3: operation = '/'; break;
    }
}

void parent(int total_operations) {
    int p;

    mkfifo(PIPE_NAME, 0666);
    p = open(PIPE_NAME, O_WRONLY);
    
    for(int i = 0; i < total_operations; i++) {
        get_operation();
        
        write(p, &number1, sizeof(number1));
        write(p, &operation, sizeof(operation));
        write(p, &number2, sizeof(number2));
    }
    close(p);
}

void child() {
    int c;
    uint8_t first_operand, second_operand;
    char sign;
    c = open(PIPE_NAME, O_RDONLY);
    while(read(c, &first_operand, sizeof(first_operand)) > 0) {
        read(c, &sign, sizeof(sign));
        read(c, &second_operand, sizeof(second_operand));
        printf("%d %c %d = ", first_operand, sign, second_operand);
        switch(sign) {
            case '+': printf("%d\n", first_operand + second_operand); break;
            case '-': printf("%d\n", first_operand - second_operand); break;
            case '*': printf("%d\n", first_operand * second_operand); break;
            case '/': printf("%d\n", first_operand / second_operand); break;
        }
    }
    close(c);
}
int main(int argc, char *argv[]) {
    
    if(argc != 2) {
        printf("Usage: %s <total_operations>\n", argv[0]);
        return 1;
    }
    int total_operations = atoi(argv[1]);
    pid_t pid = fork();
    if(pid == -1) {
        perror("fork");
        return 1;
    } else if(pid == 0) {
        child();
    } else {
        
        parent(total_operations);
        printf("RUEDA**");
    }
    return 0;
}
