#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void get_grade(int mark, char *grade) {
    if (mark > 74) {
        *grade = 'A';
    } else if (mark > 60) {
        *grade = 'B';
    } else if (mark > 45) {
        *grade = 'C';
    } else if (mark > 34) {
        *grade = 'D';
    } else {
        *grade = 'E';
    }
}

int main() {
    int marks[3];
    int pipes[3][2];
    pid_t pids[3];
    int i;

    for (i = 0; i < 3; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    printf("Enter marks for three subjects:\n");
    for (i = 0; i < 3; i++) {
        printf("Mark for subject %d: ", i + 1);
        scanf("%d", &marks[i]);
    }

    for (i = 0; i < 3; i++) {
        if ((pids[i] = fork()) == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pids[i] == 0) {
            close(pipes[i][1]);
            int mark;
            read(pipes[i][0], &mark, sizeof(int));
            close(pipes[i][0]);
            char grade;
            get_grade(mark, &grade);
            printf("Child process %d received mark %d and calculated grade %c\n", getpid(), mark, grade);
            exit(grade);
        }
    }

    for (i = 0; i < 3; i++) {
        close(pipes[i][0]);
        write(pipes[i][1], &marks[i], sizeof(int));
        close(pipes[i][1]);
    }

    for (i = 0; i < 3; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        char grade = WEXITSTATUS(status);
        printf("Subject %d: Mark = %d, Grade = %c (processed by child %d)\n", i + 1, marks[i], grade, pids[i]);
    }

    return 0;
}
