#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int rep;

int main(){
    

    // a)
    printf("Identyfikator UID  %d\n", getuid());   // user ID
    printf("Identyfikator GID  %d\n", getgid());   // group user ID
    printf("Identyfikator PID  %d\n", getpid());   // identyfikator procesu
    printf("Identyfikator PPID %d\n", getppid());  // identyfikator (parent)
    printf("Identyfikator PGID %d\n", getpgid(0)); // identyfikator grupy (lidera)
    printf("----------------------------------------------\n");
    
    // b)
    pid_t pid;
    for(rep = 0;rep < 3;rep++)
        pid = fork();
    if(pid == -1){
       perror("Coś nie tak z fork()");
       exit(1);
    }

    if(pid == 0){
        printf("Proces potomny UID: %d, GID: %d, PID: %d, PPID: %d, PGID: %d\n", getuid(), getgid(), getpid(), getppid(), getpgid(0)); 
        exit(0); // zakończenie potomnego (nie jest potrzebne ale tak można pominąc else)
    }

    while(wait(NULL) != -1); // czeka aż wszyscy się zakończą
    printf("Proces macierzysty UID: %d, GID: %d, PID %d, PPID: %d, PGID: %d\n", getuid(), getgid(), getpid(), getppid(), getpgid(0));
    if(getpgid(0) != getpid()) exit(0); // kończymy wszystkich oprócz lidera.

    // Mamy znowu jeden proces ;)
    // c) 
    // PID = 1(systemd)

    

    return 0;
}
//                    /\     /\ 
//                  {  `---'  }
//                  {  O   O  }
//                  ~~>  V  <~~
//                   \ \|/ /
//                    `---'____
//                   /     \    \_
//                  {       }\  )_\_)-.
//                  |  \_/  ) / /  \ \
//                   \__/  /(_/     \_)
//                     (__/
//                /\_/\
//               ( o.o ) 
//                > ^ <
//                /     \
//               (       )
//              ( /     \ )
//             ( /       \)

// Ćwiczenie 3: podpunkt a)
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int sig){
        if(sig == SIGINT){
                printf("blah blah bleh bleh operacja własna obsługa\n");
                sleep(10);
                return ;

        }
}

int main(){
        //signal(SIGTSTP, SIG_IGN); // program od teraz będzie ignorował (Ctrl-Z)
        //pause();
        signal(SIGINT, sig_handler);
        pause();
        while(1) { // przerwanie z klawiatury a czy program dalej działa?
                printf("siema\n");
                sleep(1);
        }
        return 0;
}
