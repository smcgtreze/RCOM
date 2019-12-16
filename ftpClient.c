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
#define SERVER_ADDR "193.137.29.15" // endereço do host name ftp.up.pt
#define MAX_STRING_LENGTH 50
#define h_addr h_addr_list[0]


 void AddressVerifier(char *address){
     char protocol[7];
	 memset(protocol,0,7);

     strcpy(protocol,address);
	 //printf("%s\n",protocol);

	 //verifica se o protocolo é o correto
     if(strcmp(protocol,"ftp://") != 0){
         printf("Not valid protocol\n");
     }
 
 }

int Receive(int sockfd, char expected[]){
	char received;
	int estado = 0, totalRead = 0;
	while(estado != 3){
	switch(estado){
		case 0:
			totalRead = read(sockfd,&c,1);
			if(totalRead > 0 && c == expected[0]){
				estado=1;
			}
			else{
				estado=0;
			}
			break;
		case 1:
			totalRead = read(sockfd,&c,1);
			if(totalRead > 0 && c == expected[1]){
				estado=2;
			}
			else{
				estado=0;
			}
			break;
		case 2:
			totalRead = read(sockfd,&c,1);
			if(totalRead > 0 && c == expected[2]){
				estado=3;
			}
			else{
				estado=0;
			}
			break;
	}
	}
	return 1;
}

/*
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
}*/

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

		printf("Escreve o host name desejado(ex: ftp.up.pt)\n");

	    if (argc != 2) {  
		    fprintf(stderr,"usage: getip address\n");
		    exit(1);
	     }

		// a struct h contém o host
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

	// a função conect now retorna o filedescriptor da socket a que nos conectamos
    sockfd = connectnow();
	printf("\033[1;34m");
	printf("Conectado Com sucesso\n");
	printf("\033[0m;");
	response(sockfd,"user",User,Filename,"331");
	//tlv n seja necesário
	printf("Passwork required for euproprio\n");
	response(sockfd,"pass",Pass,Filename,"230");
	printf("User logged in\n");
	response(sockfd,null,"pasv",Filename,"227");
	printf("Entering Passive Mode\n");

	//Comentar isto "193.137.29.15" Vai ser preciso mudar a porta 
	/*Receive(sockfd,"193");
	Receive(sockfd,"137");
	Receive(sockfd,"29");
	Receive(sockfd,"15");
	//
	int porta;
	char rcv[2];
	//ler da porta sem verificar
	porta = rcv[0]*256 + rcv[1];
	//abrir outra ligação para a outra socket sacar o file
	*/
	

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
    //char *message;

    //int state = receiveMessage(message,sockfd);
	//printf("message:%s",message);
	int state = Receive(sockfd,220);
    if(state <= 0){
        printf("Erro na leitura\n");
    }
	printf("Conection Estabilished\n");
	return sockfd;
	
 }

int response(int socketfd,char type[], char user[],char filename[],char response[]){
	int i=0;	
	char tosend[50];
	memset(tosend,0,50);
	char buf[50];
	memset(buf,0,50);

	strcat(tosend,type);//tipo de informação a enviar
	strcat(tosend,user);// a propria informação
	strcat(tosend,"\n");// o enter a colocar no terminal

	int totalRead = write(socketfd,tosend,strlen(tosend));
	printf("%s enviado :%s\n",type,tosend);

	// ver se a resposta está certa
	Receive(socketfd,response);
	printf("%s correto\n",type,tosend);

	//esta linha de código imprime tudo o que o servidor retorna quando entras
	/*while(read(socketfd,buf,1) != 0){
		printf("%s",buf);
		++i;
		if(buf[i] == "."){
			break;
		}
	}*/
}
 
