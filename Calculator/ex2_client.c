// Operating Systemes - Assigment 2 - ex2_client.c //
// גל יעקובסון : 205585227 //
// רון פייביש : 313541344 //

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/random.h>
#define SIZE 255

//General Error function:
void Error(){
    printf("ERROR_FROM_EX2 \n");
    exit(-1);
};

//Strcut function:
char* my_strcat(char* destination, const char* source){
    // make ptr point to the end of destination string
    char* ptr = destination + strlen(destination);

    // Appends characters of source to the destination string
    while (*source != '\0')
        *ptr++ = *source++;

    // null terminate destination string
    *ptr = '\0';

    // destination is returned by standard strcat()
    return destination;
};

//Strlen function:
int mystrlen(char* s) {
    int sum = 0;
    while (*s != '\0')
    {
        sum++;
        s++;
    }
    return sum;
};

//Converte from in to char* function:
char* convertor(int num, char* ret){
    int temp = num, counter = 0, i = 0;

    for (i = 0; temp != 0; i++)
    {
        temp /= 10;
        counter++;
    }
    ret[counter] = '\0';
    for (i = 0; counter > 0; counter--)
    {
        ret[counter - 1] = num % 10 + '0';
        num /= 10;
    }
    return ret;
};

//Converte from char* to int function:
int myAtoi(char *str){
    int i = 0;
    int sign = 1;
    int val = 0;

    while (str[i] == '-') {
        sign = -sign;
        i++;
    }

    while(str[i] >= '0' && str[i] <= '9')
    {
        int nbr = (int) (str[i] - '0');

        val = (val * 10) + nbr;
        i++;
    }

    return (val * sign);
};

//write to file function:
void writeToFile(int fd, char* string){
    if(write(fd, string, mystrlen(string) + 1) < 0) {close(fd);Error();}
    if(write(fd,"\n",1)<0)
        {close(fd);Error();}
    return;
};

//Read from file function:
int lineReader(int fd, char* output){
    int it = 0;
    int current = 0;
    char buf;

    while (read(fd, &buf, 1) > 0)
    {
        if (buf != '\n')
            output[it++] = buf;
        else if (buf == '\n')
        {
            current++;
            break;
        }
    }
    if (it == 0)
        return 0;
    output[it] = '\0';

    return 1;
};

//Signal handler function:
void sig_hand(int sig){
    signal(SIGUSR2,sig_hand);
    int PID=getpid(),to_clientID;
    char to_client[SIZE]="",buff[SIZE],result[SIZE];
    char *pid=convertor(PID,buff);
    // Open&Read to_client file:
    my_strcat(to_client,"to_client_");
    my_strcat(to_client,pid);
    my_strcat(to_client,".txt");
    if((to_clientID=open(to_client,O_RDWR|O_APPEND|O_CREAT,0666)) == -1) Error();
    if(lineReader(to_clientID,result)==0)
    {close(to_clientID);Error();}
    if((remove(to_client)<0)) Error();
    
    puts("\n***** End of Stage 3 ***** \n");
    printf("For Client : %s \n",pid);
    printf("The Result is : %s \n",result);
    return;
};

//Alarm Signal handler function:
void alarm_hand(int sig){
    signal(SIGALRM,alarm_hand);
    printf("Client closed because no response was received from the server for 30 seconds");
    exit(0);
};

int main(int argc, char* argv[]){
    if(argc!=5) Error();
    
    int r;
    getrandom(&r,sizeof(int),GRND_RANDOM);
    r=(r<0)?(r*(-1)):r;
    sleep(r%5);
    
    int rand=0,flag=0,to_srv,CLT_PID=getpid();
    char buff[SIZE]="",result[SIZE];
    char *SRV_PID=argv[1],*num1=argv[2],*operation=argv[3],*num2=argv[4];
    
    signal(SIGUSR2,sig_hand);
    
    // 10 times loop for access to_srv:
    while(access("to_srv.txt", F_OK)==0){
        flag++;
        if(flag==10) Error();
        r=0;
        getrandom(&r, sizeof(int), GRND_RANDOM);
        r=(r<0)?(r*(-1)):r;
        sleep(r%6);
    }
    
    // Writing the argv to to_srv file:
    if((to_srv=open("to_srv.txt",O_RDWR|O_CREAT|O_TRUNC|O_APPEND, 0666))<0) Error();
    writeToFile(to_srv, convertor(CLT_PID,buff));
    writeToFile(to_srv, argv[2]);
    writeToFile(to_srv, argv[3]);
    writeToFile(to_srv, argv[4]);
    close(to_srv);
    
    // Signaling the server:
    kill(myAtoi(SRV_PID),SIGUSR1);
    signal(SIGALRM,alarm_hand);
    alarm(30);
    pause();
    
    printf("Done ! \n");
    return 0;
}
