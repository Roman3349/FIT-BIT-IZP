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

/*
 * Autor: Roman Ondráček (login xondra58)
 * Implementované volitelní příkazy:
 *  - e (append EOL) přidej na konec aktuálního řádku znak konce řádku
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define COMMAND_SIZE 16

/**
 * Exit codes
 */
enum exitStatuses {
    NO_ERROR,
    INPUT_READ_ERROR,
    FILE_READ_ERROR,
    FILE_END,
    CONVERSION_ERROR,
    BUFFER_ERROR,
    UNKNOWN_COMMAND,
    NOT_FOUND
};

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
    CMD_SUBSTITUTE = 's',
    CMD_SUBSTITUTE_ALL = 'S',
    CMD_EOL = 'e'
//  CMD_FIND = 'f',
//  CMD_CONDITIONED_GOTO = 'c',
};

/**
 * Structure for commands
 */
typedef struct {
    enum commands cmd;
    char args[BUFFER_SIZE - 1];
} command_t;

/**
 * Structure for substitution commands
 */
typedef struct {
    int count;
    command_t cmds[COMMAND_SIZE];
} substituteCmds_t;

/**
 * Chack if execution has error status
 * @param status Execution status
 * @return Has error status?
 */
bool checkStatus(int status) {
    return (status != NO_ERROR);
}

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
 * @param count Count of the command repeats
 */
int getRepeatsCount(command_t command, long int *count) {
    char *endptr;
    if ((strlen(command.args) == 0)) {
        *count = 1;
        return NO_ERROR;
    }
    *count = strtol(command.args, &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "Error in string conversion to int.");
        return CONVERSION_ERROR;
    }
    return NO_ERROR;
}

/**
 * Create command from a line
 * @param char* line Line to parse
 * @param command_t command Parsed command
 */
command_t createCommand(char *line) {
    command_t command = {'\0', ""};
    command.cmd = line[0];
    strcpy(command.args, &line[1]);
    removeNewLine(command.args);
    return command;
}

/**
 * Replace string
 * @param source Source string buffer
 * @param pattern Pattern to replace
 * @param replacement Replacement
 * @return Execution status
 */
int strReplace(char *source, char *pattern, char *replacement) {
    char tmpBuffer[BUFFER_SIZE];
    char *p = strstr(source, pattern);
    if (p == NULL) {
        return NOT_FOUND;
    }
    unsigned long int rPos = p - source;
    strncpy(tmpBuffer, source, rPos);
    tmpBuffer[rPos] = '\0';
    strcat(tmpBuffer, replacement);
    strcat(tmpBuffer, p + strlen(pattern));
    strcpy(source, tmpBuffer);
    return NO_ERROR;
}

/**
 * Read line from stdin
 * @param inputBuffer Buffer for data from stdin
 * @return Execution status
 */
int readLine(char *inputBuffer) {
    if (fgets(inputBuffer, BUFFER_SIZE, stdin) == NULL) {
        if (feof(stdin)) {
            return FILE_END;
        }
        return INPUT_READ_ERROR;
    }
    removeNewLine(inputBuffer);
    return NO_ERROR;
}

/**
 * Add substitution command to the buffer
 * @param command Substitution command
 * @param substituteCmds Buffer with sunbstitution commands
 * @return Execution status
 */
int addSubstituteCommand(command_t command, substituteCmds_t *substituteCmds) {
    if (substituteCmds->count == COMMAND_SIZE) {
        fprintf(stderr, "Too much command for substitution.\n");
        return UNKNOWN_COMMAND;
    }
    substituteCmds->cmds[substituteCmds->count] = command;
    substituteCmds->count++;
    return NO_ERROR;
}

/**
 * Apply substitution command on the user's input
 * @param commands Substitution commands
 * @param buffer Buffer
 * @return Execution status
 */
int applySubstitutionCommands(substituteCmds_t *commands, char *buffer) {
    for (int i = 0; i < commands->count; i++) {
        command_t cmd = commands->cmds[i];
        char delimiter = cmd.args[0];
        if (delimiter == '\0') {
            break;
        }
        char pattern[BUFFER_SIZE] = "";
        char replacement[BUFFER_SIZE] = "";
        char tmpBuffer[BUFFER_SIZE] = "";
        strcpy(tmpBuffer, &cmd.args[1]);
        strncpy(pattern, tmpBuffer, strchr(tmpBuffer, delimiter) - tmpBuffer);
        strcpy(replacement, strchr(tmpBuffer, delimiter) + 1);
        int replacementStatus = NO_ERROR;
        if (cmd.cmd == CMD_SUBSTITUTE_ALL) {
            while (replacementStatus == NO_ERROR) {
                replacementStatus = strReplace(buffer, pattern, replacement);
            }
        } else {
            strReplace(buffer, pattern, replacement);
        }
    }
    commands->count = 0;
    return NO_ERROR;
}

/**
 * Command for line deletion
 * @param command Command
 * @return Execution status
 */
int commandDelete(command_t command) {
    long int count;
    if (getRepeatsCount(command, &count) == CONVERSION_ERROR) {
        return CONVERSION_ERROR;
    }
    char inputBuffer[BUFFER_SIZE] = "";
    for (int i = 0; i < count; i++) {
        int status = readLine(inputBuffer);
        if (checkStatus(status)) {
            return status;
        }
    }
    return NO_ERROR;
}

/**
 * Add EOL after the current line
 * @param command Command
 * @param outputBuffer Output buffer
 * @return Execution status
 */
int commandAddEol(char *outputBuffer) {
    if (strcat(outputBuffer, "\n") == NULL) {
        return BUFFER_ERROR;
    }
    return NO_ERROR;
}

/**
 * Inject prefix or suffix to user's input
 * @param command Command (append or before)
 * @param outputBuffer Output buffer
 * @return Execution status
 */
int commandInject(command_t command, char *outputBuffer) {
    char buffer[BUFFER_SIZE];
    if (strcpy(buffer, outputBuffer) == NULL) {
        return BUFFER_ERROR;
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
    return NO_ERROR;
}

/**
 * Command for line insertion
 * @param command Command
 * @return Execution status
 */
int commandInsert(command_t command) {
    puts(command.args);
    return NO_ERROR;
}

/**
 * Command for moving to line in the file with commands
 * @param command Command
 * @param file File with commands
 * @return Execution status
 * @todo Add cycle detection
 */
int commandGoto(command_t command, FILE *file) {
    long int line;
    if (getRepeatsCount(command, &line) == CONVERSION_ERROR) {
        return CONVERSION_ERROR;
    }
    char buffer[BUFFER_SIZE];
    rewind(file);
    for (int i = 1; i < line; i++) {
        if (fgets(buffer, BUFFER_SIZE - 1, file) == NULL) {
            return INPUT_READ_ERROR;
        }
    }
    return NO_ERROR;
}

/**
 * Command for line(s) printing
  * @param command Command
  * @param beforeBuffer Before output buffer
  * @param afterBuffer After output buffer
  * @return Execution status
  */
int
commandNext(command_t command, char *beforeBuffer, char *afterBuffer, bool *newLine, substituteCmds_t *substituteCmds) {
    long int count;
    if (getRepeatsCount(command, &count) == CONVERSION_ERROR) {
        return CONVERSION_ERROR;
    }
    char inputBuffer[BUFFER_SIZE] = "";
    char outputBuffer[4 * BUFFER_SIZE] = "";
    for (int i = 0; i < count; i++) {
        int status = readLine(inputBuffer);
        if (checkStatus(status)) {
            return status;
        }
        if (*newLine) {
            commandAddEol(afterBuffer);
        } else {
            *newLine = true;
        }
        sprintf(outputBuffer, "%s%s%s", beforeBuffer, inputBuffer, afterBuffer);
        afterBuffer[0] = beforeBuffer[0] = inputBuffer[0] = '\0';
        int substitutionStatus = applySubstitutionCommands(substituteCmds, outputBuffer);
        if (substitutionStatus != NO_ERROR) {
            break;
        }
        printf("%s", outputBuffer);
        outputBuffer[0] = '\0';
    }
    return NO_ERROR;
}

/**
 * Parse commands from the file
 * @param commandFile File with commands
 * @return Execution status
 */
int parseCommands(FILE *commandFile) {
    char commandBuffer[BUFFER_SIZE] = "";
    char beforeBuffer[BUFFER_SIZE] = "";
    char afterBuffer[BUFFER_SIZE] = "";
    // Dirty substitution command solution
    substituteCmds_t substituteCmds;
    substituteCmds.count = 0;
    bool newLine = true;
    int status = NO_ERROR;
    while (fgets(commandBuffer, BUFFER_SIZE - 2, commandFile) != NULL) {
        command_t command = createCommand(commandBuffer);
        switch (command.cmd) {
            case CMD_APPEND:
                status = commandInject(command, afterBuffer);
                break;
            case CMD_BEFORE:
                status = commandInject(command, beforeBuffer);
                break;
            case CMD_INSERT:
                status = commandInsert(command);
                break;
            case CMD_DELETE:
                status = commandDelete(command);
                break;
            case CMD_REMOVE:
                newLine = false;
                break;
            case CMD_NEXT:
                status = commandNext(command, beforeBuffer, afterBuffer, &newLine, &substituteCmds);
                break;
            case CMD_GOTO:
                status = commandGoto(command, commandFile);
                break;
            case CMD_EOL:
                status = commandAddEol(afterBuffer);
                break;
            case CMD_SUBSTITUTE:
            case CMD_SUBSTITUTE_ALL:
                status = addSubstituteCommand(command, &substituteCmds);
                break;
            case CMD_QUIT:
                return NO_ERROR;
            default:
                fprintf(stderr, "Unknown command: %c.\n", command.cmd);
                return UNKNOWN_COMMAND;
        }
        if (checkStatus(status)) {
            return status;
        }
    }
    char inputBuffer[BUFFER_SIZE] = "";
    do {
        status = readLine(inputBuffer);
        if (checkStatus(status)) {
            return status;
        }
        puts(inputBuffer);
    } while (!feof(stdin));
    return NO_ERROR;
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
        return FILE_READ_ERROR;
    }
    int output = parseCommands(commandFile);
    fclose(commandFile);
    if (output == FILE_END) {
        return NO_ERROR;
    }
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
    return NO_ERROR;
}

/**
 * Main program function
 * @param argc Count of arguments
 * @param argv Program's arguments
 * @return Execution status
 */
int main(int argc, char *argv[]) {
    if ((argc != 2) || (strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0)) {
        return printUsage();
    }
    return parseCommandFile(argv);
}
