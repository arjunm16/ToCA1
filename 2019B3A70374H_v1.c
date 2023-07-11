#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define LINE_LEN 1000

bool checkWithinGrid(int x, int y, int n) {
    if(x >= 0 && y >= 0 && x <= n && y <= n)
        return true;
    
    return false;
}

bool checkIfOver(int x, int y, int n) {
    if(x == n && y == n)
        return true;
    
    return false;
}

int main()
{
    FILE *fptr;
    int n;
    char w[LINE_LEN];
    int curpos_x = 0, curpos_y = 0;
    
    fptr = fopen("input.txt", "r");
    
    if ((fptr = fopen("input.txt", "r")) == NULL) {
        printf("Error in opening file, please retry execution\nExiting...");
        exit(1);
    }
    
    fgets(w, LINE_LEN, fptr);
    
    n = atoi(w);
    
    fgets(w, LINE_LEN, fptr);
    
    fclose(fptr);
    
    key_t key = ftok("toca1", 10);
    int shmid;
    
    if((shmid = shmget(key, 1024, 0666 | IPC_CREAT)) == -1) {
        printf("\nshmget() error\n");
        exit(1);
    }
    
    char *shmPtr = shmat(shmid, (void*)0, 0);
    
    if(shmPtr == (char *) -1) {
        printf("\nshmPtr error\n");
        exit(2);
    }
    
    shmPtr[0] = '0';
    
    pid_t main_pid = getpid();
    
    for(int i = 0; w[i] != '\0'; i++) {
        
        //check if shared mem location has been updated with bool found, if yes, then exit program
        if(shmPtr[0] == '1')
            break;
        
        if(w[i] == '0') {
            int alt1 = curpos_x - 1;
            int alt2 = curpos_x + 1;
            
            if(checkWithinGrid(alt1, curpos_y, n) && checkWithinGrid(alt2, curpos_y, n)) {
                //fork processes for each
                
                if(fork() == 0) {
                    //child process
                    curpos_x = alt1;
                    if(w[i + 1] == '\0' && checkIfOver(curpos_x, curpos_y, n)) {
                        //update shared mem location with found, but only when entire string has been consumed
                        shmPtr[0] = '1';
                        break;
                    }
                }
                
                else{
                    //parent process
                    curpos_x = alt2;
                    if(w[i + 1] == '\0' && checkIfOver(curpos_x, curpos_y, n)) {
                        //update shared mem location with found, but only when entire string has been consumed
                        shmPtr[0] = '1';
                        break;
                    }
                }
            }
            
            else if(checkWithinGrid(alt1, curpos_y, n)) {
                //do work in same process
                curpos_x = alt1;
                if(w[i + 1] == '\0' && checkIfOver(curpos_x, curpos_y, n)) {
                    //update shared mem location with found, but only when entire string has been consumed
                    shmPtr[0] = '1';
                    break;
                }
            }
            
            else if(checkWithinGrid(alt2, curpos_y, n)) {
                //do work in same process
                curpos_x = alt2;
                if(w[i + 1] == '\0' && checkIfOver(curpos_x, curpos_y, n)) {
                    //update shared mem location with found, but only when entire string has been consumed
                    shmPtr[0] = '1';
                    break;
                }
            }
        }
        
        else {
            int alt1 = curpos_y - 1;
            int alt2 = curpos_y + 1;
            
            if(checkWithinGrid(curpos_x, alt1, n) && checkWithinGrid(curpos_x, alt2, n)) {
                //fork processes for each
                
                if(fork() == 0) {
                    //child process
                    curpos_y = alt1;
                    if(w[i + 1] == '\0' && checkIfOver(curpos_x, curpos_y, n)) {
                        //update shared mem location with found, but only when entire string has been consumed
                        shmPtr[0] = '1';
                        break;
                    }
                }
                
                else{
                    //parent process
                    curpos_y = alt2;
                    if(w[i + 1] == '\0' && checkIfOver(curpos_x, curpos_y, n)) {
                        //update shared mem location with found, but only when entire string has been consumed
                        shmPtr[0] = '1';
                        break;
                    }
                }
            }
            
            else if(checkWithinGrid(curpos_x, alt1, n)) {
                //do work in same process
                curpos_y = alt1;
                if(w[i + 1] == '\0' && checkIfOver(curpos_x, curpos_y, n)) {
                    //update shared mem location with found, but only when entire string has been consumed
                    shmPtr[0] = '1';
                    break;
                }
            }
            
            else if(checkWithinGrid(curpos_x, alt2, n)) {
                //do work in same process
                curpos_y = alt2;
                if(w[i + 1] == '\0' && checkIfOver(curpos_x, curpos_y, n)) {
                    //update shared mem location with found, but only when entire string has been consumed
                    shmPtr[0] = '1';
                    break;
                }
            }
        }
    }
    
    while(wait(NULL) > 0);
    
    if(getpid() == main_pid) {
        if(shmPtr[0] == '1')
            printf("\nString Accepted\n");
        else
            printf("\nString Not Accepted\n");
    }

    return 0;
}