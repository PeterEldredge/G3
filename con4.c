#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

const char HOST_PIECE = 'X';
const char CLIENT_PIECE = 'O';
const char EMPTY_PIECE = ' ';

const int HOST_INT = 1;
const int CLIENT_INT = 0;
const int EMPTY_INT = -1;

const int BOARD_FULL = 2;
const int HOST_WON = 1;
const int CLIENT_WON = 0;

struct GameData
{
    int** board;
    int dimensions;
    int column;
    int row;
    bool hostTurn;
};

struct Point 
{
    int x;
    int y;
};

// Creates a new point structure
struct Point newPoint(int x, int y)
{
    //printf("NewPoint\n");

    struct Point point = {x, y};

    return point; 
}

// Adds two points together
struct Point addPoints(struct Point a, struct Point b)
{
    // printf("AddPoints\n");

    return newPoint(a.x + b.x, a.y + b.y);
}

// Reverses signs of the given point, used for swapping the direction to check on
// a given line
struct Point reversePoints(struct Point p)
{
    // printf("ReversePoints\n");

    return newPoint(p.x * -1, p.y * -1);
}

// Returns the value of the given point in the board
int getPoint(int** board, struct Point p)
{
    // printf("GetPoint\n");

    return board[p.x][p.y];
}

// Checks to see if the given point is in the board given the dimensions
bool pointInBoard(struct Point p, int dimensions)
{
    // printf("PointInBoard\n");

    if(p.x < 0 || p.y < 0)
    {
        return false;
    }

    if(p.x > (dimensions - 1) || p.y > (dimensions - 1))
    {
        return false;
    }

    return true;
}

// Prints board to console
void printBoard(struct GameData* gameData)
{
    // printf("PrintBoard\n");

    int i = gameData->dimensions - 1;
    int j = 0;
    
    for(i; i >= 0; i--)
    {
        for(j; j < gameData->dimensions; j++)
        {
            if(j == 0)
            {
                printf("|");
            }

            if(gameData->board[j][i] == -1)
            {
                printf("%c", EMPTY_PIECE);
            }
            else if(gameData->board[j][i] == 0)
            {
                printf("%c", CLIENT_PIECE);
            }
            else
            {
                printf("%c", HOST_PIECE);
            }

            printf("|");
        }

        printf("\n");
        j = 0;
    }
}

// Returns the open spot in the designated column, if the column is full returns -1
int checkColumnFull(int** board, int dimensions, int column)
{
    // printf("CheckColumnFull\n");

    int i = 0;

    for(i; i < dimensions; i++)
    {
        if(board[column][i] == EMPTY_INT)
        {
            return i;
        }
    }

    return -1;
}

// Returns if the board is full or not
bool checkBoardFull(struct GameData* gameData)
{
    // printf("CheckBoardFull\n");

    int i = 0;

    for(i; i < gameData->dimensions; i++)
    {
        if(checkColumnFull(gameData->board, gameData->dimensions, i) != -1)
        {
            return false;
        }
    }

    return true;
}

// Checks the number of pieces in a row of the same type,
// given a starting point and a directions
int checkLine(struct GameData* gameData, struct Point direction)
{
    // printf("CheckLine\n");

    struct Point startingPoint = newPoint(gameData->column, gameData->row);
    struct Point currentPoint = startingPoint;

    int piece = gameData->hostTurn ? HOST_INT : CLIENT_INT;
    int counter = -1; // Set to -1 because starting point gets counted twice

    while((getPoint(gameData->board, currentPoint) == piece))
    {
        counter++;
        currentPoint = addPoints(currentPoint, direction);

        if(!pointInBoard(currentPoint, gameData->dimensions))
        {
            break;
        }
    }

    direction = reversePoints(direction);
    currentPoint = startingPoint;

    while((getPoint(gameData->board, currentPoint) == piece))
    {
        counter++;
        currentPoint = addPoints(currentPoint, direction);

        if(!pointInBoard(currentPoint, gameData->dimensions))
        {
            break;
        }
    }

    return counter;
}

// Sees if the game has been won by the current player
bool checkGameWon(struct GameData* gameData)
{
    // printf("CheckBoardFull\n");

    if(checkLine(gameData, newPoint(-1, 1)) >= 4) // Diagonal "\"
    {
        return true;
    }
    else if(checkLine(gameData, newPoint(0, 1)) >= 4) // Vertical "|"
    {
        return true;
    }
    else if(checkLine(gameData, newPoint(1, 1)) >= 4) // Diagonal "/"
    {
        return true;
    }
    else if(checkLine(gameData, newPoint(1, 0)) >= 4) // Horrizontal "--"
    {
        return true;
    }

    return false;
}

// Calls functions that check game over condition
int checkGameOver(struct GameData* gameData)
{
    // printf("CheckGameOver\n");

    if(checkGameWon(gameData))
    {
        return gameData->hostTurn ? HOST_WON : CLIENT_WON;
    }

    if(checkBoardFull(gameData))
    {
        return BOARD_FULL;
    }

    return -1;
}

// Checks to see if a piece is being placed in a valid column,
// if so, places piece
int placePiece(int** board, int dimensions, int column, bool hostTurn)
{
    // printf("PlacePiece\n");

    int row = checkColumnFull(board, dimensions, column);
    
    if(row < 0)
    {
        //printf("Trying to place piece in full column!\n");
        return -1;
    }

    if(hostTurn)
    {
        board[column][row] = HOST_INT;
    }
    else
    {
        board[column][row] = CLIENT_INT;
    }

    return row;
}

// Places pieces until the game is over
void gameLoop(struct GameData* gameData)
{
    // printf("GameLoop\n");

    int gameState = -1;
    
    srand(time(0));

    while(gameState < 0)
    {
        gameData->column = rand() % gameData->dimensions;
        gameData->row = placePiece(gameData->board, gameData->dimensions, gameData->column, gameData->hostTurn);

        if(gameData->row >= 0)
        {
            gameState = checkGameOver(gameData);
            gameData->hostTurn = !gameData->hostTurn;
        }
    }

    if(gameState == HOST_WON)
    {
        printf("The HOST won!\n");
    }
    else if(gameState == CLIENT_WON)
    {
        printf("The CLIENT won!\n");
    }
    else if(gameState == BOARD_FULL)
    {
        printf("The game ended in a DRAW!\n");
    }
}

// Sets Up GameData and starts the game loop
void initializeGame(int dimensions)
{
    // printf("InitializeGame\n");

    struct GameData* gameData = (struct GameData*)malloc(sizeof(struct GameData));

    int i = 0;
    int j = 0;

    gameData->board = (int**)malloc(dimensions * sizeof(int*));
    gameData->dimensions = dimensions;
    gameData->column = 0;
    gameData->row = 0;
    gameData->hostTurn = true;
    
    for(i; i < gameData->dimensions; i++)
    {
        gameData->board[i] = (int*)malloc(gameData->dimensions * sizeof(int));

        for(j; j < gameData->dimensions; j++)
        {
            gameData->board[i][j] = EMPTY_INT;
        }

        j = 0;
    }

    gameLoop(gameData);
    printBoard(gameData);
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
