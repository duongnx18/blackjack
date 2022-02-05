#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include "protocol.h"
#include "card.h"

char Player[4][30];
Card deck[5][10];
int Point[5] = {0, 0, 0, 0, 0};
int ncard[5] = {0, 0, 0, 0, 0};
int slot = 0;
void menu()
{
	printf("***************************************\n");
	printf("MENU:\n");
	printf("1. Create room\n");
	printf("2. Join room\n");
	printf("3. See score\n");
	printf("4. Log out\n");
	printf("***************************************\n");
	printf("Enter your choice:  ");
}

void SignIU()
{
	printf("***************************************\n");
	printf("Welcome to the world of Blackjack\n");
	printf("1. Sign in\n");
	printf("2. Sign up\n");
	printf("3. Exit\n");
	printf("***************************************\n");
	printf("Enter your choice: \n");
}


void startProcess(char *msg){
	int i = 0, id = 0, card[20] = {0};
	while(msg[i+2]!= ' '){
		id *= 10;
		id += msg[i+2] - '0';
		i++;
	}
	i++;
	slot += msg[i+2] - '0';
	i += 2; 
	for(int n = 0; n < slot + 1; n++){
		while(msg[i+2]!= ' '){
			Point[n] *= 10;
			Point[n] += msg[i+2] - '0';
			i++;
		}
		i++;
	}
	for (int j = 0; j < 2*slot + 2; ++j)
	{
		while(msg[i+2]!= ' ' && msg[i+2]!= '\0'){
			card[j] *= 10;
			card[j] += msg[i+2] - '0';
			i++;
		}
		//printf("%d\n", card[j]);
		i++;
	}
	
	Card start[20];
	for(int k =0; k< 2*slot + 2; k++){
		start[k] = newcard(card[k]);
	}
	printf("Dealer Hand:\n");
	deck[0][0] = start[slot];
	deck[0][1] = start[2*slot+1];
	ncard[0] = 2;
	display_state(deck[0], 1);
	if (Point[i+1] == 21){
			printf("Blackjack\n");
	}
	// printf("------------------\n");
	// printf("|%s\t| |%s\t|\n",start[slot].showvalue,start[2*slot+1].showvalue);
	// printf("|%s\t| |%s\t|\n",start[slot].showtype,start[2*slot+1].showtype);
	// printf("------------------\n");
	//printf("Your Hand:\n");
	for (int i = 0; i < slot; ++i)
	{
		printf("Player %d: %s's hand\n", i+1, Player[i]);
		deck[i+1][0] = start[i];
		deck[i+1][1] = start[i+slot+1];
		ncard[i+1] = 2;
		display_state(deck[i+1], ncard[i+1]);
		// printf("------------------\n");
		// printf("|%s\t| |%s\t|\n",start[i].showvalue,start[i+slot+1].showvalue);
		// printf("|%s\t| |%s\t|\n",start[i].showtype,start[i+slot+1].showtype);
		// printf("------------------\n");
		printf("Sum: %d\n", Point[i+1]);
		if (Point[i+1] == 21){
			printf("Blackjack\n");
			if (Point[0] < 21){
				printf("Win\n");
			}else{
				printf("Push\n");
			}
		}
	}
	printf("Wait for your turn\n");


}
int playProcess(char *msg, int turn){
	int option;
	int i = 0, turner = 0;
	if(msg[i] == '0' + HIT_RES){
		int card = 0, id = 0;
		while(msg[i+2]!= ' '){
			id *= 10;
			id += msg[i+2] - '0';
			i++;
		}
		i++;
		turner += msg[i+2] - '0';
		i += 2; 
		Point[turner+1] = 0;
		while(msg[i+2]!= ' '){
			Point[turner+1] *= 10;
			Point[turner+1] += msg[i+2] - '0';
			i++;
		}
		i++;
		while(msg[i+2]!= ' ' && msg[i+2]!= '\0'){
			card *= 10;
			card += msg[i+2] - '0';
			i++;
		}
		Card new = newcard(card);
		deck[turner+1][ncard[turner+1]++] = new;
	}
	if(msg[i] == '0' + DEA_RES){
		int card[10] = {0}, id = 0;
		while(msg[i+2]!= ' '){
			id *= 10;
			id += msg[i+2] - '0';
			i++;
		}
		i++; 
		Point[0] = 0;
		while(msg[i+2]!= ' ' && msg[i+2]!= '\0'){
			Point[0] *= 10;
			Point[0] += msg[i+2] - '0';
			i++;
		}
		if (msg[i+2]!= '\0'){
			i++;
		}
		int j = 0;
		while(msg[i+2]!= '\0')
		{		
			while(msg[i+2]!= ' ' && msg[i+2]!= '\0'){
				card[j] *= 10;
				card[j] += msg[i+2] - '0';
				i++;
			}
			
			Card new = newcard(card[j]);
			deck[0][ncard[0]++] = new;
			j++;
			i++;
		}

	}
	printf("Dealer Hand:\n");
	if(msg[0] == '0' + DEA_RES){
		display_state(deck[0], ncard[0]);
		printf("Sum: %d\n", Point[0]);
		if(Point[0] > 21)
		{
			printf("Dealer Busted\n");
		}
		if(Point[0] == 21 && ncard[0] == 2){
			printf("Dealer Blackjack\n");
		}
	}
	else{
		display_state(deck[0], 1);
	}
	// printf("------------------\n");
	// printf("|%s\t| |%s\t|\n",start[slot].showvalue,start[2*slot+1].showvalue);
	// printf("|%s\t| |%s\t|\n",start[slot].showtype,start[2*slot+1].showtype);
	// printf("------------------\n");
	//printf("Your Hand:\n");
	for (int i = 0; i < slot; ++i)
	{
		printf("Player %d: %s's hand\n", i+1, Player[i]);

		display_state(deck[i+1], ncard[i+1]);
		// printf("------------------\n");
		// printf("|%s\t| |%s\t|\n",start[i].showvalue,start[i+slot+1].showvalue);
		// printf("|%s\t| |%s\t|\n",start[i].showtype,start[i+slot+1].showtype);
		// printf("------------------\n");
		printf("Sum: %d\n", Point[i+1]);

		if(Point[i+1] > 21)
		{
			printf("Busted\n");
			printf("Lose\n");
		}
		if(msg[0] == '0' + DEA_RES){
			if (Point[i+1] < 21 && Point[i+1] > Point[0]){
				printf("Win\n");
			}
			if (Point[i+1] < 21 && Point[0] > 21){
				printf("Win\n");
			}
			if (Point[i+1] < 21 && Point[i+1] == Point[0]){
				printf("Push\n");
			}
			if (Point[i+1] < 21 && Point[i+1] < Point[0] && Point[0] <= 21){
				printf("Lose\n");
			}
			if(Point[i+1] == 21 && ncard[i+1] > 2){
				if(Point[i+1] > Point[0] || Point[0] > 21){
					printf("Win\n");
				}

				if (Point[i+1] == Point[0] && ncard[0] > 2){
					printf("Push\n");
				} else if (Point[i+1] == Point[0] && ncard[0] == 2){
					printf("Lose\n");
				}
			}

		}
		if(Point[i+1] == 21 && ncard[i+1] == 2){
			printf("Blackjack\n");
			if (Point[0] < 21){
				printf("Win\n");
			}else if (Point[0] == 21 && ncard[0] == 2){
				printf("Push\n");
			}
		}
	}
	if(msg[0] == '0' + DEA_RES){
		return 0;
	}
	if(turn == turner || msg[i] == '0' + TURN_RES){
		if (Point[turn+1] >= 21){
			return 2;
		}
		printf("1.Hit\n");
		printf("2.Stand\n");
		printf("Enter your option:");
		scanf("%d", &option);
		return option;
	}
	else {
		printf("Wait\n");
		return 0;
	}
}

void waitProcess(char *msg){
	//printf("%s\n", msg);
	char *nickname = (char*)calloc(20,sizeof(char));
	int i = 4, j, k = 1;
	while(msg[i]!=' ') i++;
	i++;
	while(msg[i]!='\0'){
		j = i;
		while(msg[i]!=' ' && msg[i]!='\0')
		{
			nickname[i-j] = msg[i];
			i++;
		}
		nickname[i-j] = '\0';
		i++;
		printf("Player %d: %s\n", k, nickname);
		strcpy(Player[k-1], nickname);
		k++;
	}
}
int betProcess(char *msg, int *turn){
	char *nickname = (char*)calloc(20,sizeof(char));
	int i = 0;
	while(msg[i+2]!=' ')
	{
		nickname[i] = msg[i+2];
		i++;
	}
	nickname[i++] = '\0';
	int id = 0, bet, point= 0;
	while(msg[i+2]!= ' '){
		point *= 10;
		point += msg[i+2] - '0';
		i++;
	}
	i++;
	while(msg[i+2]!= ' '){
		id *= 10;
		id += msg[i+2] - '0';
		i++;
	}
	i++;
	*turn = msg[i+2] - '0';
	//printf("%d\n", *turn);
	printf("Player: %s\n", nickname);
	printf("Point: %d\n", point);
	while(1){
		printf("Enter score you want to bet:");
		scanf("%d", &bet);
		if(bet <= point) break;
		else 
			printf("You just have %d point. Bet again!\n", point);
	}

	return bet;
}
char *getNickName(char *msg)
{
	char *nickname = (char*)calloc(20,sizeof(char));
	int i = 0;
	while(msg[i+4]!='\0')
	{
		nickname[i] = msg[i+4];
		i++;
	}
	nickname[i] = '\0';
	return nickname;
}
void showListRoom(char *msg)
{
	int i = 0,j,lenght = strlen(msg);
	int id = 0,status = 0;
	char *temp = (char*)calloc(5,sizeof(char));
	printf("ID\tStatus\n");
	while(i<lenght)
	{
		j = 0;
		id = 0;
		while(msg[i]!='|'&&i<lenght)
		{
			temp[j++] = msg[i++];
		}
		j = 0;
		while(temp[j]!='-')
		{
			id*=10;
			id+=temp[j]-'0';
			j++;
		}
		j++;
		status = temp[j] - '0';
		if(id<10)
			printf(" %d\t%d\n",id,status);
		else printf("%d\t%d\n",id,status);
		i++;
	}
	free(temp);
}
char* showIDCreatedRoom(char *msg, char id[])
{
	int i = 4;
	printf("Room ID is ");
	while(msg[i] != ' ' && msg[i] != '\0')
	{
		printf("%c", msg[i]);
		id[i-4] = msg[i];
		i++;
	}
	printf("\n");
	id[i-4] = '\0';
	return id;
}

int getScore(char *msg)
{
	int i = 2;
	int score = 0;
	while(i < strlen(msg))
	{
		score*=10;
		score+=msg[i] - '0';
		i++;
	}
	return score;
}


int main()
{
 	int sockfd = socket(AF_INET,SOCK_STREAM,0);	
	assert(sockfd != -1 );
	char *nickname = (char*)calloc(20,sizeof(char));
	char *msg = (char*)calloc(MSG_SIZE,sizeof(char));
	int choice,isLoged_in = 0;
	int rcvsize;
	int plus;
	char control[2];
	//Set Address Information
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(6666);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//Link to server
	int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res != -1);
	while(isLoged_in == 0)
	{
		SignIU();
		scanf("%d",&choice);
		while(choice<1||choice>3)
		{
			printf("Invalid selection. Re-enter:  ");
			scanf("%d",&choice);
		}
		getchar();
		switch(choice)
		{
			case 1:
			{
				strcpy(msg,makeSignInMessage());
				send(sockfd,msg,strlen(msg),0);
				rcvsize = recv(sockfd,msg,MSG_SIZE,0);
				msg[rcvsize] = '\0';
				if(msg[2] == '0' + SUCCESS_SIGNIN)
				{
					strcpy(nickname,getNickName(msg));
					isLoged_in = 1;
				}
				else if(msg[2] == '0' + NOT_EXIST)
				{
					printf("Account does not exist\n");
				}
				else if(msg[2] == '0' + WRONG_PASS)
				{
					printf("Wrong password\n");
				}
				else if(msg[2] == '0' + LOGED_IN)
				{
					printf("This account is logged in somewhere else\n");
				}
				break;
			}
			case 2:
			{
				strcpy(msg,makeSignUpMessage());
				send(sockfd,msg,strlen(msg),0);
				rcvsize = recv(sockfd,msg,MSG_SIZE,0);
				msg[rcvsize] = '\0';
				if(msg[2] == '0' + SUCCESS_SIGNUP)
				{
					strcpy(nickname,getNickName(msg));
					isLoged_in = 1;
				}
				else if(msg[2] == '0' + EXISTED)
				{
					printf("Already have this account\n");
				}
				break;
			}
			case 3:
			{
				return 0;
			}
		}
	}
	system("clear");
	printf("Login successfully. Welcome %s\n",nickname );
	while(isLoged_in == 1)
	{
		char Player[4][30] ;
		for (int i = 0; i < 4; ++i)
		{
			bzero(Player[i], 30);
		}
		for (int i = 0; i < 5; ++i)
		{
			Point[i] = 0;
			ncard[i] = 0;
		}
		
		slot = 0;
		menu();
		scanf("%d",&choice);
		while(choice<1||choice>4)
		{
			printf("Invalid selection. Re-enter:  ");
			scanf("%d",&choice);
		}
		switch(choice)
		{
			case 1:
			{
				system("clear");
				strcpy(msg,makeCreateRoomMessage(nickname));
				send(sockfd,msg,strlen(msg),0);
				rcvsize = recv(sockfd,msg,MSG_SIZE,0);
				msg[rcvsize] = '\0';
				char id[3];
				int turn;
				system("clear");
				strcpy(id, showIDCreatedRoom(msg,id));
				while(1){
					
					printf("Do you want to start game right now(y/n)?:");
					scanf("%s", control);
					if(strcmp(control,"y") == 0) break;
					rcvsize = recv(sockfd,msg,MSG_SIZE,0);
					msg[rcvsize] = '\0';
					system("clear");
					waitProcess(msg);

				}

				strcpy(msg,startGameMessage(id));
				send(sockfd,msg,strlen(msg),0);
				rcvsize = recv(sockfd,msg,MSG_SIZE,0);
			    msg[rcvsize] = '\0';
			    system("clear");
			    int bet = betProcess(msg, &turn);
			    strcpy(msg,makeBetMessage(bet, atoi(id), turn));
			    // printf("%d", turn);
			    send(sockfd,msg,strlen(msg),0);
			    rcvsize = recv(sockfd,msg,MSG_SIZE,0);
			    msg[rcvsize] = '\0';

			    //printf("%s\n", msg);
			    //playProcess(msg);
			    startProcess(msg);
			    
			    rcvsize = recv(sockfd,msg,MSG_SIZE,MSG_DONTWAIT);
			    msg[rcvsize] = '\0';
			    while(1){
			    	system("clear");
			    	int choice = playProcess(msg,turn);
			    	if (choice != 0)
				    {
				    	strcpy(msg,makePlayMessage(choice, atoi(id), turn));
				    	send(sockfd,msg,strlen(msg),0);
				    }
				    if(msg[0] == '0' + DEA_RES) break;
				    rcvsize = recv(sockfd,msg,MSG_SIZE,0);
				    msg[rcvsize] = '\0';
				}

				// plus = firstplay(opponent,sockfd);
				// printf("Please wait 8 seconds to return to the main screen\n");	
				// sleep(8);
				// system("clear");
				// printf("You get %d points added to your account\n",plus);
				// strcpy(msg,makePlusScoreMessage(nickname,plus));
				// send(sockfd,msg,strlen(msg),0);
				break;
			}
			case 2:
			{
				system("clear");
				strcpy(msg,getAvailableRoomMessage());
				send(sockfd,msg,strlen(msg),0);
				rcvsize = recv(sockfd,msg,MSG_SIZE,0);
				msg[rcvsize] = '\0';
				showListRoom(msg);
				strcpy(msg,makeJoinRoomMessage(nickname));
				if(strlen(msg)==0)
					break;
				send(sockfd,msg,strlen(msg),0);
				rcvsize = recv(sockfd,msg,MSG_SIZE,0);
				msg[rcvsize] = '\0';
				if(msg[2]=='0' + JOIN_SUCCESS)
				{
					printf("Successfully join.\n");
					char id[3];
					int turn;
					strcpy(id, showIDCreatedRoom(msg,id));
					while(1){
						system("clear");
						waitProcess(msg);
						rcvsize = recv(sockfd,msg,MSG_SIZE,0);
						msg[rcvsize] = '\0';
						if(msg[0] =='0' + STA_RES) break;
					}
					int bet = betProcess(msg, &turn);
			    	strcpy(msg,makeBetMessage(bet, atoi(id), turn));
			    	send(sockfd,msg,strlen(msg),0);
			    	rcvsize = recv(sockfd,msg,MSG_SIZE,0);
				    msg[rcvsize] = '\0';
			    	startProcess(msg);
			    	rcvsize = recv(sockfd,msg,MSG_SIZE,MSG_DONTWAIT);
				    msg[rcvsize] = '\0';
				    while(1){
				    	system("clear");
				    	int choice = playProcess(msg,turn);
					    if (choice != 0)
					    {
					    	strcpy(msg,makePlayMessage(choice, atoi(id), turn));
					    	send(sockfd,msg,strlen(msg),0);
					    }
					    if(msg[0] == '0' + DEA_RES) break;
					    rcvsize = recv(sockfd,msg,MSG_SIZE,0);
					    msg[rcvsize] = '\0';
					}
				    
				// 	plus = secondplay(opponent,sockfd);
				// 	printf("Please wait 12 seconds to return to the main screen\n");	
				// 	sleep(12);
				// 	system("clear");
				// 	printf("You get %d points added to your account\n",plus);
				// 	strcpy(msg,makePlusScoreMessage(nickname,plus));
				// 	send(sockfd,msg,strlen(msg),0);
				}
				else
				{
					if(msg[2]=='0' + FULL_SLOT)
						printf("Room is full\n");
					if(msg[2]=='0' + WRONG_RPASS)
						printf("Wrong password\n");
					if(msg[2]=='0' + ROOM_NEXIST)
						printf("Room does not exist\n");
					system("clear");
				}
				
				break;
			}
			case 3:
			{
				strcpy(msg,getScoreMessage(nickname));
				send(sockfd,msg,strlen(msg),0);
				rcvsize = recv(sockfd,msg,MSG_SIZE,0);
				plus = getScore(msg);
				printf("Your score is %d\n",plus);
				break;
			}
			case 4:
			{
				printf("Goodbye %s\n",nickname);
				isLoged_in = 0;
				break;
			}
		}
	}
	close(sockfd);
}