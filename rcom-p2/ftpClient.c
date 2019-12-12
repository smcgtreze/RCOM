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
#include <fcntl.h>

#define FTP_PORT 21
#define MAXLENGTH 1024

char user[MAXLENGTH];
char password[MAXLENGTH];
char host[MAXLENGTH];
char path[MAXLENGTH];
char* ipAddress;

int sockfd;
int datasockfd;
int portConnection;

/**
 * Function to verify the input address and save credentials, host and path into arrays.
 * @param address Address string.
 */
void verifyAddress(char* address)
{
	char protocol[7];

	bzero(protocol, 7);

	strncpy(protocol, address, 6);

	//Verify protocol
	if(strcmp(protocol, "ftp://") != 0)
	{
		printf("Not valid protocol!\n");
		exit(1);
	}

	if (strlen(address) == 6)
	{
		printf("Address not found!\n");
		exit(1);
	}

	char * separatorUser = strchr(address + 6, ':');
	char * separatorAt;
	char * separatorPath;

	// Verify if there is an user
	if(separatorUser != NULL)
	{
		//verify char separators
		
		separatorAt = strchr(separatorUser, '@');

		if(separatorAt == NULL)
		{
			printf("Missing separator @ after user:password!\n");
			exit(1);
		}

		separatorPath = strchr(separatorAt, '/');

		if(separatorPath == NULL)
		{
			printf("Missing separator between host and path!\n");
			exit(1);
		}

		char userLength = separatorUser - address - 6;
		char passwordLength = separatorAt - separatorUser - 1;
		char hostLength = separatorPath - separatorAt - 1;
		char pathLength = address + strlen(address) - separatorPath - 1;

		if(userLength == 0)
		{
			printf("User not found\n");
			exit(1);
		}

		if(passwordLength == 0)
		{
			printf("Password not found\n");
			exit(1);
		}

		if(hostLength == 0)
		{
			printf("Host not found\n");
			exit(1);
		}

		if(pathLength == 0)
		{
			printf("Path not found\n");
			exit(1);
		}

		strncpy(user, address+6, userLength);
		strncpy(password, separatorUser + 1, passwordLength);
		strncpy(host, separatorAt + 1, hostLength);
		strcpy(path, separatorPath + 1);
	}


	else if(separatorUser == NULL)
	{
		// verify char separators
		
		separatorAt = strchr(address, '@');

		if(separatorAt != NULL)
		{
			printf("There is no user and password!\n");
			exit(1);

		}

		separatorPath = strchr(address + 6, '/');

		if(separatorPath == NULL)
		{
			printf("Missing separator between host and path!\n");
			exit(1);
		}

		char hostLength = separatorPath - address - 6;
		char pathLength = address + strlen(address) - separatorPath - 1;

		if(hostLength == 0)
		{
			printf("Host not found\n");
			exit(1);
		}

		if(pathLength == 0)
		{
			printf("Path not found\n");
			exit(1);
		}

		strncpy(host, address + 6, hostLength);
		strcpy(path, separatorPath + 1);
	}
}

/**
 * Function to get message sent from server.
 * @param message Message received from server.
 * @return Message code.
 */
int getMessage(char * message)
{
	bzero(message, MAXLENGTH);

	char buffer[MAXLENGTH];
	char trash[MAXLENGTH];
	bzero(buffer, MAXLENGTH);
	bzero(trash, MAXLENGTH);

	sleep(1);

	int totalRead = read(sockfd, buffer, MAXLENGTH);
	printf("%s\n", buffer);
	int i = 0;

	if(buffer[3] != ' ')
	{
		while(totalRead == MAXLENGTH)
		{
			sleep(1);
			totalRead = read(sockfd, trash, MAXLENGTH);
			printf("%s\n", trash);

			if(trash[3] == ' ')
				break;
		}
	}

	if(totalRead <= 0)
	{
		shutdown(sockfd,SHUT_RDWR);
		close(sockfd);
		exit(1);
	}

	for(i = 0; i < MAXLENGTH; i++)
	{
		if(buffer[i] < '0' || buffer[i] > '9')
			break;
	}

	char tempNum[5];
	bzero(tempNum,5);

	strncpy(tempNum, buffer, i); 

	//get message number
	int statusConnection = atoi(tempNum);

	buffer[totalRead] = '\0';

	strcpy(message, buffer + i);

	return statusConnection;
}

/**
 * Function to start connection with the given host.
 */
void startConnection()
{
	struct hostent *h;
	struct	sockaddr_in server_addr;

	// from getip.c with some changes
	if ((h=gethostbyname(host)) == NULL) {  
		printf("Error while getting ip address!\n");
		exit(1);
	}

	printf("Host name  : %s\n", h->h_name);

	ipAddress = inet_ntoa(*((struct in_addr *)h->h_addr));
	printf("IP Address : %s\n", ipAddress);

	// from clientTCP.c
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ipAddress);
	server_addr.sin_port = htons(FTP_PORT);

	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		perror("socket()");
		exit(1);
	}
	/*connect to the server*/
	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		perror("connect()");
		exit(1);
	}

	char message[MAXLENGTH];

	int state = getMessage(message);

	if(state != 220)
	{
		printf("Error while getting first message!\n");
		shutdown(sockfd,SHUT_RDWR);
		close(sockfd);
		exit(1);
	}
}

/**
 * Function to send credentials and open UDP socket.
 */
void sendStartData()
{
	char message[MAXLENGTH];
	char buffer[MAXLENGTH];
	int written;
	int state;

	// send credentials

	// send user
	bzero(message, MAXLENGTH);

	if(strlen(user) == 0)
	{
		strcpy(user, "anonymous");
	}

	printf("user: %s\n", user);

	strcpy(message, "USER ");
	strcat(message, user);
	strcat(message, "\r\n");

	written = write(sockfd, message, strlen(message));

	if(written <= 0)
	{
		printf("Error when writing user to socket\n");
		shutdown(sockfd,SHUT_RDWR);
		close(sockfd);
		exit(1);
	}

	bzero(buffer, MAXLENGTH);

	state = getMessage(buffer);

	if(state >= 400)
	{
		printf("Error while getting user authentication message!\n");
		shutdown(sockfd,SHUT_RDWR);
		close(sockfd);
		exit(1);
	}

	// send pass

	bzero(message, MAXLENGTH);

	if(strlen(password) == 0)
	{
		strcpy(password, "anonymous");
	}

	strcpy(message, "PASS ");
	strcat(message, password);
	strcat(message, "\r\n");

	written = write(sockfd, message, strlen(message));

	if(written <= 0)
	{
		printf("Error when writing pass to socket\n");
		shutdown(sockfd,SHUT_RDWR);
		close(sockfd);
		exit(1);
	}

	bzero(buffer, MAXLENGTH);

	state = getMessage(buffer);

	if(state >= 400)
	{
		printf("Error when receiving pass ack!\n");
		shutdown(sockfd,SHUT_RDWR);
		close(sockfd);
		exit(1);
	}
	bzero(buffer, MAXLENGTH);
	bzero(message, MAXLENGTH);

	// start passive mode
	strcpy(message, "PASV\r\n");
	written = write(sockfd, message, strlen(message));

	if(written <= 0)
	{
		printf("Error when writing PASV to socket\n");
		shutdown(sockfd,SHUT_RDWR);
		close(sockfd);
		exit(1);
	}

	state = getMessage(buffer);

	if(state >= 400)
	{
		printf("Error while getting password authentication message!\n");
		shutdown(sockfd,SHUT_RDWR);
		close(sockfd);
		exit(1);
	}

	int port1;
	int port2;

	char * commaRight = strrchr(buffer, ',');

	if(commaRight == NULL)
	{
		printf("ERROR when getting the comma on right\n");
		exit(1);
	}

	commaRight[0] = '\0';
	char * commaLeft = strrchr(buffer, ',');

	if(commaLeft == NULL)
	{
		printf("ERROR when getting the comma on left\n");
		exit(1);
	}

	commaLeft[0] = '\0';
	commaRight++;

	port1 = atoi(commaLeft + 1);
	port2 = atoi(commaRight);

	portConnection = port1*256 + port2;

	//data socket
	struct	sockaddr_in server_addr;

	// from clientTCP.c
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ipAddress);
	server_addr.sin_port = htons(portConnection);

	if ((datasockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		perror("socket()");
		exit(1);
	}
	/*connect to the server*/
	if(connect(datasockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		perror("connect()");
		exit(1);
	}
}

/**
 * Function to download file. It will be created in the executable folder.
 */
void receiveData(){

	int state;
	char message[MAXLENGTH];
	char buffer[MAXLENGTH];

	bzero(message, MAXLENGTH);
	bzero(buffer, MAXLENGTH);

	//send retrieve message
	strcpy(message, "RETR /");
	strcat(message, path);
	strcat(message, "\r\n");

	int written = write(sockfd, message, strlen(message));

	if(written <= 0)
	{
		printf("Error when writing RETR to socket\n");
		shutdown(sockfd,SHUT_RDWR);
		close(sockfd);
		shutdown(datasockfd,SHUT_RDWR);
		close(datasockfd);
		exit(1);
	}
	
	state = getMessage(buffer);

	if(state >= 400)
	{
		printf("Error while getting retrieve message!\n");
		shutdown(sockfd,SHUT_RDWR);
		close(sockfd);
		shutdown(datasockfd,SHUT_RDWR);
		close(datasockfd);
		exit(1);
	}

	// Open file here

	int fd;
	int readchars = 1;

	bzero(buffer, MAXLENGTH);

	char * filename = strrchr(path, '/') + 1;

	printf("filename: %s\n", filename);

	//delete last file
	unlink(filename);

	// open new file
	fd = open(filename, O_WRONLY | O_CREAT, 0777);

	if (fd < 0){
		printf("Problems opening file!\n");
		shutdown(sockfd,SHUT_RDWR);
		close(sockfd);
		shutdown(datasockfd,SHUT_RDWR);
		close(datasockfd);
		exit(1);
	}
	printf("Progress:\n");
	while(readchars>0)
	{
		bzero(buffer, MAXLENGTH);

		readchars=recv(datasockfd, buffer, MAXLENGTH, 0);
		printf(":");
		fflush(stdout);
		if(readchars > 0)
		{
			written = write(fd, buffer, readchars);
			
			if(written <= 0)
			{
				printf("Error when writing to file\n");
				shutdown(sockfd,SHUT_RDWR);
				close(sockfd);
				shutdown(datasockfd,SHUT_RDWR);
				close(datasockfd);
				exit(1);
			}
		}
	}

	close(fd);
	printf("\n");
	bzero(buffer, MAXLENGTH);

	printf("Finishing\n");
	sleep(3);

	// get last messages that could be in the socket
	recv(sockfd, buffer, MAXLENGTH, MSG_DONTWAIT);

	printf("%s\n", buffer);
}

/**
 * Main function.
 * @return Program state.
 */
int main(int argc, char *argv[])
{
	bzero(user, MAXLENGTH);

	if(argc != 2)
	{
		printf("usage: ftpClient ftp://[<user>:<password>@]<host>/<url-path>\n");
		exit(1);
	}

	verifyAddress(argv[1]);

	if(strlen(user) != 0){
		printf("%c[%d;%dmUSER: %c[%dm",27,1,35,27,0);
		printf("%s\n", user);

	}


	if(strlen(host) != 0){
		printf("%c[%d;%dmHOST: %c[%dm",27,1,35,27,0);
		printf("%s\n", host);
	}

	if(strlen(path) != 0){
		printf("%c[%d;%dmPATH: %c[%dm",27,1,35,27,0);
		printf("%s\n", path);
	}

	startConnection();

	sendStartData();
	receiveData();

	printf("%c[%d;%dmALL OK!!%c[%dm\n",27,1,32,27,0);

	shutdown(sockfd,SHUT_RDWR);
	close(sockfd);
	shutdown(datasockfd,SHUT_RDWR);
	close(datasockfd);

	return 0;
}
