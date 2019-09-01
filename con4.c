#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <string.h> 
#include <sys/wait.h> 
#include <signal.h>

const char PARENT_PIECE = 'X';
const char CHILD_PIECE = 'O';
const char EMPTY_PIECE = ' ';

const int PARENT_INT = 1;
const int CHILD_INT = 0;
const int EMPTY_INT = -1;

const int BOARD_FULL = 2;
const int PARENT_WON = 1;
const int CHILD_WON = 0;

struct GameData
{
    int** board;
    int dimensions;
    int column;
    int row;
    int gameNumber;
    bool parentTurn;
};

struct ForkData
{
    pid_t pid;
    int* parentFD;
    int* childFD;
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
                printf("%c", CHILD_PIECE);
            }
            else
            {
                printf("%c", PARENT_PIECE);
            }

            printf("|");
        }

        printf("\n");
        j = 0;
    }

    printf("------------------------------------\n");
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

    int piece = gameData->parentTurn ? PARENT_INT : CHILD_INT;
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
        return gameData->parentTurn ? PARENT_WON : CHILD_WON;
    }

    if(checkBoardFull(gameData))
    {
        return BOARD_FULL;
    }

    return -1;
}

// Checks to see if a piece is being placed in a valid column,
// if so, places piece
int placePiece(int** board, int dimensions, int column, bool parentTurn)
{
    // printf("PlacePiece\n");

    int row = checkColumnFull(board, dimensions, column);
    
    if(row < 0)
    {
        //printf("Trying to place piece in full column!\n");
        return -1;
    }

    if(parentTurn)
    {
        board[column][row] = PARENT_INT;
    }
    else
    {
        board[column][row] = CHILD_INT;
    }

    return row;
}

// Places pieces until the game is over
void gameLoop(struct GameData* gameData, struct ForkData* forkData)
{
    // printf("GameLoop\n");
    
    int gameState = -1;
    int readFD;
    int writeFD;

    int* recievedMove = (int*)malloc(sizeof(int));
    int* move = (int*)malloc(sizeof(int));

    if(forkData->pid == 0)
    {   
        readFD = forkData->parentFD[0];
        writeFD = forkData->childFD[1];
    }
    else
    {   
        readFD = forkData->childFD[0];
        writeFD = forkData->parentFD[1];
    }

    while(gameState < 0)
    {
        if((gameData->parentTurn && (forkData->pid == 0)) || (!gameData->parentTurn && (forkData->pid == 1)))
        {
            read(readFD, recievedMove, sizeof(recievedMove));

            gameData->column = *recievedMove;
            gameData->row = placePiece(gameData->board, gameData->dimensions, gameData->column, gameData->parentTurn);

            if(gameData->row >= 0)
            {
                gameState = checkGameOver(gameData);
            }

            gameData->parentTurn = !gameData->parentTurn;
        }
        else
        {
            gameData->column = rand() % gameData->dimensions;
            gameData->row = placePiece(gameData->board, gameData->dimensions, gameData->column, gameData->parentTurn);

            if(gameData->row >= 0)
            {
                write(writeFD, move, sizeof(move));
                gameState = checkGameOver(gameData);

                gameData->parentTurn = !gameData->parentTurn;
            }
        }
    }

    close(forkData->parentFD[0]);
    close(forkData->parentFD[1]);
    close(forkData->childFD[0]);
    close(forkData->childFD[1]);

    if(forkData->pid == 0)
    {
        exit(0);
    }

    if(gameState == PARENT_WON)
    {
        printf("Game %i: The PARENT won!\n", gameData->gameNumber);
    }
    else if(gameState == CHILD_WON)
    {
        printf("Game %i: The CHILD won!\n", gameData->gameNumber);
    }
    else if(gameState == BOARD_FULL)
    {
        printf("Game %i: Ended in a DRAW!\n", gameData->gameNumber);
    }
}

// Sets Up GameData and starts the game loop
void initializeGame(int dimensions, int gameNumber, int* parentFD, int* childFD)
{
    // printf("InitializeGame\n");

    struct GameData* gameData = (struct GameData*)malloc(sizeof(struct GameData));
    struct ForkData* forkData = (struct ForkData*)malloc(sizeof(struct ForkData));

    int i = 0;
    int j = 0;

    gameData->board = (int**)malloc(dimensions * sizeof(int*));
    gameData->dimensions = dimensions;
    gameData->column = 0;
    gameData->row = 0;
    gameData->gameNumber = gameNumber;
    gameData->parentTurn = true;
    
    for(i; i < gameData->dimensions; i++)
    {
        gameData->board[i] = (int*)malloc(gameData->dimensions * sizeof(int));

        for(j; j < gameData->dimensions; j++)
        {
            gameData->board[i][j] = EMPTY_INT;
        }

        j = 0;
    }

    pipe(parentFD);
    pipe(childFD);
    
    forkData->parentFD = parentFD;
    forkData->childFD = childFD;
    forkData->pid = fork();

    gameLoop(gameData, forkData);
    printBoard(gameData);
}

int** generateFDArray(int games)
{
    // printf("GenerateFDArray\n");

    int i = 0;
    int** array;

    array = (int**)malloc(games * sizeof(int*));

    for(i; i < games; i++)
    {
        array[i] = (int*)malloc(2 * sizeof(int));
    }

    return array;
}

int main(int argc, const char* argv[])
{
    int games;
    int dimensions;

    int i = 0;

    pid_t* pids;
    int** parentFDs;
    int** childFDs;

    if(argc != 3)
    {
        printf("Improper number of arguments\n");
        return -1;
    }

    games = atoi(argv[1]);
    dimensions = atoi(argv[2]);

    pids = (pid_t*)malloc(games * sizeof(pid_t));
    
    parentFDs = generateFDArray(games);
    childFDs = generateFDArray(games);

    printf("------------------------------------\n");

    for(i; i < games; i++)
    {
        srand(i * time(0));
        initializeGame(dimensions, i + 1, parentFDs[i], childFDs[i]);
    }

    return 0;
}
