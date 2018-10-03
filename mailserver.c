#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>

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
	int ssfd, csfd;
	struct sockaddr_in sadrs, cadrs;

	if(argc < 2)
	{
		printf("\nUsage: %s <port number>\n", argv[0]);
		exit(1);
	}

	ssfd = socket(AF_INET, SOCK_STREAM, 0);
	if(ssfd < 0)
		Die("\nFailed to create socket");

	memset(&sadrs, 0, sizeof(sadrs));
	sadrs.sin_family = AF_INET;
	sadrs.sin_addr.s_addr = htonl(INADDR_ANY);
	sadrs.sin_port = htons(atoi(argv[1]));

	if(bind(ssfd,(struct sockaddr *)&sadrs, sizeof(sadrs)) < 0)
		Die("\nFailed to bind the server socket");

	listen(ssfd, 5);
	system("mkdir -p /home/sibalnirbhay/Desktop/DATABASE");
	signal(SIGCHLD, SIG_IGN);
	printf("\nSERVER ONLINE\n");

	while(1)
	{
		int ch = 0;
		char intro[150] = "\n\n\nMAIL \t SERVER\n1. Sign Up\n2. Sign In\n3. Exit\n\nEnter your choice: ";
		unsigned int clilen = sizeof(cadrs);

		csfd = accept(ssfd, (struct sockaddr *)&cadrs, &clilen);
		if(csfd < 0)
			Die("\nFailed to accept client connection");

		if(fork() == 0)
		{
			a:
			write(csfd, &intro, 150);
			read(csfd, &ch, 2);
			switch(ch)
			{
				case 1:
				{
					signup(csfd);
					goto a;
				}
					break;
				case 2:
				{
					signin(csfd);
					goto a;
				}
					break;
				case 3:
					close(csfd);
					break;
				default:
					goto a;
					break;
			}
		}
		else
			close(csfd);
	}
}

void signup(int csfd)
{
	FILE *db, *pwdsv;
	db = fopen("/home/sibalnirbhay/Desktop/DATABASE/userid.txt", "a+");
	char usr[100] = "\n\nCREATE YOUR ACCOUNT\n\nEnter your desired username: ";
	char usrnm[20], tmp[20], pswrd[20];
	char mkdir[100] = "mkdir /home/sibalnirbhay/Desktop/DATABASE/";
	char touch[200] = "touch /home/sibalnirbhay/Desktop/DATABASE/";
	char alrexi[100] = "\nUsername already exists\n";
	char found[100] = "Found";
	char pswd[100] = "\nEnter password: ";
	char pwd[100] = "/home/sibalnirbhay/Desktop/DATABASE/";
	char confo[50] = "\nAccount Created\n";
	b:
	rewind(db);
	bzero(usrnm, 20);
	bzero(tmp, 20);
	bzero(pswrd, 20);
	write(csfd, &usr, 100);
	read(csfd, &usrnm, 20);
	while(!feof(db))
	{
		fscanf(db, "%s", tmp);
		if(strcmp(usrnm, tmp) == 0)
		{
			write(csfd, &found, 100);
			write(csfd, &alrexi, 100);
			goto b;
		}
	}
	write(csfd, &pswd, 100);
	read(csfd, &pswrd, 20);
	strcat(mkdir, usrnm);
	strcat(touch, usrnm);
	strcat(touch, "/pswd.txt /home/sibalnirbhay/Desktop/DATABASE/");
	strcat(touch, usrnm);
	strcat(touch, "/inbox.txt /home/sibalnirbhay/Desktop/DATABASE/");
	strcat(touch, usrnm);
	strcat(touch, "/sentitems.txt");
	strcat(pwd, usrnm);
	strcat(pwd, "/pswd.txt");
	strcat(usrnm, " \n");
	fwrite(&usrnm, strlen(usrnm), 1, db);
	fclose(db);
	system(mkdir);
	system(touch);
	pwdsv = fopen(pwd, "w");
	fwrite(&pswrd, strlen(pswrd), 1, pwdsv);
	fclose(pwdsv);
	write(csfd, &confo, 50);
}

void signin(int csfd)
{
	FILE *db, *pwdsv;
	db = fopen("/home/sibalnirbhay/Desktop/DATABASE/userid.txt", "r+");
	char usr[100] = "\n\nLOG IN TO YOUR ACCOUNT\n\nUsername: ";
	char usrnm[20], tmp[20], pswrd[20], temp[20];
	int found = 0;
	int i;
	char noexi[100] = "\nUsername does not exist\n";
	char notfound[100] = "Not Found";
	char pswd[100] = "\nPassword: ";
	char pwd[100] = "/home/sibalnirbhay/Desktop/DATABASE/";
	char unmat[100] = "Password Unmatched";
	char retyp[100] = "\nRetype password: ";
	char sucss[100] = "\n\nLogging in...";
	c:
	rewind(db);
	bzero(usrnm, 20);
	bzero(tmp, 20);
	write(csfd, &usr, 100);
	read(csfd, &usrnm, 20);
	while(!feof(db))
	{
		fscanf(db, "%s", tmp);
		if(strcmp(usrnm, tmp) == 0)
		{
			found = 1;
			break;
		}
	}
	if(found == 0)
	{
		write(csfd, &notfound, 100);
		write(csfd, &noexi, 100);
		goto c;
	}
	strcat(pwd, usrnm);
	strcat(pwd, "/pswd.txt");
	pwdsv = fopen(pwd, "r");
	write(csfd, &pswd, 100);
	d:
	i = -1;
	rewind(pwdsv);
	bzero(pswrd, 20);
	bzero(temp, 20);
	read(csfd, &pswrd, 20);
	while(!feof(pwdsv))
		temp[++i] = fgetc(pwdsv);
	temp[i]='\0';
	if(strcmp(pswrd, temp) != 0)
	{
		write(csfd, &unmat, 100);
		write(csfd, &retyp, 100);
		goto d;
	}
	fclose(db);
	fclose(pwdsv);
	write(csfd, &sucss, 100);
	loggedin(csfd, usrnm);
}

void loggedin(int csfd, char usrnm[20])
{
	char logintro[150] = "\n\nHi ";
	strcat(logintro, usrnm);
	strcat(logintro, ",\n\n1. Compose\n2. Inbox\n3. Sent Items\n4. Sign Out\n\nEnter your choice: ");
	char signout[50] = "\n\nSigning Out..\n";
	int ch = 0;
	e:
	write(csfd, &logintro, 150);
	read(csfd, &ch, 2);
	switch(ch)
	{
		case 1:
		{
			compose(csfd, usrnm);
			goto e;
		}
			break;
		case 2:
		{
			inbox(csfd, usrnm);
			goto e;
		}
			break;
		case 3:
		{
			sentitems(csfd, usrnm);
			goto e;
		}
			break;
		case 4:
			write(csfd, &signout, 50);
			break;
		default:
			goto e;
			break;
	}
}

void compose(int csfd, char usrnm[20])
{
	FILE *db, *toinbox, *tosent;
	db = fopen("/home/sibalnirbhay/Desktop/DATABASE/userid.txt", "r");
	int found = 0;
	char tor[20], tmp[20], msgr[1024];
	char stra[100] = "\n\nFrom:\n";
	char strb[100] = "\n\nTo:\n";
	char strc[100] = "\n\n+++++-----+++++-----+++++-----+++++-----+++++-----+++++\n\n";
	char tos[30] = "\n\nCompose new mail\n\nTo:\n";
	char msgs[20] = "\n\nMessage:\n";
	char canc[30] = "\n\nCanceled\n";
	char sent[50] = "/home/sibalnirbhay/Desktop/DATABASE/";
	char inbox[50] = "/home/sibalnirbhay/Desktop/DATABASE/";
	char sucss[30] = "\n\nMail sent\n";
	char failed[30] = "\n\nMail sending failed\n";
	bzero(tor, 20);
	bzero(tmp, 20);
	bzero(msgr, 1024);
	write(csfd, &tos, 30);
	read(csfd, &tor, 20);
	write(csfd, &msgs, 20);
	read(csfd, &msgr, 1024);
	if(strcmp(msgr, "cancel") == 0)
	{
		write(csfd, &canc, 30);
		fclose(db);
		return;
	}
	while(!feof(db))
	{
		fscanf(db, "%s", tmp);
		if(strcmp(tor, tmp) == 0)
		{
			found = 1;
			break;
		}
	}
	if(found == 1)
	{
		strcat(inbox, tor);
		strcat(inbox, "/inbox.txt");
		toinbox = fopen(inbox, "a+");
		strcat(stra, usrnm);
		strcat(stra, "\n\nMessage:\n");
		fwrite(&stra, strlen(stra), 1, toinbox);
		fwrite(&msgr, strlen(msgr), 1, toinbox);
		fwrite(&strc, strlen(strc), 1, toinbox);
		fclose(toinbox);
		strcat(sent, usrnm);
		strcat(sent, "/sentitems.txt");
		tosent = fopen(sent, "a+");
		strcat(strb, tor);
		strcat(strb, "\n\nMessage:\n");
		fwrite(&strb, strlen(strb), 1, tosent);
		fwrite(&msgr, strlen(msgr), 1, tosent);
		fwrite(&strc, strlen(strc), 1, tosent);
		fclose(tosent);
		write(csfd, &sucss, 30);
	}
	else
		write(csfd, &failed, 30);
	fclose(db);
}

void inbox(int csfd, char usrnm[20])
{
	FILE *inbox;
	int i = -1;
	int a;
	char str[1024];
	char loc[100] = "/home/sibalnirbhay/Desktop/DATABASE/";
	strcat(loc, usrnm);
	strcat(loc, "/inbox.txt");
	inbox = fopen(loc, "r+");
	fseek(inbox, 1, SEEK_END);
	a = ftell(inbox);
	if(a < 2)
	{
		strcpy(str, "\n\nInbox Empty\n\n");
	}
	else
	{
		rewind(inbox);
		while(!feof(inbox))
			str[++i] = fgetc(inbox);
		str[i] = '\0';
	}
	write(csfd, &str, 1024);
	fclose(inbox);
}

void sentitems(int csfd, char usrnm[20])
{
	FILE *sentitems;
	int i = -1;
	int a;
	char str[1024];
	char loc[100] = "/home/sibalnirbhay/Desktop/DATABASE/";
	strcat(loc, usrnm);
	strcat(loc, "/sentitems.txt");
	sentitems = fopen(loc, "r+");
	fseek(sentitems, 1, SEEK_END);
	a = ftell(sentitems);
	if(a < 2)
	{
		strcpy(str, "\n\nEmpty Sent Items\n\n");
	}
	else
	{
		rewind(sentitems);
		while(!feof(sentitems))
			str[++i] = fgetc(sentitems);
		str[i] = '\0';
	}
	write(csfd, &str, 1024);
	fclose(sentitems);
}
