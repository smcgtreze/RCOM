#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>



//struct hostent* gethostbyname (const char* name);

#define SERVER_PORT 21
//#define SERVER_ADDR "192.168.28.96"
#define SERVER_ADDR "193.137.29.15"
#define MAX_STRING_LENGTH 50
#define h_addr h_addr_list[0]

//ftp://abrilsuperinteressante.files.wordpress.com/2018/07/52d0469d865be2752d0003a9emperor-cold_hg.jpeg?quality=70&strip=info

 void AddressVerifier(char *address){
     char protocol[7];
     strcpy(protocol,address);
	 printf("%s\n",protocol);
     if(strcmp(protocol,"ftp://") != 0){
         printf("Not valid protocol\n");
     }
     if(strlen(protocol) != 6){
         printf("Not a valid protocol\n");
     }
 }

int receiveMessage(char *message,int sockfd){
     //char buffer[MAX_STRING_LENGTH];
     //char thrash[MAX_STRING_LENGTH];
     //strcpy(buffer,message);
	
    // if(strlen(buffer) <= 0){
     //    printf("erro na mensagem\n");
      //   close(sockfd);
       //  exit(1);
     //}
	 char c,buf[10];
	 memset(buf,0,10);
	 int totalRead; 
	 for(int i=0;i < 3;i++){
     	totalRead = read(sockfd,&c,1);
	 	printf("Lido da socket\n");
		printf("Erro:%d\n",totalRead);
	 	printf("%c",c);
		buf[i] = c;
	}
	
    // if(buffer[3] != ' '){
     //   while(totalRead == MAX_STRING_LENGTH){
     //       sleep(1);
     //        int totalRead = read(sockfd,thrash,MAX_STRING_LENGTH);
     //        if(thrash[3] == ' ')
     //           break;
     //  }
    //}

    if(totalRead <= 0){
		printf("Erro de leitura");        
		shutdown(sockfd,SHUT_RDWR);
        close(sockfd);
        exit(1);
    }

	if(strcmp(buf,"220") != 0){
		printf("Erro na resposta recebida\n");
	}

    return totalRead;
}

int main(int argc, char** argv){

	int	sockfd;
	int	sockfdClient;
	struct	sockaddr_in server_addr;
 	struct hostent *h;
	char	buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";  
	int	bytes;
	char User[MAX_STRING_LENGTH];
	char Pass[MAX_STRING_LENGTH];
	char Host[MAX_STRING_LENGTH];
	char Path[MAX_STRING_LENGTH];
	char Filename[MAX_STRING_LENGTH];

   /* if(argc != 3){
        printf("Erro no formato do username/pass\n");
        exit(1);
    }*/


	    if (argc != 2) {  
		    fprintf(stderr,"usage: getip address\n");
		    exit(1);
	     }

		if ((h=gethostbyname(argv[1])) == NULL) {  
		    herror("gethostbyname");
		    exit(1);
	    }

	printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));


	strcpy(User,"anonymous");
	strcpy(Pass,"none");
	strcpy(Host,SERVER_ADDR);
	strcpy(Path,"ftp://abrilsuperinteressante.files.wordpress.com/2018/07/");
	strcpy(Filename,"52d0469d865be2752d0003a9emperor-cold_hg.jpeg");
    //AddressVerifier(User);

       if(strlen(User) == 0){
        printf("Erro num dos parâmetros\n");
    }
       if(strlen(Pass) == 0){
        printf("Erro num dos parâmetros\n");
    }
       if(strlen(Path) == 0){
        printf("Erro num dos parâmetros\n");
    }
       if(strlen(Host) == 0){
        printf("Erro num dos parâmetros\n");
    }
        if(strlen(Filename) == 0){
         printf("Erro num dos parâmetros\n");
    }

    sockfd = connectnow();
	printf("Conectado Com sucesso\n");
	response(sockfd,User,Filename,"331");
	response(sockfd,User,Filename,"331");

	close(sockfd);
	exit(0);
}

 int connectnow(){

    struct hostent* host;
    int sockfd;

    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char*)&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	/*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(SERVER_PORT);		/*server TCP port must be network byte ordered */
	
    //printf("Server Adress: %s\n",server_addr.sin_addr.s_addr);
    /*open an FTP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(0);
    	}

	/*connect to the server*/
    printf("Connecting...\n");
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        	perror("connect()");
		    exit(0);
	}
    printf("Conection Estabilished\n");
    char *message;

    int state = receiveMessage(message,sockfd);
	//printf("message:%s",message);
    if(state <= 0){
        printf("Erro na leitura\n");
    }
	return sockfd;
	
 }

int response(int socketfd, char user[],char filename[],char response[]){
	int i=0;	
	char buffer[50];
	memset(buffer,0,50);
	char buf[50];
	memset(buffer,0,50);
	strcat(buffer,"user ");
	strcat(buffer,user);
	strcat(buffer,"\n");
	int totalRead = write(socketfd,buffer,strlen(buffer));
	printf("Username enviado :%s\n",buffer);
	/*while(read(socketfd,buf,1) != 0){
		printf("%s",buf);
		++i;
		if(buf[i] == "."){
			break;
		}
	}*/
}
 
