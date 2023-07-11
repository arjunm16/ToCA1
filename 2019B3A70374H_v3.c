#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <math.h>

#define LINE_LEN 1000
#define PATH_LEN 100000

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

void print_log(int path_arr[], int acc) {
    
    FILE *fp;
    fp = fopen("output.txt", "a");
    
    
    
    if(acc == 0) {   //failed path
        
        fprintf(fp, "Failed at path: ");
        
        for(int i = 0; i < PATH_LEN && path_arr[i] != -1; i++)
            fprintf(fp, "%d ", path_arr[i]);
        
        fprintf(fp, "\n");
    }
    else if(acc == 1) {  //accepted path
        
        fprintf(fp, "Accepted! Followed path: ");
        
        for(int i = 0; i < PATH_LEN && path_arr[i] != -1; i++)
            fprintf(fp, "%d ", path_arr[i]);
        
        fprintf(fp, "\n");
    }
    else if(acc == 2) {  //exploring path
        
        fprintf(fp, "[%d:%d] Exploring path: ", getppid(), getpid());
        
        for(int i = 0; i < PATH_LEN && path_arr[i] != -1; i++)
            fprintf(fp, "%d ", path_arr[i]);
        
        fprintf(fp, "\n");
    }
    
    fclose(fp);
}

int state_label(int x, int y, int n) {
    return (n + 1) * y + x;
}

int search(int arr[], int length, int search_val) {
    for(int i = 0; i < length; i++)
        if(arr[i] == search_val)
            return 1;
    
    return 0;
}

int main()
{
    FILE *fptr;
    int n;
    char w[LINE_LEN];
    int curpos_x = 0, curpos_y = 0;
    int visited[PATH_LEN], vis_update = 0;
    int reached_end = 0;
    
    memset(visited, -1, PATH_LEN * sizeof(visited[0]));
    
    remove("output.txt");
    
    fptr = fopen("input.txt", "r");
    
    if ((fptr = fopen("input.txt", "r")) == NULL) {
        printf("Error in opening file, please retry execution\nExiting...");
        exit(1);
    }
    
    fgets(w, LINE_LEN, fptr);
    
    n = atoi(w);
    //n given

    fgets(w, LINE_LEN, fptr);
    //w is the string i/p

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
    
    visited[vis_update++] = (n + 1) * curpos_y + curpos_x;
    print_log(visited, 2);
    
    if(w[0] == '\0') {
                    
        if(checkIfOver(curpos_x, curpos_y, n)) {
            //update shared mem location with found, but only when entire string has been consumed
            shmPtr[0] = '1';
            reached_end = 1;
        }
        print_log(visited, reached_end);
    }
    
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
                    visited[vis_update++] = (n + 1) * curpos_y + curpos_x;
                    print_log(visited, 2);
                    
                    if(w[i + 1] == '\0') {
                    
                        if(checkIfOver(curpos_x, curpos_y, n)) {
                            //update shared mem location with found, but only when entire string has been consumed
                            shmPtr[0] = '1';
                            reached_end = 1;
                        }
                        print_log(visited, reached_end);
                    }
                }
                
                else{
                    //parent process
                    curpos_x = alt2;
                    visited[vis_update++] = (n + 1) * curpos_y + curpos_x;
                    print_log(visited, 2);
                    
                    if(w[i + 1] == '\0') {
                    
                        if(checkIfOver(curpos_x, curpos_y, n)) {
                            //update shared mem location with found, but only when entire string has been consumed
                            shmPtr[0] = '1';
                            reached_end = 1;
                        }
                        print_log(visited, reached_end);
                    }
                }
            }
            
            else if(checkWithinGrid(alt1, curpos_y, n)) {
                //do work in same process
                curpos_x = alt1;
                visited[vis_update++] = (n + 1) * curpos_y + curpos_x;
                print_log(visited, 2);
                
                if(w[i + 1] == '\0') {
                    
                    if(checkIfOver(curpos_x, curpos_y, n)) {
                        //update shared mem location with found, but only when entire string has been consumed
                        shmPtr[0] = '1';
                        reached_end = 1;
                    }
                    print_log(visited, reached_end);
                }
            }
            
            else if(checkWithinGrid(alt2, curpos_y, n)) {
                //do work in same process
                curpos_x = alt2;
                visited[vis_update++] = (n + 1) * curpos_y + curpos_x;
                print_log(visited, 2);
                
                if(w[i + 1] == '\0') {
                    
                    if(checkIfOver(curpos_x, curpos_y, n)) {
                        //update shared mem location with found, but only when entire string has been consumed
                        shmPtr[0] = '1';
                        reached_end = 1;
                    }
                    print_log(visited, reached_end);
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
                    visited[vis_update++] = (n + 1) * curpos_y + curpos_x;
                    print_log(visited, 2);
                    
                    if(w[i + 1] == '\0') {
                    
                        if(checkIfOver(curpos_x, curpos_y, n)) {
                            //update shared mem location with found, but only when entire string has been consumed
                            shmPtr[0] = '1';
                            reached_end = 1;
                        }
                        print_log(visited, reached_end);
                    }
                }
                
                else{
                    //parent process
                    curpos_y = alt2;
                    visited[vis_update++] = (n + 1) * curpos_y + curpos_x;
                    print_log(visited, 2);
                    
                    if(w[i + 1] == '\0') {
                    
                        if(checkIfOver(curpos_x, curpos_y, n)) {
                            //update shared mem location with found, but only when entire string has been consumed
                            shmPtr[0] = '1';
                            reached_end = 1;
                        }
                        print_log(visited, reached_end);
                    }
                }
            }
            
            else if(checkWithinGrid(curpos_x, alt1, n)) {
                //do work in same process
                curpos_y = alt1;
                visited[vis_update++] = (n + 1) * curpos_y + curpos_x;
                print_log(visited, 2);
                
                if(w[i + 1] == '\0') {
                    
                    if(checkIfOver(curpos_x, curpos_y, n)) {
                        //update shared mem location with found, but only when entire string has been consumed
                        shmPtr[0] = '1';
                        reached_end = 1;
                    }
                    print_log(visited, reached_end);
                }
            }
            
            else if(checkWithinGrid(curpos_x, alt2, n)) {
                //do work in same process
                curpos_y = alt2;
                visited[vis_update++] = (n + 1) * curpos_y + curpos_x;
                print_log(visited, 2);
                
                if(w[i + 1] == '\0') {
                    
                    if(checkIfOver(curpos_x, curpos_y, n)) {
                        //update shared mem location with found, but only when entire string has been consumed
                        shmPtr[0] = '1';
                        reached_end = 1;
                    }
                    print_log(visited, reached_end);
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
    
        //code for converting nfa to dfa

        int n_states = (n + 1) * (n + 1);
        int nfa_tran[10000][2];

        memset(nfa_tran, -1, 10000 * sizeof(nfa_tran[0]));

        // int states[10000];              //PSED
        // int dfa_transn[10000][2];//go   //PSED
        // int new_states[10000];//arr     //PSED

        //assign state nos in prog from 1, 2^0 = 1 (q0)

        // memset(states, -1, 10000 * sizeof(states[0]));

        // for(int i = 0; i < n_states; i++)
        //     states[(int) pow(2, i)] = 1;
        
        int nfa_fstate = n_states - 1;
        int nfa_istate = 0;

        //nfa transition array
        //all dec
        //PSED
        // for(int i = 0; i <= n; i++) {
        //     for(int j = 0; j <= n; j++) {
        //         //i,j is posn in grid
                
        //         //L
        //         if(checkWithinGrid(i - 1, j, n))
        //             nfa_tran[state_label(i, j, n)][0][state_label(i - 1, j, n)] = 1;
                
        //         //R
        //         if(checkWithinGrid(i + 1, j, n))
        //             nfa_tran[state_label(i, j, n)][0][state_label(i + 1, j, n)] = 1;

        //         //U
        //         if(checkWithinGrid(i, j + 1, n))
        //             nfa_tran[state_label(i, j, n)][1][state_label(i, j + 1, n)] = 1;
                
        //         //D
        //         if(checkWithinGrid(i, j - 1, n))
        //             nfa_tran[state_label(i, j, n)][1][state_label(i, j - 1, n)] = 1;
        //     }
        // }

        //SSED
        //nfa transition table
        //in each transn, the initial state will be in dec, but final in binary bitmask
        for(int i = 0; i <= n; i++) {
            for(int j = 0; j <= n; j++) {
                //i,j is posn in grid
                
                int comb_state = 0;
                //L
                if(checkWithinGrid(i - 1, j, n))
                    // nfa_tran[state_label(i, j, n)][0] = *(nfa_tran[state_label(i, j, n)][0]) | (int) pow(2, state_label(i - 1, j, n));
                    comb_state = comb_state | (int) pow(2, state_label(i - 1, j, n));

                //R
                if(checkWithinGrid(i + 1, j, n))
                    // nfa_tran[state_label(i, j, n)][0] = *(nfa_tran[state_label(i, j, n)][0]) | (int) pow(2, state_label(i + 1, j, n));
                    comb_state = comb_state | (int) pow(2, state_label(i + 1, j, n));

                nfa_tran[state_label(i, j, n)][0] = comb_state;

                comb_state = 0;
                //U
                if(checkWithinGrid(i, j + 1, n))
                    // nfa_tran[state_label(i, j, n)][1] = *(nfa_tran[state_label(i, j, n)][1]) | (int) pow(2, state_label(i, j + 1, n));
                    comb_state = comb_state | (int) pow(2, state_label(i, j + 1, n));
                
                //D
                if(checkWithinGrid(i, j - 1, n))
                    // nfa_tran[state_label(i, j, n)][1] = *(nfa_tran[state_label(i, j, n)][1]) | (int) pow(2, state_label(i, j - 1, n));
                    comb_state = comb_state | (int) pow(2, state_label(i, j - 1, n));
                
                nfa_tran[state_label(i, j, n)][1] = comb_state;
            }
        }

        //SSED
        int dfa_states[100000] = {-1};
        int dfa_fstates[100000] = {-1};
        int dfa_tran[100000][2] = {-1};

        memset(dfa_states, -1, 100000 * sizeof(dfa_states[0]));
        memset(dfa_fstates, -1, 100000 * sizeof(dfa_fstates[0]));
        memset(dfa_tran, -1, 100000 * sizeof(dfa_tran[0]));
        
        // dfa_states[(int) pow(2, state_label(0, 0, n))] = 1;
        //start state is state 0, q0, represented by 2^0 = 1_dec = 00000000000001_bin
        dfa_states[0] = (int) pow(2, state_label(0, 0, n));

        // dfa_tran[(int) pow(2, state_label(0, 0, n))][0] = (int) pow(2, state_label(1, 0, n));    //not needed to initialize, loop will do it
        // dfa_tran[(int) pow(2, state_label(0, 0, n))][1] = (int) pow(2, state_label(0, 1, n));    //not needed to initialize, loop will do it
        
        //the below loop builds all the dfa states and the dfa transition table
        
        int ctr = 1;
        //as there's already 1 state in dfa_states

        for(int i = 0; i < ctr; i++) {
            for(int j = 0; j < 2; j++) {
                //j is symbol 0,1
                int comb_state = 0;
                //00000000000 which is comb_state = 0 is state phi, not state 0 (state 0 == q0 == 2^0 == 1 == 00000000000001)
                
                for(int k = 0; k < n_states; k++) {
                    //k is state number (from state_label)
                    //m is bin representation of state k
                    int m = (int) pow(2, k);

                    if(m & dfa_states[i])       //check if state k is part of this dfa state
                        comb_state = comb_state | nfa_tran[k][j];
                }

                if(search(dfa_states, 10000, comb_state) == 0)
                    dfa_states[ctr++] = comb_state;
                
                dfa_tran[dfa_states[i]][j] = comb_state;
            }
        }

        //the below loop will list all the final states in the dfa in dfa_fstates
        //basically we need to check which dfa_states contain the final nfa state, which has the label (n+1)*(n+1) - 1 == n_states - 1
        //final nfa state label stored in nfa_fstate
        int f_state_ctr = 0;
        for(int i = 0; i < ctr; i++) {
            int m = (int) pow(2, nfa_fstate);
            //m contains bin rep of final state of nfa

            if(m & dfa_states[i])   //check if nfa final state is contained in this dfa state
                dfa_fstates[f_state_ctr++] = dfa_states[i];
        }
        
        //print dfa states
        for(int i = 0; i < ctr; i++) {
            // printf("q");
            for(int k = 0; k < n_states; k++) {
                int m = (int) pow(2, k);
                if(m & dfa_states[i])
                    printf("q%d", k);
            }
            printf("\n");
        }
        
        //print dfa transitions
        for(int i = 0; i < ctr; i++) {
            // printf("q");
            for(int k = 0; k < n_states; k++) {
                int m = (int) pow(2, k);
                if(m & dfa_states[i])
                    printf("q%d", k);
            }

            printf("||");
            for(int j = 0; j < 2; j++) {
                
                for(int k = 0; k < n_states; k++) {
                int m = (int) pow(2, k);
                if(m & dfa_tran[dfa_states[i]][j])
                    printf("q%d", k);
                }

                if(j == 0)
                    printf("||");
            }
            printf("\n");
        }

        //PSED
        // int ctr = 0;
        //00000000000 is state phi, not state 0 (state 0 == q0 == 0*1)
        // for(int i = 0; i < n_states; i++) {
        //     for(int j = 0; j < 2; j++) {
        //         int comb_state = 0;//stf
                
        //         for(int k = 0; k < n_states; k++) {
        //             if(nfa_tran[i][j][k] == 1)
        //                 comb_state += (int) pow(2, k);
        //         }
                
        //         dfa_transn[(int) pow(2, i)][j] = comb_state;
        //         //if any elt of dfa_trans is a 0, it means that that state is phi

        //         if(states[comb_state] == 0)
        //             new_states[ctr++] = comb_state;
                
        //         states[comb_state] = 1;
        //     }
        // }

        //so till now, the previous loop assigned transitions for all existing states in the nfa as per the dfa we're building



        //the next loop will assign transitions as per the dfa for all the new states that we built during the previous loop
        //PSED
        // for(int i = 0; i < ctr; i++) {
        //     for(int j = 0; j < 2; j++) {
        //         int comb_state = 0;

        //         for(int k = 0; k < n_states; k++) {
                    
        //             int m = (int) pow(2, k);
        //             if(m & new_states[i]) {  //checking if the state k is part of the new state denoted by new_states[i]
        //                 if(comb_state == 0)
        //                     comb_state = dfa_transn[m][j];
                        
        //                 comb_state = comb_state | dfa_transn[m][j];
        //             }
        //         }

        //         if(states[comb_state] == 0) {
        //             new_states[ctr++] = comb_state;
        //             states[comb_state] = 1;
        //         }
        //     }
        // }
    }






    return 0;
}