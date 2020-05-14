// Operating Systemes - Assigment 2 - ex2_srv.c //
// גל יעקובסון : 205585227 //
// רון פייביש : 313541344 //

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
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
int mystrlen(char* s){
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
    
    if(num>=0){
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
    }
    else{
        char str[SIZE];
        temp=temp* (-1);
        num=num * (-1);
        for (i = 0; temp != 0; i++)
        {
            temp /= 10;
            counter++;
        }
        str[counter] = '\0';
        for (i = 0; counter > 0; counter--)
        {
            str[counter - 1] = num % 10 + '0';
            num /= 10;
        }
        my_strcat(ret,"-");
        my_strcat(ret,str);
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

//Calculation function:
int calc(int num1,int operation,int num2){
    switch (operation) {
        case 1://Plus
            return num1+num2;
        case 2://Minus
            return num1-num2;
        case 3://Mul
            return num1*num2;
        case 4://DIV
            return num1/num2;
        default:
            Error();
    }
    return 0;
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

//File handler function:
void Files(){
    int to_srv,to_clientID;
    char to_client[SIZE]="",PID[SIZE],num1[SIZE],operation[SIZE],num2[SIZE];
    
    // Reading from to_srv file:
    if((to_srv=open("to_srv.txt",O_RDWR|O_APPEND)) == -1) Error();
    lineReader(to_srv, PID);
    lineReader(to_srv, num1);
    lineReader(to_srv, operation);
    lineReader(to_srv, num2);
    if((to_srv=remove("to_srv.txt")!=0)) Error();
    
    // Calculation:
    char charResult[SIZE]="";
    if((myAtoi(operation)==4) && myAtoi(num2)==0)
        my_strcat(charResult,"Math Error !\n");
    else
    {
        int intResult=calc(myAtoi(num1),myAtoi(operation),myAtoi(num2));
        convertor(intResult,charResult);
    }
    
    // Writing result to to_client file:
    my_strcat(to_client,"to_client_");
    my_strcat(to_client,PID);
    my_strcat(to_client,".txt");
    if ((to_clientID=open(to_client,O_RDWR|O_APPEND|O_CREAT,0666)) == -1) Error();
    if((write(to_clientID,&charResult,mystrlen(charResult)))<0)
    {close(to_clientID);Error();}

    puts("\n***** End of Stage 2 ***** \n");
    // Signaling the client:
    kill(myAtoi(PID),SIGUSR2);
    exit(-1);
};

//File handler function:
void sig_hand(int sig){
    puts("\n***** End of Stage 1 ***** \n");
    signal(SIGUSR1,sig_hand);
    int stat;
    pid_t PID;
    
    if((PID=fork())<0)
       Error();
    else if (PID==0)
       Files();
    else
       if(waitpid(PID,&stat,0)==-1)
           Error();
};

//File handler function:
void alarm_hand(int sig){
    signal(SIGALRM,alarm_hand);
    printf("The server was closed because no service request was received for the last 60 seconds \n");
    exit(0);
};

int main(int argc, char* argv[]){
    if(access("to_srv.txt", F_OK)==0)
        if(remove("to_srv.txt")!=0) Error();
    
    signal(SIGALRM,alarm_hand);
    signal(SIGUSR1,sig_hand);
    while(1){
        alarm(60);
        pause();
    }
}
