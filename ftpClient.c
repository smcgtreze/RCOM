#include <stdio.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <stdlib.h>
//#include <unistd.h>
#include <signal.h>
//#include <netdb.h>
#include <string.h>
//#include <strings.h>

struct hostent*
gethostbyname (const char* name);
struct hostent{
char* hname; /*nome oficial*/
char** haliases;
int h_addrtype; /*AF_INET*/
int h_length;
char** h_addr_list;
};

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"
#define MAX_STRING_LENGTH 50
#define h_addr h_addr_list[0]

int main(int argc, char** argv){

	int	sockfd;
	struct	sockaddr_in server_addr;
	char	buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";  
	int	bytes;
	char User[MAX_STRING_LENGTH];
	char Pass[MAX_STRING_LENGTH];
	char Host[MAX_STRING_LENGTH];
	char Path[MAX_STRING_LENGTH];
	char Filename[MAX_STRING_LENGTH];

    if(argc != 2){
        printf("Erro no formato do username/pass\n");
        exit(1);
    }

    AddressVerifier(argv[1]);
    User = argv[1];
    Pass = argv[2];

       if(strlen(User) == 0){
        printf("Erro num dos parâmetros\n")
    }
       if(strlen(Pass) == 0){
        printf("Erro num dos parâmetros\n")
    }
       if(strlen(Path) == 0){
        printf("Erro num dos parâmetros\n")
    }
       if(strlen(Host) == 0){
        printf("Erro num dos parâmetros\n")
    }
        if(strlen(Filename) == 0){
         printf("Erro num dos parâmetros\n")
    }

    Connectnow();



	close(sockfd);
	exit(0);
}

 void AddressVerifier(char adress[]){
     char protocol[7];
     strcpy(protocol,address);
     if(strcmp(protocol,"ftp://") != 0){
         printf("Not valid protocol\n");
     }
     if(strlen(protocol) != 7){
         printf("Not a valid protocol\n");
     }
 }

 int Connectnow(){

    struct hostent* host;

    /*server address handling*/
    bzero((char*)&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	/*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(SERVER_PORT);		/*server TCP port must be network byte ordered */

    /*open an FTP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(0);
    	}

	/*connect to the server*/
    	if(connect(sockfd, 
	           (struct sockaddr *)&server_addr, 
		   sizeof(server_addr)) < 0){
        	perror("connect()");
		    exit(0);
	}

    char message[MAX_STRING_LENGTH];

    int state = receiveMessage(message,sockfd);
    if(state <= 0){
        printf("Erro na leitura\n");
    }
 }

 int receiveMessage(char *message,int sockfd){
     char buffer[MAX_STRING_LENGTH];
     char thrash[MAX_STRING_LENGTH];
     strcpy(buffer,message);
     
     if(strlen(buffer) <= 0){
         printf("erro na mensagem\n");
         close(sockfd);
         exit(1);
     }

     int totalRead = read(sockfd,buffer,MAX_STRING_LENGTH);

    if(buffer[3] != ' '){
        while(totalRead == MAX_STRING_LENGTH){
            sleep(1);
             int totalRead = read(sockfd,thrash,MAX_STRING_LENGTH);
             if(thrash[3] == ' ')
                break;
        }
    }

    if(totalRead <= 0){
        shutdown(sockfd,SHUT_RDWR);
        close(sockfd);
        exit(1);
    }
    buffer[totalRead]= "\0";
    return totalRead;


 }



