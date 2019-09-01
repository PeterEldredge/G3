#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void printBoard(int** board, int dimensions)
{
    int i = dimensions - 1;
    int j = 0;
    
    for(i; i >= 0; i--)
    {
        for(j; j < dimensions; j++)
        {
            if(j == 0)
            {
                printf("|");
            }

            if(board[j][i] == -1)
            {
                printf(" |");
            }
            else if(board[j][i] == 0)
            {
                printf("O|");
            }
            else
            {
                printf("X|");
            }
        }

        printf("\n");
        j = 0;
    }
}

//Returns the open spot in the designated column, if the column is full returns -1
int checkColumnFull(int** board, int dimensions, int column)
{
    int i = 0;

    for(i; i < dimensions; i++)
    {
        if(board[column][i] == -1)
        {
            return i;
        }
    }

    return -1;
}

//Returns if the board is full or not
bool checkBoardFull(int** board, int dimensions)
{
    int i = 0;

    for(i; i < dimensions; i++)
    {
        if(checkColumnFull(board, dimensions, i) != -1)
        {
            return false;
        }
    }

    return true;
}

bool checkGameOver(int** board, int dimensions, int column, int row)
{
    if(checkBoardFull(board, dimensions))
    {
        return true;
    }
}

int placePiece(int** board, int dimensions, int column, bool hostTurn)
{
    int placementLocation = checkColumnFull(board, dimensions, column);

    if(placementLocation < 0)
    {
        printf("Trying to place piece in full column!\n");
        return -1;
    }

    if(hostTurn)
    {
        board[column][placementLocation] = 1;
    }
    else
    {
        board[column][placementLocation] = 0;
    }

    return placementLocation;
}

void gameLoop(int** board, int dimensions)
{
    bool gameOver = false;
    bool hostTurn = true;

    int column;
    int placementLocation;

    srand(time(0));

    while(!gameOver)
    {
        column = rand() % dimensions;
        placementLocation = placePiece(board, dimensions, column, hostTurn);

        if(placementLocation >= 0)
        {
            gameOver = checkGameOver(board, dimensions, column, placementLocation);
            hostTurn = !hostTurn;
        }
    }
}

void initializeGame(int dimensions)
{
    int i = 0;
    int j = 0;

    int** board = (int**)malloc(dimensions * sizeof(int*));
    
    for(i; i < dimensions; i++)
    {
        board[i] = (int*)malloc(dimensions * sizeof(int));

        for(j; j < dimensions; j++)
        {
            board[i][j] = -1;
        }

        j = 0;
    }

    gameLoop(board, dimensions);

    printBoard(board, dimensions);
}

int main(int argc, const char* argv[])
{
    int games;
    int dimensions;

    if(argc != 3)
    {
        printf("Improper number of arguments\n");
        return -1;
    }

    games = atoi(argv[1]);
    dimensions = atoi(argv[2]);

    initializeGame(dimensions);

    return 0;
}
