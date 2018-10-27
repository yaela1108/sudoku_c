// -------------------------- includes -------------------------

#include "SudokuTree.h"
#include "GenericDFS.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

// -------------------------- macros -------------------------

//-----parameters for reading from file.
//argument index for the input file.
#define FILE_INDEX 1
//format for opening the input file.
#define FILE_FORMAT "r"
//expected number of arguments.
#define NUM_ARGS 2
//size of buffer for reading lines from input file.
#define BUFFER_SIZE 256

//-----parameters for checking input
//max size of board.
#define MAX_BOARD_SIZE 100
//board shape.
#define SQUARED 2

//-----parameters for printing board.
//end-line delimiter
#define END_LINE '\n'
//mid-line delimiter
#define SPACE ' '
//printing format.
#define PRINT_FORMAT "%u%c"
//printing format for board size.
#define PRINT_SIZE_FORMAT "%u\n"

//-----exit codes
//allocation error exit code.
#define MALLOC_EXIT -2
//invalid input error exit code
#define INVALID_INPUT_EXIT -3
//fclose error exit code.
#define FCLOSE_EXIT -4
//unexpected number of arguments exit code.
#define ARGC_EXIT -5
//fopen error exit code.
#define FOPEN_EXIT -6

//-----messages.
//invalid input error message.
#define INVALID_FILE_ERROR "%s:not a valid sudoku file\n"
//fopen error message,
#define FOPEN_ERROR "%s:no such file\n"
//unexpected number of arguments exit message
#define ARGC_ERROR "please supply a file! usage: SudokuSolver<filename>\n"
//no solution message.
#define NO_SOLUTION_ERROR "no solution!\n"
//fclose error message.
#define FCLOSE_ERROR "failed to close file\n"

// -------------------------- functions -------------------------

/**
 * @brief scan the size of the sudoku board from the input file.
 * @param inputFile the given input file.
 * @param inputFileName the name of the given input file.
 * @param errorNum for the result: address for an int representing an error.
 * @return the size of the board.
 */
static unsigned int scanSize(FILE* inputFile, char* inputFileName, int *errorNum)
{
    assert(inputFile != NULL && inputFileName != NULL && errorNum != NULL);

    char buffer[BUFFER_SIZE], *pBuffer;
    fgets(buffer, BUFFER_SIZE, inputFile);
    pBuffer = buffer;

    int scannedSize = (int)strtod(pBuffer, &pBuffer);

    if (*pBuffer != END_LINE || scannedSize <= 0 || scannedSize > MAX_BOARD_SIZE ||
        pow(sqrt(scannedSize), SQUARED) != scannedSize)
    {
        fprintf(ERROR_DEST, INVALID_FILE_ERROR, inputFileName);
        *errorNum = INVALID_INPUT_EXIT;
        return 0;
    }

    *errorNum = SUCCESS_EXIT;
    return (unsigned)scannedSize;
}

/**
 * @brief scan a line from the input, representing a row in the sudoku board.
 * @param inputFile the given input file.
 * @param inputFileName the name of the given input file.
 * @param board the given board.
 * @param i the row index.
 * @return an error code in case of an error; SUCCESS_EXIT otherwise.
 */
static int scanLine(FILE* inputFile, char* inputFileName, pBoard board, unsigned int i)
{
    assert(inputFile != NULL && inputFileName != NULL && board != NULL);
    assert(i < getSize(board));

    char buffer[BUFFER_SIZE], *pBuffer;
    fgets(buffer, BUFFER_SIZE, inputFile);
    pBuffer = buffer;
    unsigned int size = getSize(board);

    for (unsigned int j = 0; j < size; j++)
    {
        int val = (int)strtod(pBuffer, &pBuffer);
        if (val < 0 || (unsigned)val > size || (j < size - 1 && *pBuffer != SPACE) ||
            (j == size - 1 && i < size - 1 && *pBuffer != END_LINE))
        {
            fprintf(ERROR_DEST, INVALID_FILE_ERROR, inputFileName);
            return INVALID_INPUT_EXIT;
        }

        setCell(board, i, j, (unsigned int)val);
        if (j < size - 1)
        {
            pBuffer++;
        }
    }
    return SUCCESS_EXIT;
}

/**
 * @brief scan the values of a sudoku board from the given input file.
 * @param inputFile the given input file.
 * @param inputFileName the name of the given input file.
 * @param errorNum for the result: address for an int representing an error.
 * @return the new sudoku board; NULL in case of error.
 */
static pBoard scanBoard(FILE* inputFile, char* inputFileName, int* errorNum)
{
    assert(inputFile != NULL && inputFileName != NULL && errorNum != NULL);

    unsigned int size = scanSize(inputFile, inputFileName, errorNum);
    if (*errorNum != SUCCESS_EXIT)
    {
        return NULL;
    }

    pBoard board = newBoard(size);
    if (board == NULL)
    {
        *errorNum = MALLOC_EXIT;
        return NULL;
    }

    int rc;
    for (unsigned int i = 0; i < size; i++)
    {
        rc = scanLine(inputFile, inputFileName, board, i);
        if (rc != SUCCESS_EXIT)
        {
            *errorNum = rc;
            freeBoard(board);
            return NULL;
        }
    }

    int endFile;
    if ((endFile = fgetc(inputFile)) != END_LINE && endFile != EOF)
    {
        fprintf(ERROR_DEST, INVALID_FILE_ERROR, inputFileName);
        *errorNum = INVALID_INPUT_EXIT;
        freeBoard(board);
        return NULL;
    }

    return board;
}

/**
 * @brief prints the given sudoku board.
 * @param board the given board.
 */
static void printBoard(pBoard board) const
{
    assert(board != NULL);
    unsigned int size = getSize(board);
    printf(PRINT_SIZE_FORMAT, size);
    for (unsigned int i = 0; i < size; i++)
    {
        for (unsigned int j = 0; j < size; j++)
        {
            printf(PRINT_FORMAT, getCellValue(board, i, j), j < size - 1 ? SPACE : END_LINE);
        }
    }
}

/**
 * solves a sudoku puzzle.
 * @param argc number of arguments.
 * @param argv arguments.
 * @return SUCCESS_EXIT if the program ended successfully; an error code otherwise.
 */
int main(int argc, char* argv[])
{
    if (argc != NUM_ARGS)
    {
        fprintf(ERROR_DEST, ARGC_ERROR);
        return ARGC_EXIT;
    }

    FILE* inputFile = fopen(argv[FILE_INDEX], FILE_FORMAT);
    if (inputFile == NULL)
    {
        fprintf(ERROR_DEST, FOPEN_ERROR, argv[FILE_INDEX]);
        return FOPEN_EXIT;
    }

    int errNum;
    pBoard board = scanBoard(inputFile, argv[FILE_INDEX], &errNum);

    if (fclose(inputFile))
    {
        freeBoard(board);
        fprintf(ERROR_DEST, FCLOSE_ERROR);
        return FCLOSE_EXIT;
    }
    if (board == NULL)
    {
        return errNum;
    }

    unsigned int best = (unsigned int) pow(getSize(board), SQUARED);
    pBoard bestBoard = getBest(board, getBoardChildren, getFilledCells, freeBoard, copyBoard, best);

    if (bestBoard == NULL)
    {
        freeBoard(board);
        printf(NO_SOLUTION_ERROR);
        return SUCCESS_EXIT;
    }
    printBoard(bestBoard);

    freeBoard(board);
    freeBoard(bestBoard);
    return SUCCESS_EXIT;
}