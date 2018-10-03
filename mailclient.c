#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

void signup(int);
void signin(int);
void loggedin(int, char[]);
void compose(int, char[]);
void inbox(int, char[]);
void sentitems(int, char[]);

void Die(char *mess)
{
	perror(mess);
	exit(1); 
}

int main(int argc, char *argv[])
{
	int sockfd, ch;
	struct sockaddr_in sadrs;
	char intro[150];	

	if(argc < 3)
	{
		printf("\nUsage: %s <IP address> <port number>\n", argv[0]);
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		Die("\nFailed to create socket");

	memset(&sadrs, 0, sizeof(sadrs));
	sadrs.sin_family = AF_INET;
	sadrs.sin_addr.s_addr = inet_addr(argv[1]);
	sadrs.sin_port = htons(atoi(argv[2]));

	if(connect(sockfd, (struct sockaddr *)&sadrs, sizeof(sadrs)) < 0)
		Die("\nFailed to connect with server");

	a:
	bzero(intro, 150);
	system("clear");
	read(sockfd, &intro, 150);
	printf("%s", intro);
	fflush(stdin);
	scanf("%d", &ch);
	write(sockfd, &ch, 2);
	switch(ch)
	{
		case 1: 
		{
			signup(sockfd);
			goto a;
		}
			break;
		case 2:
		{
			signin(sockfd);
			goto a;
		}
			break;
		case 3:
		{
			close(sockfd);
			exit(0);
		}
			break;
		default:
			goto a;
			break;
	}
	close(sockfd);
	exit(0);
}

void signup(int sockfd)
{
	char usr[100], usrnm[20], str[100], alrexi[100], pswrd[20], confo[50];
	b:
	system("clear");
	bzero(usr, 100);
	bzero(usrnm, 20);
	bzero(str, 100);
	bzero(alrexi, 100);
	bzero(pswrd, 20);
	bzero(confo, 50);
	read(sockfd, &usr, 100);
	printf("%s", usr);
	scanf("%s", usrnm);
	write(sockfd, &usrnm, 20);
	read(sockfd, &str, 100);
	if(strcmp(str, "Found") == 0)
	{
		read(sockfd, &alrexi, 100);
		printf("%s", alrexi);
		system("sleep 2");
		goto b;
	}
	printf("%s", str);
	scanf("%s", pswrd);
	write(sockfd, &pswrd, 20);
	read(sockfd, &confo, 50);
	printf("%s", confo);
	system("sleep 2");
}

void signin(int sockfd)
{
	char usr[100], usrnm[20], str[100], noexi[100], pswrd[20], str1[100], retyp[100];
	c:
	system("clear");
	bzero(usr, 100);
	bzero(usrnm, 20);
	bzero(str, 100);
	bzero(noexi, 100);
	bzero(str1, 50);
	read(sockfd, &usr, 100);
	printf("%s", usr);
	scanf("%s", usrnm);
	write(sockfd, &usrnm, 20);
	read(sockfd, &str, 100);
	if(strcmp(str, "Not Found") == 0)
	{
		read(sockfd, &noexi, 100);
		printf("%s", noexi);
		system("sleep 2");
		goto c;
	}
	printf("%s", str);
	d:
	bzero(pswrd, 20);
	bzero(retyp, 100);
	scanf("%s", pswrd);
	write(sockfd, &pswrd, 20);
	read(sockfd, &str1, 100);
	printf("\n%s\n", str1);
	if(strcmp(str1, "Password Unmatched") == 0)
	{
		read(sockfd, &retyp, 100);
		printf("%s", retyp);
		goto d;
	}
	system("sleep 2");
	loggedin(sockfd, usrnm);
}

void loggedin(int sockfd, char usrnm[20])
{
	char logintro[150], signout[50];
	int ch;
	e:
	system("clear");
	bzero(logintro, 150);
	read(sockfd, &logintro, 150);
	printf("%s", logintro);
	fflush(stdin);
	scanf("%d", &ch);
	write(sockfd, &ch, 2);
	switch(ch)
	{
		case 1:
		{
			compose(sockfd, usrnm);
			goto e;
		}
			break;
		case 2:
		{
			inbox(sockfd, usrnm);
			goto e;
		}
			break;
		case 3:
		{
			sentitems(sockfd, usrnm);
			goto e;
		}
			break;
		case 4:
		{
			read(sockfd, &signout, 50);
			printf("%s", signout);
			system("sleep 2");
		}
			break;
		default:
			goto e;
			break;
	}
}

void compose(int sockfd, char usrnm[20])
{
	char tor[30], tos[20], msgr[20], msgs[1024], str[1024], sof[30];
	system("clear");
	bzero(tor, 30);
	bzero(tos, 20);
	bzero(msgr, 20);
	bzero(msgs, 200);
	bzero(str, 200);
	bzero(sof, 30);
	read(sockfd, &tor, 30);
	printf("%s", tor);
	scanf("%s", tos);
	write(sockfd, &tos, 20);
	read(sockfd, &msgr, 20);
	printf("%s", msgr);
	while(1)
	{
		gets(str);
		if(strcmp(str, "send") == 0)
			break;
		if(strcmp(str, "cancel") == 0)
		{
			strcpy(msgs, str);
			break;
		}
		strcat(msgs, str);
		strcat(msgs, "\n");
	}
	write(sockfd, &msgs, 1024);
	read(sockfd, &sof, 30);
	printf("%s", sof);
	system("sleep 2");
}

void inbox(int sockfd, char usrnm[20])
{
	char str[1024];
	system("sleep 2");
	system("clear");
	bzero(str, 1024);
	printf("\n\nINBOX\n\n");
	read(sockfd, &str, 1024);
	printf("%s", str);
	printf("\n\nPress ENTER to go back...\n");
	getchar();
	getchar();
}

void sentitems(int sockfd, char usrnm[20])
{
	char str[1024];
	system("sleep 2");
	system("clear");
	bzero(str, 1024);
	printf("\n\nSENT ITEMS\n\n");
	read(sockfd, &str, 1024);
	printf("%s", str);
	printf("\n\nPress ENTER to go back...\n");
	getchar();
	getchar();
}
