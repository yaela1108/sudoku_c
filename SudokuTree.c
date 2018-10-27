// -------------------------- includes -------------------------

#include "SudokuTree.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// -------------------------- structs -------------------------

/**
 * @brief: A struct representing a sudoku board.
 */
struct SudokuBoard
{
    unsigned int size;
    unsigned int** grid;
    unsigned int filledCells;
};

// -------------------------- functions -------------------------

/**
 * @brief allocates and sets to zero the cells of the i'th row in the given board's grid.
 * @param board the given board.
 * @param i the row index.
 * @param size the size of the grid.
 * @return success if the allocation and assignment were successful; error otherwise.
 */
static int assignCells(pBoard board, unsigned int i, unsigned int size)
{
    board->grid[i] = (unsigned int*)malloc(size * sizeof(unsigned int));
    if (board->grid[i] == NULL)
    {
        fprintf(ERROR_DEST, MALLOC_ERROR);
        for (unsigned int j = 0; j < i; j++)
        {
            free(board->grid[j]);
        }
        free(board->grid);
        free(board);
        return ERROR;
    }

    for (unsigned j = 0; j < size; j++)
    {
        board->grid[i][j] = 0;
    }
    return SUCCESS_EXIT;
}

/**
 * @brief Constructs a new sudoku board of the given size and sets all it cells to zero.
 * @param size the given size.
 * @return the new board.
 */
pNode newBoard(unsigned int size)
{
    pBoard board = (pBoard)malloc(sizeof(Board));
    if (board == NULL)
    {
        fprintf(ERROR_DEST, MALLOC_ERROR);
        return NULL;
    }

    board->grid = (unsigned int**)malloc(size * sizeof(unsigned int*));
    if (board->grid == NULL)
    {
        fprintf(ERROR_DEST, MALLOC_ERROR);
        free(board);
        return NULL;
    }

    int rc;
    for (unsigned int i = 0; i < size; i++)
    {
        rc = assignCells(board, i, size);
        if (rc == ERROR)
        {
            return NULL;
        }
    }

    board->size = size;
    board->filledCells = 0;
    return board;
}

/**
 * @brief Sets the [i][j] cell of the board to the given value.
 * @param board the given board.
 * @param i the row index.
 * @param j the column index.
 * @param val the given value.
 */
void setCell(pBoard board, unsigned int i, unsigned int j, unsigned int val)
{
    assert(board != NULL);
    assert(i >= 0 && i < board->size);
    assert(j >= 0 && j < board->size);
    assert(val >= 0 && val <= board->size);
    assert(board->grid[i][j] == 0);

    board->grid[i][j] = val;
    if (val)
    {
        board->filledCells++;
    }
}

/**
 * @brief Checks whether setting the [i][j] cell of the board to the given value would be a legal
 * move.
 * @param board the given board.
 * @param i the row index.
 * @param j the column index.
 * @param val the given value.
 * @return TRUE if the move is legal; FALSE otherwise.
 */
static int checkCell(pBoard board, unsigned int i, unsigned int j, unsigned int val)
{
    assert(board != NULL);
    assert(i >= 0 && i < board->size);
    assert(j >= 0 && j < board->size);
    assert(val >= 0 && val <= board->size);
    assert(board->grid[i][j] == 0);

    for (unsigned int k = 0; k < board->size; k++)
    {
        if (board->grid[i][k] == val)
        {
            return FALSE;
        }
        if (board->grid[k][j] == val)
        {
            return FALSE;
        }
    }

    unsigned int blockSize = (unsigned int)sqrt(board->size);
    i -= i % blockSize;
    j -= j % blockSize;
    for (unsigned int k = i; k < i + blockSize; k++)
    {
        for (unsigned int l = j; l < j + blockSize; l++)
        {
            if (board->grid[k][l] == val)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

/**
 * @brief Creates a deep copy of the given board.
 * @param node the given board.
 * @return the new board.
 */
pNode copyBoard(pNode node) const
{
    pBoard board = (pBoard)node;
    if (board == NULL)
    {
        fprintf(ERROR_DEST, POINTER_ERROR);
        return NULL;
    }
    pBoard copyBoard = newBoard(board->size);
    if (copyBoard == NULL)
    {
        return NULL;
    }

    for (unsigned int i = 0; i < board->size; i++)
    {
        for (unsigned int j = 0; j < board->size; j++)
        {
            setCell(copyBoard, i, j, board->grid[i][j]);
        }
    }
    return copyBoard;
}

/**
 * @brief free the given board from the heap.
 * @param node the given board.
 */
void freeBoard(pNode node)
{
    pBoard board = (pBoard)node;
    if (board != NULL)
    {
        if (board->grid != NULL)
        {
            for (unsigned int i = 0; i < board-> size; i++)
            {
                free(board->grid[i]);
            }
        }
        free(board->grid);
    }
    free(board);
}

/**
 * return the number of filled (not zero) cells on the board.
 * @param node the given board.
 * @return the number of cells.
 */
unsigned int getFilledCells(pNode node)
{
    pBoard board = (pBoard)node;
    if (board == NULL)
    {
        fprintf(ERROR_DEST, POINTER_ERROR);
        return 0;
    }
    return board->filledCells;
}

/**
 * @brief find the next empty cell on the board, going left to right from the top,
 * @param board the given board.
 * @param row address for the empty cell's row index.
 * @param column address for the empty cell's column index.
 * @return TRUE if found an empty cell; FALSE otherwise;
 */
static int nextEmptyBlock(pBoard board, unsigned int* row, unsigned int* column)
{
    assert(board != NULL);
    assert(row != NULL && column != NULL);

    for (unsigned int i = 0; i < board->size; i++)
    {
        for (unsigned int j = 0; j < board-> size; j++)
        {
            if (board->grid[i][j] == 0)
            {
                *row = i;
                *column = j;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**
 * @brief constructs all legal permutations of the given board created by setting a value to
 * the given [i][j] cell, and saves them into the given address of an array.
 * @param board the given board.
 * @param children the address of an array; used for the result.
 * @param i the row index
 * @param j the column index
 * @return the number of permutations created; ERROR if allocation failed.
 */
static int assignBoardChildren(pBoard board, pBoard** children, unsigned int i, unsigned int j)
{
    assert(*children != NULL);
    unsigned int count = 0;

    for (unsigned int val = 1; val <= board->size; val++)
    {
        if(checkCell(board, i, j, val))
        {
            (*children)[count] = copyBoard(board);
            if ((*children)[count] == NULL)
            {
                fprintf(ERROR_DEST, MALLOC_ERROR);
                for (unsigned int k = 0; k < count; k++)
                {
                    freeBoard((*children)[k]);
                }
                free(*children);
                return ERROR;
            }
            setCell((*children)[count], i, j, val);
            count++;
        }
    }
    return count;
}

/**
 * @brief constructs all legal permutations of the given board created by setting a value to
 * the next empty cell, and saves them into the given address of an array.
 * @param node the given board
 * @param nodeChildren the address of an array; used for the result.
 * @return the number of permutations created or ERROR.
 */
int getBoardChildren(pNode node, pNode** nodeChildren /*for the result*/)
{

    pBoard board = (pBoard)node;
    pBoard** children = (pBoard**)nodeChildren;
    if (board == NULL || children == NULL)
    {
        fprintf(ERROR_DEST, POINTER_ERROR);
        return ERROR;
    }

    unsigned int i, j;
    int result = nextEmptyBlock(board, &i, &j);
    if (!result)
    {
        return 0;
    }

    *children = (pBoard*)malloc(board->size * sizeof(pBoard));
    if (*children == NULL)
    {
        fprintf(ERROR_DEST, MALLOC_ERROR);
        return ERROR;
    }

    return assignBoardChildren(board, children, i, j);
}

/**
 * @brief return the board's size.
 * @param board the given board.
 * @return the board's size.
 */
unsigned int getSize(pBoard board)
{
    return board->size;
}

/**
 * @brief return the value of the given cell.
 * @param board the given board.
 * @param i the row index.
 * @param j the column index.
 * @return the value.
 */
unsigned int getCellValue(pBoard board, unsigned int i, unsigned int j)
{
    return board->grid[i][j];
}
