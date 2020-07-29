#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

void childFunction(char *line);

int main(int argc, char *argv[])
{
    int status;
    char *stdinBuffer;
    char *fileBuffer;
    const char firstMessage[] = "This program encodes text\n";
    const char enterMessage[] = "\nEnter a sentence, e.g., Good morning sir, my name is Big Foot\n";
    const char parentInitialMessage[] = "Parent says: Created a child to perform task, waiting...\n";
    const char emptyLineMessage[] = "Parent says: Empty line\n";

    // Prompts the message "This program encodes text"
    write(STDOUT_FILENO, firstMessage, sizeof(firstMessage));

    // Gets in an infinite loop
    while (1)
    {
        stdinBuffer = malloc(1024 * sizeof(char));
        // Writes the message "Enter a sentence, e.g., Good morning sir, my name is Big Foot"
        write(STDOUT_FILENO, enterMessage, sizeof(enterMessage));
        // Use read() to read the whole input line
        read(STDIN_FILENO, stdinBuffer, 1024);

        // Fork
        int pid = fork();
        if (pid == -1)
        {
            perror("impossible to fork");
            exit(1);
        }
        if (pid > 0) // Parent
        {
            // the parent writes the message "Created a child to perform task, waiting..."
            write(STDOUT_FILENO, parentInitialMessage, sizeof(parentInitialMessage));
            // the parent calls wait() to wait for its child
            if (wait(&status) >= 0)
            {
                if (WIFEXITED(status) && WEXITSTATUS(status) == 10)
                {
                    // Once the child terminates, the parent checks the returned status value and if it is 10,
                    // writes on the screen "Empty line" and goes back to 1
                    write(STDOUT_FILENO, emptyLineMessage, sizeof(emptyLineMessage));
                }
                else if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                {
                    // Otherwise, the parent opens file codes.txt for reading and reads the encoded line,
                    // prints it on the screen, closes codes.txt and goes back to 1
                    int fdr;
                    if ((fdr = open("codes.txt", O_RDONLY, 0777)) == -1)
                    {
                        perror("file problem");
                        exit(1);
                    }
                    fileBuffer = malloc(1024 * sizeof(char));
                    read(fdr, fileBuffer, 1024);
                    write(STDOUT_FILENO, fileBuffer, 1024);
                    close(fdr);
                }
            }
        }
        else if (pid == 0) // Child
        {
            // the child process calls the function childFunction(char *) and never returns
            childFunction(stdinBuffer);
        }
    }
}

void childFunction(char *line)
{
    // Writes the message "I am a child working for my parent"
    const char initialMessage[] = "Child says: I am a child working for my parent\n";
    write(STDOUT_FILENO, initialMessage, sizeof(initialMessage));

    // in case of an empty line, the child calls exit(10)
    if (line[1] == '\0')
        exit(10);

    // encodes the input line, for example the above sentence becomes "tooF giB si eman ym ,ris gninrom dooG"
    int i, j = 0, length = 0;
    while (line[length] != '\0')
    {
        length++;
    }
    char *invertedLine;
    invertedLine = malloc(1024 * sizeof(char));
    for (i = length - 1; i >= 0; i--)
    {
        invertedLine[j++] = line[i];
    }

    // opens the file called codes.txt, for writing and truncates it, to get rid of old contents
    int fdw;
    if ((fdw = open("codes.txt", O_CREAT | O_WRONLY | O_TRUNC, 0777)) == -1)
    {
        perror("file problem");
        exit(1);
    }

    // writes the encoded line in codes.txt, then closes the file
    write(fdw, invertedLine, length);
    close(fdw);

    // calls exit(0)
    exit(0);
}
