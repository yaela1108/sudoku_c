#ifndef EX3_SUDOKUTREE_H
#define EX3_SUDOKUTREE_H

// -------------------------- macros -------------------------

//-----boolean values
//true.
#define TRUE 1
//false.
#define FALSE 0
//general error exit code.
#define ERROR -1
//success exit code.
#define SUCCESS_EXIT 0

//malloc error message.
#define MALLOC_ERROR "error: out of memory\n"
//pointer error message.
#define POINTER_ERROR "warning: received NULL pointer\n"
//error message destination
#define ERROR_DEST stderr

// -------------------------- structs & typedef -------------------------

/**
 * @brief: A struct representing a sudoku board.
 */
typedef struct SudokuBoard Board;

/**
 * @brief A pointer to a sudoku board.
 */
typedef Board* pBoard;

/**
 * @brief a void pointer
 */
typedef void* pNode;

// -------------------------- functions -------------------------

/**
 * @brief Constructs a new sudoku board of the given size and sets all it cells to zero.
 * @param size the given size.
 * @return the new board.
 */
pNode newBoard(unsigned int size);

/**
 * @brief Sets the [i][j] cell of the board to the given value.
 * @param board the given board.
 * @param i the row index.
 * @param j the column index.
 * @param val the given value.
 */
void setCell(pBoard board, unsigned int i, unsigned int j, unsigned int val);

/**
 * @brief return the value of the given cell.
 * @param board the given board.
 * @param i the row index.
 * @param j the column index.
 * @return the value.
 */
unsigned int getSize(pBoard board);

/**
 * @brief return the board's size.
 * @param board the given board.
 * @return the board's size.
 */
unsigned int getCellValue(pBoard board, unsigned int i, unsigned int j);

/**
 * @brief constructs all legal permutations of the given board created by setting a value to
 * the next empty cell, and saves them into the given address of an array.
 * @param node the given board
 * @param nodeChildren the address of an array; used for the result.
 * @return the number of permutations created or ERROR.
 */
int getBoardChildren(pNode node, pNode** nodeChildren /*for the result*/);

/**
 * return the number of filled (not zero) cells on the board.
 * @param node the given board.
 * @return the number of cells.
 */
unsigned int getFilledCells(pNode node);

/**
 * @brief free the given board from the heap.
 * @param node the given board.
 */
void freeBoard(pNode node);

/**
 * @brief Creates a deep copy of the given board.
 * @param node the given board.
 * @return the new board.
 */
pNode copyBoard(pNode node);

#endif //EX3_SUDOKUTREE_H
