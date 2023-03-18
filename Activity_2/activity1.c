#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PIPE_NAME "my_pipe"

uint8_t operand1, operand2;
char operation;

void generate_operands_and_operation() {
    operand1 = rand() % 101;
    operand2 = rand() % 101;
    int op = rand() % 4;
    switch(op) {
        case 0: operation = '+'; break;
        case 1: operation = '-'; break;
        case 2: operation = '*'; break;
        case 3: operation = '/'; break;
    }
}

void parent_process(int num_ops) {
    int fd;
    mkfifo(PIPE_NAME, 0666);
    fd = open(PIPE_NAME, O_WRONLY);
    for(int i=0; i<num_ops; i++) {
        generate_operands_and_operation();
        write(fd, &operand1, sizeof(operand1));
        write(fd, &operation, sizeof(operation));
        write(fd, &operand2, sizeof(operand2));
    }
    close(fd);
}

void child_process() {
    int fd;
    uint8_t op1, op2;
    char op;
    fd = open(PIPE_NAME, O_RDONLY);
    while(read(fd, &op1, sizeof(op1)) > 0) {
        read(fd, &op, sizeof(op));
        read(fd, &op2, sizeof(op2));
        printf("%d %c %d = ", op1, op, op2);
        switch(op) {
            case '+': printf("%d\n", op1+op2); break;
            case '-': printf("%d\n", op1-op2); break;
            case '*': printf("%d\n", op1*op2); break;
            case '/': printf("%d\n", op1/op2); break;
        }
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: %s <num_ops>\n", argv[0]);
        return 1;
    }
    int num_ops = atoi(argv[1]);
    pid_t pid = fork();
    if(pid == -1) {
        perror("fork");
        return 1;
    } else if(pid == 0) {
        child_process();
    } else {
        parent_process(num_ops);
    }
    return 0;
}
