#include <stdio.h>
#include <fcntl.h>
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
#define MAX_STRING_LENGTH 75
#define h_addr h_addr_list[0]

int Receive(int sockfd, char expected[]);

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
    char c;

	while(estado != 3){
        printf("%c",c);
        //printf("Expecteded:%s",expected);
	switch(estado){
		case 0:
			totalRead = read(sockfd,&c,1); 
			if(totalRead > 0 && c == expected[0]){
                printf("estado:%d\n",estado);
				estado=1;
			}
			else{
				estado=0;
			}
			break;
		case 1:
			totalRead = read(sockfd,&c,1);
			if(totalRead > 0 && c == expected[1]){
                printf("estado:%d\n",estado);
				estado=2;
			}
			else{
				estado=0;
			}
			break;
		case 2:
			totalRead = read(sockfd,&c,1);
			if(totalRead > 0 && c == expected[2]){
                printf("estado:%d\n",estado);
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
        printf("\033[0;31m");
		printf("Escreve o host name desejado(ex: ftp.up.pt)\n");
        printf("\033[0m");
	    if (argc != 2) {  
		    fprintf(stderr,"usage: getip address\n");
		    exit(1);
	     }

		// a struct h contém o host
		if ((h=gethostbyname(argv[1])) == NULL) {  
		    herror("gethostbyname");
		    exit(1);
	    }
    printf("\033[0;32m");
	printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));
    printf("\033[0m");

	strcpy(User,"anonymous");
	strcpy(Pass,"anonymous");
	strcpy(Host,SERVER_ADDR);
    strcpy(Filename,"/pub/sage/robots.txt");
	
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
    sockfd = connectnow(SERVER_PORT);
	printf("\033[1;34m");
	printf("Conectado Com sucesso\n");
	printf("\033[0m;");
	response(sockfd,"user",User,Filename,"220");
	//tlv n seja necesário
	printf("Passwork required for euproprio\n");
	response(sockfd,"pass",Pass,Filename,"331");
	printf("User logged in\n");
    Receive(sockfd,"230"); 
	response(sockfd, NULL ,"pasv\r\n",Filename,"227");
    //printf("PASSEI\n");
	//printf("Entering Passive Mode\n");

    //char thrash[MAX_STRING_LENGTH];
    // int i=0,j=0;
    // char porta1[10],porta2[10];
    // int port1,port2;
   


    int state = 0;
	int index = 0;
    char charResponse[2];
	memset(charResponse, 0, 2);
    char largeByte[4];
    memset(largeByte, 0, 4);
	char smallByte[4];
	memset(smallByte, 0, 4);

   // char c;

    while (state != 7)
	{
		read(sockfd, charResponse, 1);
		charResponse[1] = '\0';
		switch (state)
		{
		//waits for 3 digit number followed by ' '
		case 0:
			if (charResponse[0] == '(')
			{
				state = 1;
			}
			break;
		case 1:
			if (charResponse[0] == ',')
			{
				state++;
			}
			break;
        case 2:
			if (charResponse[0] == ',')
			{
				state++;
			}
			break;
        case 3:
			if (charResponse[0] == ',')
			{
				state++;
			}
			break;
        case 4:
			if (charResponse[0] == ',')
			{   

				state++;
			}
			break;
        case 5:
			if (charResponse[0] == ',')
			{
                largeByte[index] = charResponse[0];
				state++;
                index = 0;
			}
			

            else{
                largeByte[index] = charResponse[0];
                index++;
            }
            
            break;
		case 6:
			if (charResponse[0] == ')')
			{
                smallByte[index] = '\0';
				state++;
                index=0;
			}
			else
			{
				smallByte[index] = charResponse[0];
				index++;
			}
			break;
	
		}
	}

	int first = atoi(largeByte);
	int second = atoi(smallByte);
    
	int port1 =  (first * 256 + second);

    printf("first = %d  second %d  port1 %d \n", first, second, port1);

    // // / from clientTCP.c
	// bzero((char*)&server_addr,sizeof(server_addr));
	// server_addr.sin_family = AF_INET;
	// server_addr.sin_addr.s_addr = inet_addr(ipAddress);
	// server_addr.sin_port = htons(port1);
    sockfdClient= connectnow(port1);
    
    char retr[MAX_STRING_LENGTH];
    char meleon[MAX_STRING_LENGTH];
    strcat(retr,"retr ");
    strcat(retr,Filename);
    strcat(retr,"\n");
    
    int j=0;
    printf("%s",retr);
    write(sockfd,retr,100);
    // for(int i=0;i < strlen(Filename);i++){
    //     if(Filename[i] == '/'){
    //         ++j;
    //     }
    //     if(j==3){
    //         meleon[i] = Filename[i]; 
    //     }
    // }
    // meleon[strlen(meleon)-1]='\0';
    char * token = strtok(Filename, "/");
    int k=0;
   // loop through the string to extract all other tokens
    while( token != NULL ) {
      ++k;
      printf( " %s\n", token ); //printing each token
      if(k == 3){
          strcpy(meleon,token);
      }
      token = strtok(NULL, "/");
   }
    //printf("Ficheiro :%s\n",meleon);


    int fd = open(meleon, O_RDWR);
    if(fd <0 ){
        printf("Erro a abrir o ficheiro\n");
    }
    char mander;
    char izard[512];
    int i=0;
    while(read(sockfdClient,&mander,1) > 0){
        izard[i]=mander;
        write(fd,&izard[i],1);
    }


	//Comentar isto "193.137.29.15" Vai ser preciso mudar a porta
    // while(1){
    //     read(sockfd,&c,1);
    //     thrash[i]= c;
    //     printf("%c",c);
    //     if(thrash[i] == ','){
    //         ++j;
    //     }
    //     else if(j == 4 ){

    //         read(sockfd,porta1,3);
    //         i+=3;
    //         porta1[3]='\0';
    //         port1= atoi(porta1);
    //         printf("Porta1 :%d",port1);


    //         if(thrash[i] == ')'){
    //         //printf("cheguei ao .\n");
    //             br;eak
    //     }
            
    //     }

    //     else if(j == 5){
            
    //         read(sockfd,porta2,3);
    //         i+=3;
    //         porta2[3] = '\0';
    //         port2= atoi(porta2);
          
    //         printf("Porta2 :%d",port2);
    //         if(thrash[i] == ')'){
    //         //printf("cheguei ao .\n");
    //             break;
    //     }
    //     }
    //     i++;
    //     //printf("valores do i %d e J %d\n", i, j);
        
    // }

    //int porta = atoi(rcv[0]);

    //printf("Porta:%s\n", porta1);
    //printf("Porta:%s\n", porta2);
	// Receive(sockfd,"193");
    // read(sockfd,NULL,1); 
	// Receive(sockfd,"137");
    // read(sockfd,NULL,1);
	// Receive(sockfd,"29");
    // read(sockfd,",",1);
	// Receive(sockfd,"15");
    // read(sockfd,",",1);
    
	//
	//ler da porta sem verificar
	//porta = rcv[0]*256 + rcv[1];
    //printf("porta: %s",porta);
	//abrir outra ligação para a outra socket sacar o file
	
	
    printf("Boas festas!\n");
    close(fd);
    close(sockfdClient);
	close(sockfd);
	exit(0);
}

 int connectnow(int port){

    struct hostent* host;
    int sockfd;

    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char*)&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	/*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */
	
    //printf("Server Adress: %s\n",server_addr.sin_addr.s_addr);
    /*open an FTP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(0);
    	}

	/*connect to the server*/
    printf("\033[1;33m");
    printf("Connecting...\n");
    printf("\033[0m");
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        	perror("connect()");
		    exit(0);
	}
    //char *message;

    //int state = receiveMessage(message,sockfd);
	//printf("message:%s",message);
    
	// int state = Receive(sockfd,"220");
    // if(state <= 0){
    //     printf("Erro na leitura\n");
    // }
     
	printf("Conection Estabilished\n");
	return sockfd;
	
 }

int response(int socketfd,char type[], char user[],char filename[],char response[]){
	int i=0;	
	char tosend[50];
	memset(tosend,0,50);
	char buf[50];
	memset(buf,0,50);
    if(type != NULL){
        strcat(tosend,type);//tipo de informação a enviar
        strcat(tosend," ");//tipo de informação a enviar
        strcat(tosend,user);// a propria informação
        strcat(tosend,"\n");// o enter a colocar no terminal
    }
    else{
        strcpy(tosend,user);
    }

	int totalRead = write(socketfd,tosend,strlen(tosend));
	printf("%s enviado :%s\n",type,tosend);

	// ver se a resposta está certa1
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

