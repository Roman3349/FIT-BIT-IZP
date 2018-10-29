/*
 * Copyright (C) 2018  Roman Ondráček <xondra58@stud.fit.vutbr.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

/**
 * @todo Add enum with return codes
 */

/**
 * Commands
 */
enum commands {
    CMD_INSERT = 'i',
    CMD_APPEND = 'a',
    CMD_BEFORE = 'b',
    CMD_DELETE = 'd',
    CMD_REMOVE = 'r',
    CMD_NEXT = 'n',
    CMD_QUIT = 'q',
    CMD_GOTO = 'g',
//    CMD_SUBSTITUTE = 's',
//    CMD_SUBSTITUTE_ALL = 'S',
//|  CMD_FIND = 'f',
//|  CMD_CONDITIONED_GOTO = 'c',
//|  CMD_EOL = 'e'
};

/**
 * @todo Add command for substitution
 */

/**
 * Structure for commands
 */
typedef struct {
    enum commands cmd;
    char args[BUFFER_SIZE - 1];
} command_t;

/**
 * Remove a new line from the string
 * @param str String
 */
void removeNewLine(char *str) {
    size_t pos = strcspn(str, "\n");
    str[pos] = '\0';
}

/**
 * Get count of the command repeats
 * @param command Command to cast
 * @return Count of the command repeats
 */
long int getRepeatsCount(command_t command) {
    char *endptr;
    if ((strlen(command.args) == 0)) {
        return 1;
    }
    long int count = strtol(command.args, &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "Error in string conversion to int. String: %s\n", command.args);
        return -1;
    }
    return count;
}

/**
 * Create command from a line
 * @param char* line Line to parse
 * @param command_t command Parsed command
 */
command_t createCommand(char *line) {
    char cmdBuffer;
    command_t command = {'\0', ""};
    sscanf(line, " %c%s", &cmdBuffer, command.args);
    removeNewLine(command.args);
    command.cmd = cmdBuffer;
    return command;
}

/**
 * Read line from stdin
 * @param inputBuffer Buffer for data from stdin
 * @return Execution status
 */
int readLine(char *inputBuffer) {
    if (fgets(inputBuffer, BUFFER_SIZE, stdin) == NULL) {
        return 1;
    }
    removeNewLine(inputBuffer);
    return 0;
}

/**
 * Print string from zhe output buffer
 * @param outputBuffer Output buffer
 */
void flushOutputBuffer(char *outputBuffer) {
    if (strlen(outputBuffer) != 0) {
        puts(outputBuffer);
        outputBuffer[0] = '\0';
    }
}

/**
 * Inject prefix or suffix to user's input
 * @param command Command (append or before)
 * @param outputBuffer Output buffer
 * @return Execution status
 */
int commandInject(command_t command, char *outputBuffer) {
    char buffer[BUFFER_SIZE];
    if (strlen(outputBuffer) == 0) {
        readLine(buffer);
        if (feof(stdin)) {
            return -1;
        }
    } else {
        strcpy(buffer, outputBuffer);
    }
    switch (command.cmd) {
        case CMD_APPEND:
            sprintf(outputBuffer, "%s%s", buffer, command.args);
            break;
        case CMD_BEFORE:
            sprintf(outputBuffer, "%s%s", command.args, buffer);
            break;
        default:
            break;
    }
    return 0;
}

/**
 * Command for line deletion
 * @param command Command
 * @param inputBuffer Input buffer
 * @return Execution status
 */
int commandDelete(command_t command, char *inputBuffer) {
    long int count = getRepeatsCount(command);
    if (count == -1) {
        return 1;
    }
    for (int i = 0; i < count; i++) {
        readLine(inputBuffer);
        if (feof(stdin)) {
            return -1;
        }
    }
    return 0;
}

/**
 * Command for line insertion
 * @param command Command
 * @param inputBuffer Input buffer
 * @param outputBuffer Output buffer
 * @return Execution status
 */
int commandInsert(command_t command, char *inputBuffer, char *outputBuffer) {
    readLine(inputBuffer);
    if (feof(stdin)) {
        return -1;
    }
    puts(command.args);
    if (strcpy(outputBuffer, inputBuffer) == NULL) {
        return 1;
    }
    return 0;
}

/**
 * Command for moving to line in the file with commands
 * @param command Command
 * @param file File with commands
 * @return Execution status
 */
int commandGoto(command_t command, FILE *file) {
    long int line = getRepeatsCount(command);
    char buffer[BUFFER_SIZE];
    rewind(file);
    for (int i = 1; i < line; i++) {
        if (fgets(buffer, BUFFER_SIZE - 1, file) == NULL) {
            return 1;
        }
    }
    return 0;
}

/**
 * Command for line(s) printing
 * @param command Command
 * @param inputBuffer Input buffer
 * @return Execution status
 */
int commandNext(command_t command, char *inputBuffer) {
    long int count = getRepeatsCount(command);
    if (count == -1) {
        return 1;
    }
    for (int i = 0; i < count; i++) {
        readLine(inputBuffer);
        if (feof(stdin)) {
            return -1;
        }
        puts(inputBuffer);
    }
    return 0;
}

/**
 * Command to remove EOL from the current line
 * @param inputBuffer Input buffer
 * @param outputBuffer Output buffer
 * @return Execution status
 */
int commandRemove(char *inputBuffer, char *outputBuffer) {
    readLine(inputBuffer);
    if (feof(stdin)) {
        return -1;
    }
    char buffer[BUFFER_SIZE];
    strcpy(buffer, outputBuffer);
    removeNewLine(buffer);
    sprintf(outputBuffer, "%s%s", buffer, inputBuffer);
    return 0;
}

/**
 * Parse commands from the file
 * @param commandFile File with commands
 * @return Execution status
 */
int parseCommands(FILE *commandFile) {
    char commandBuffer[BUFFER_SIZE] = "";
    char inputBuffer[BUFFER_SIZE] = "";
    char outputBuffer[2 * BUFFER_SIZE] = "";
    int status = 0;
    while (fgets(commandBuffer, BUFFER_SIZE - 2, commandFile) != NULL) {
        command_t command = createCommand(commandBuffer);
        switch ((int) command.cmd) {
            case CMD_APPEND:
            case CMD_BEFORE:
                status = commandInject(command, outputBuffer);
                break;
            case CMD_INSERT:
                status = commandInsert(command, inputBuffer, outputBuffer);
                break;
            case CMD_REMOVE:
                status = commandRemove(inputBuffer, outputBuffer);
                break;
            case CMD_DELETE:
                flushOutputBuffer(outputBuffer);
                status = commandDelete(command, inputBuffer);
                break;
            case CMD_NEXT:
                /** @todo Fix command's behavior */
                flushOutputBuffer(outputBuffer);
                status = commandNext(command, inputBuffer);
                break;
            case CMD_GOTO:
                flushOutputBuffer(outputBuffer);
                status = commandGoto(command, commandFile);
                break;
            case CMD_QUIT:
                return 0;
            default:
                fprintf(stderr, "Unknown command: %c.", command.cmd);
                break;
        }
        if (status == -1) {
            return 0;
        }
        if (status != 0) {
            return status;
        }
    }
    do {
        readLine(inputBuffer);
        if (feof(stdin)) {
            return -1;
        }
        puts(inputBuffer);
    } while (!feof(stdin));
    return 0;
}

/**
 * Parse commands from file
 * @param argv Values of the program's parameters
 * @return Execution status
 */
int parseCommandFile(char *argv[]) {
    FILE *commandFile;
    if ((commandFile = fopen(argv[1], "r")) == NULL) {
        perror("proj1");
        return 1;
    }
    int output = parseCommands(commandFile);
    fclose(commandFile);
    return output;
}

/**
 * Print program's usage
 * @return Execution status
 */
int printUsage() {
    puts("Usage: ./proj1 [options] file_with_commands");
    puts("Options:");
    puts("\t-h, --help\t\tPrints help (this message) and exit");
    return 0;
}

/**
 * Main program function
 * @param int argc Count of arguments
 * @param char** argv Program's arguments
 * @return Execution status
 */
int main(int argc, char *argv[]) {
    if ((argc != 2) || (strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0)) {
        return printUsage();
    } else {
        return parseCommandFile(argv);
    }
}
