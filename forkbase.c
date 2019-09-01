#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
#include<time.h>
#include<stdbool.h>
#include<signal.h>

int main (int argc, char* argv[]){
    //Number of Games
    //int M = atoi(argv[1]);
    int M = 1;

    //Board dimensions
    //int N = atoi(argv[2]);
    int N = 4;

    //Array of Process IDs
    pid_t pids[M];
    
    //Arrays holding the stdin and stdouts for the file descriptors that come out of using pipe()
    int childPipesFD[M][2];
    int parentPipesFD[M][2];

    //Board declaration
    char gameBoard[N][N];

    //Seeding random for random moves
    srand(time(0));

    //Loop that happens for every game
    for (int i = 0; i < M; i++){

        //Game Board set up
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                gameBoard[j][k] = 'O'; 
            }
        }

        //Pipes for parent and children for this loop
        if(pipe(childPipesFD[i]) == -1){
            printf("Child Pipe error");
            return 1;
        }
        if(pipe(parentPipesFD[i]) == -1){
            printf("Parent Pipe failed");
            return 1;
        }

        if((pids[i] = fork()) < 0 ){
            perror("Fork");
            return 1;
        } else if (pids[i] == 0){
            //Child's Turn
            
            char receiveBuffer[100];
            char buffer[100];

            //Closing unused pipes
            if(close(childPipesFD[i][0]) == -1){
                perror("Close");
                abort();
            }
            if(close(parentPipesFD[i][1]) == -1){
                perror("Close");
                abort();
            }
            
            //Play Game
            
            read(parentPipesFD[i][0], receiveBuffer, sizeof(receiveBuffer));
            printf("Child recieved: %s\n", receiveBuffer);
            
            while(1){
                
                write(childPipesFD[i][1], buffer, sizeof(buffer));

            }


            //Clean up
            if(close(childPipesFD[i][1]) == -1){
                perror("Close");
                abort();
            }
            if(close(parentPipesFD[i][0]) == -1){
                perror("Close");
                abort();
            }

            exit(0);
        } else{
            //Parent's Turn

            //Move information
            char buffer[100];
            char receiveBuffer[100];

            //Closing unused pipes
            if(close(childPipesFD[i][1]) == -1){
                perror("Close");
                abort();
            }
            if(close(parentPipesFD[i][0]) == -1){
                perror("Close");
                abort();
            }
            
            //Play Game
            //Parent Makes initial Move

            //Parent updates board

            //Parent sends information to Child
            write(parentPipesFD[i][1], buffer, sizeof(buffer));


            //Game Loop begins
            //Continues until a winner is detected
            while(1){

                //Parent waits and reads from child pipe
                read(childPipesFD[i][0], receiveBuffer, sizeof(receiveBuffer));
                printf("Got back: %s\n", receiveBuffer);
                
                //Parent updates the board

                //Parent checks for a winner

                //Parent makes next move

                //Parent checks for a winner

            }

            //Clean Up
            wait(NULL); //Wait for child process to exit

            if(close(childPipesFD[i][0]) == -1){
                perror("Close");
                abort();
            }
            if(close(parentPipesFD[i][1]) == -1){
                perror("Close");
                abort();
            }

        }
    }
    return 0;
}