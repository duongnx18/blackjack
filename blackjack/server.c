#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/time.h>
#include "user.h"
#include "protocol.h"
#include "room.h"
#include "card.h"
#define MAXFD 10	//Size of fds array
#define PORT 6666	
User* headUser = NULL;
Room *headRoom = NULL;
typedef struct{
	int i1;
	int i2;
	int i3;
	int i4;
}connectt;
connectt Link[20];
void LoadUser()
{
	FILE *file = fopen("user.txt","r");
	char username[30],password[20],nickname[20];
	int score;
	while(!feof(file))
	{
		fscanf(file,"%s %s %s %d\n",username,password,nickname,&score);
		pushUser(&headUser,username,password,nickname,score);
	}
	fclose(file);
}

void updateUser()
{
	FILE* file = fopen("user.txt","w");
	User *temp = headUser;
	while(temp->next!=NULL)
	{
		fprintf(file,"%s %s %s %d\n",temp->username,temp->password,temp->nickname,temp->score);
		temp=temp->next;
	}
	fprintf(file,"%s %s %s %d",temp->username,temp->password,temp->nickname,temp->score);
	fclose(file);
}

void fds_add(int fds[],int fd)
{
	int i=0;
	for(;i<MAXFD;++i)
	{
		if(fds[i]==-1)
		{
	      fds[i]=fd;
		  break;
		}
	}
}
//0username password nickname
int processSignUp(char *msg,char *nick)
{
	char *username = (char*)calloc(30,sizeof(char));
	char *password = (char*)calloc(20,sizeof(char));
	char *nickname = (char*)calloc(20,sizeof(char));
	int i = 1,j = 0;
	while(msg[i]!=' ')
	{
		username[j] = msg[i];
		j++;
		i++;
	}
	username[j] = '\0';
	if(getUserByUserName(headUser,username)!=NULL)
	{
		strcpy(nick,"\0");
		return 0;
	}
	j = 0;i++;
	while(msg[i]!=' ')
	{
		password[j] = msg[i];
		j++;
		i++;
	}
	password[j] = '\0';
	j = 0;i++;
	while(msg[i]!='\0')
	{
		nickname[j] = msg[i];
		j++;
		i++;
	}
	nickname[j] = '\0';
	strcpy(nick,nickname);
	pushUser(&headUser,username,password,nickname,2000);
	User * user = getUserByUserName(headUser,username);
	user->isLogin = 1;
	return 1;
}

int processSignIn(char *msg,char *nick)
{
	char *username = (char*)calloc(30,sizeof(char));
	char *password = (char*)calloc(20,sizeof(char));
	User *loginUser = NULL;
	int i = 1,j = 0;
	while(msg[i]!=' ')
	{
		username[j] = msg[i];
		j++;
		i++;
	}
	username[j] = '\0';
	loginUser = getUserByUserName(headUser,username);
	if(loginUser==NULL)
	{
		strcpy(nick,"\0");
		return 0;
	}
	j = 0;i++;
	while(msg[i]!='\0')
	{
		password[j] = msg[i];
		j++;
		i++;
	}
	password[j] = '\0';
	j = 0;i++;
	if(strcmp(loginUser->password,password)!=0)
	{
		strcpy(nick,"\0");
		return 1;
	}
	if(loginUser->isLogin == 1)
	{
		strcpy(nick,"\0");
		return -1;
	}
	strcpy(nick,loginUser->nickname);
	loginUser->isLogin = 1;
	return 2;
}
char *signIURes(RES_TYPE type,VALUE_RES res,char *nickname)
{
	char *msg = (char*)calloc(25,sizeof(char));
	int i = 4;
	msg[0] = '0' + type;
	msg[1] = ' ';
	msg[2] = '0' + res;
	msg[3] = ' ';
	while(i-4 < strlen(nickname))
	{
		msg[i] = nickname[i-4];
		i++;
	}
	msg[i] = '\0';
	printf("%s\n",msg);
	return msg;
}
char *listRoomRes()
{
	int temp,i = 0;
	char *str = (char*)calloc(20,sizeof(char));
	Room *r = headRoom;
	printf("%d\n", r->id);
	if (r->id == 0) return str;
	while(r!=NULL)
	{
		if(r->canPlay==0)
		{
			temp = r->id;
			while(temp!=0)
			{
				str[i++] = '0' + temp%10;
				temp/=10;
			}
			str[i++] = '-';
			str[i++] = '0' + r->status;
			str[i++] = '|';
		}
		r = r->next;
	}
	str[i-1] = '\0';
	printf("%s\n",str);
	return str;
}
char *createRoomRes(int ruler)
{
	int i = 4,id = ruler;
	char *msg = (char*)calloc(6,sizeof(char));
	msg[0] = '0' + CRE_RES;
	msg[1] = ' ';
	msg[2] = '0' + CRE_SUCCESS;
	msg[3] = ' ';
	while(id!=0)
	{
		msg[i++] = '0'+id%10;
		id/=10;
	}
	msg[i] = '\0';
	printf("%s\n",msg);
	return msg;
}



int* processStand(char *msg, int turner[]){
	int i = 2, turn = 0, id=0;
	while(msg[i]!=' '){
		id *=10;
		id+=msg[i]-'0';
		i++;
	}
	i++;
	turn += msg[i]-'0';
	turner[0] = id;
	Room *r = getRoombyID(headRoom,id);
	if(msg[0] == '0' + SURRENDER){
		r->bet[turn] -= 3*r->bet[turn]/2;
	}
	if (turn < r->slot -1){
		turner[1] = turn+1;
	}else turner[1] = 0;

	return turner;
}
int* processHit(char *msg, int card[]){
	int i = 2, turn = 0, id=0;
	while(msg[i]!=' '){
		id *=10;
		id+=msg[i]-'0';
		i++;
	}
	i++;
	turn += msg[i]-'0';
	Room *r = getRoombyID(headRoom,id);
	if(msg[0] == '0' + DOUBLE){
		r->bet[turn] *= 2;
	}
	r->player[turn].hand[r->player[turn].ncard++] = newcard(r->deck[r->ncard++]);
	card[0] = id;
	card[1] = turn;
	card[2] = calc_sum(r->player[turn].hand, r->player[turn].ncard);
	card[3] = r->deck[r->ncard-1];
	return card;
}
int* processDealer(int id, int card[]){

	Room *r = getRoombyID(headRoom,id);

	card[0] = id;
	int i = 0;
	while((card[1] = calc_sum(r->dealerHand, r->ncardealer)) < 17){
	r->dealerHand[r->ncardealer++] = newcard(r->deck[r->ncard++]);
	card[i+2] = r->deck[r->ncard-1];
	i++;
	}
	return card;
}
void processInsurance(char *msg, int room[]){
	int i = 2, turn, insurance = 0, id=0;
	while(msg[i]!=' '&&i<strlen(msg))
	{
		insurance *=10;
		insurance +=msg[i]-'0';
		i++;
	}
	i++;
	while(msg[i]!=' '){
		id *=10;
		id+=msg[i]-'0';
		i++;
	}
	i++;
	turn = msg[i]-'0';
	Room *r = getRoombyID(headRoom,id);
	User *user = getUserByNickName(headUser,r->player[turn].nickname);
	room[0] = id;
	room[1] = r->slot;
	printf("%d\n", user->score);
	printf("%d\n", r->bet[turn]);
	if(insurance == 3){
		user->score -= r->bet[turn]/2;
	}
	if(insurance == 2){
		user->score -= r->bet[turn];
	}
	printf("%d\n", user->score);
	updateUser();
	if(insurance <= 2){
		room[turn+2] = 1;
	}
}
int processBet(char *msg, int card[]){
	int i = 2, turn, point = 0, id=0;
	while(msg[i]!=' '&&i<strlen(msg))
	{
		point *=10;
		point +=msg[i]-'0';
		i++;
	}
	i++;
	while(msg[i]!=' '){
		id *=10;
		id+=msg[i]-'0';
		i++;
	}
	i++;
	turn = msg[i]-'0';
	Room *r = getRoombyID(headRoom,id);
	
	r->bet[turn] = point;
	//printf("%d\n", point);
	//printf("%d\n", turn);
	//printf("%d\n", r->bet[turn]);
	int j = 0;
	if (r->ncard == 0){	
		for (; j < r->slot ; ++j)
		{
			r->player[j].hand[0] = newcard(r->deck[j]);
			r->player[j].hand[1] = newcard(r->deck[j + 1 + r->slot]);
			r->player[j].ncard = 2;
			r->ncard += 2;
		}
		//r->deck[j] = 0;
		r->dealerHand[0] = newcard(r->deck[j]);
		r->dealerHand[1] = newcard(r->deck[j + 1 + r->slot]);
		r->ncardealer = 2;
		r->ncard += 2;
	}

	card[0] = id;
	card[1] = r->slot;
	int k = 0;
	card[2] = calc_sum(r->dealerHand, 2);
	
	for (; k < r->slot; ++k)
	{
		card[k+3] = calc_sum(r->player[k].hand, 2);
	}
	//printf("%d\n", k);
	for (int i = 0 ; i < r->ncard; ++i)
	{
		card[k+3] = r->deck[i];
		k++;
	}
	int count = 0;
	for (int i = 0; i < 4; ++i)
	{
		if(r->bet[i] > 0){
			count++;
		}
	}
	return count;
}
int processCreateRoom(char *msg)
{
	int j,i = 4,status = msg[2]-'0';
	int id;
	char *nickname = (char*)calloc(20,sizeof(char));
	char *password = (char*)calloc(20,sizeof(char));
	while(msg[i]!=' '&&i < strlen(msg))
	{
		nickname[i-4] = msg[i];
		i++;
	}
	nickname[i-4] = '\0';
	Player p = newPlayer(nickname,100);
	if(status == 1)
	{
		i++;
		j = i;
		while(i < strlen(msg))
		{
			password[i-j] = msg[i];
			i++;
		}
		password[i-j] = '\0';
		id = pushRoom(&headRoom,1,password,p);
		return id;
	}
	strcpy(password,"\0");
	id = pushRoom(&headRoom,0,password,p);
	return id;
}
void processScore(int id)
{

	Room *r = getRoombyID(headRoom,id);
	int dealerPoint = calc_sum(r->dealerHand, r->ncardealer);
	for (int i = 0; strlen(r->player[i].nickname)>0; ++i)
	{
		int playerPoint = calc_sum(r->player[i].hand, r->player[i].ncard);
		User *user = getUserByNickName(headUser,r->player[i].nickname);
		printf("%d\n", user->score);
		printf("%d\n", r->bet[i]);
		if (r->bet[i] < 0){
			user->score += r->bet[i];
		}
		else if (playerPoint > 21){
			user->score -= r->bet[i];
		}
		else if (playerPoint == 21 && r->player[i].ncard == 2){
			if(dealerPoint != 21 || r->ncardealer > 2)
			{
				user->score += 3*r->bet[i]/2;
			}
		}
		else if (playerPoint == 21 && r->player[i].ncard > 2){
			if (playerPoint > dealerPoint || dealerPoint > 21){
				user->score += r->bet[i];
			}
			else if (playerPoint == dealerPoint && r->ncardealer == 2){
				user->score -= r->bet[i];
			}
		}
		else if (playerPoint < 21){
			if (playerPoint > dealerPoint || dealerPoint > 21){
				user->score += r->bet[i];
			}
			else if (playerPoint < dealerPoint && dealerPoint <= 21){
				user->score -= r->bet[i];
			}
		}
		printf("%d\n", user->score);
	}
	updateUser();
	removeRoom(&headRoom,id);
	//printf("%d\n", headRoom->id);
}
int processGetScore(char *msg)
{
	int i = 2;
	char *nickname = (char*)calloc(20,sizeof(char));
	while(i < strlen(msg))
	{
		nickname[i-2] = msg[i];
		i++;
	}
	nickname[i-2] = '\0';
	User *user = getUserByNickName(headUser,nickname);
	return user->score;
}
char *scoreMessage(int score)
{
	int i = 2;
	char *str = (char*)calloc(10,sizeof(char));
	char *temp = (char*)calloc(4,sizeof(char));
	sprintf(temp,"%d",score);
	str[0] = '0' + SCORE_RES;
	str[1] = ' ';
	while(i-2 <strlen(temp))
	{
		str[i] = temp[i-2];
		i++;
	}
	str[i] = '\0';
	printf("%s\n",str);
	return str;
}
int startGameProcess(char *msg){
	int i = 2, id=0;
	while(msg[i]!=' '&&i<strlen(msg))
	{
		id*=10;
		id+=msg[i]-'0';
		i++;
	}
	Room *r = getRoombyID(headRoom,id);
	setDeckToRoom(r);
	r->canPlay = 1;
	return id;
}

int processJoinRoom(char *msg)
{
	int id=0,i = 2,j;
	char *nickname = (char*)calloc(20,sizeof(char));
	char *password = (char*)calloc(20,sizeof(char));
	while(msg[i]!=' ')
	{
		nickname[i-2] = msg[i];
		i++;
	}
	nickname[i] = '\0';
	i++;
	while(msg[i]!=' '&&i<strlen(msg))
	{
		id*=10;
		id+=msg[i]-'0';
		i++;
	}
	Room *r = getRoombyID(headRoom,id);
	if(r==NULL)
	{
		return 0;
	}
	if(r->canPlay==1)
	{
		return -1;
	}
	Player p = newPlayer(nickname,100);
	if(r->status==1)
	{
		i++;
		j = i;
		while(i < strlen(msg))
		{
			password[i-j] = msg[i];
			i++;
		}
		password[i-j] = '\0';
		if(strcmp(r->password,password)==0)
		{
			joinRoom(r,p);
			//setDeckToRoom(r);
			return id;
		}
		else
		{
			return -2;
		}
	}
	joinRoom(r,p);
	//setDeckToRoom(r);
	return id;
}
char *betRes(int card[]){
	char *str = (char*)calloc(48,sizeof(char));
	char *temp = (char*)calloc(20,sizeof(char));
	str[0] = '0' + BET_RES;
	str[1] = ' ';
	for (int i = 0; card[i] != -1; ++i)
	{
		sprintf(temp,"%d ", card[i]);
		strcat(str, temp);
	}
	printf("%s\n",str);
	return str;
}
char *dealerRes(int card[]){
	char *str = (char*)calloc(48,sizeof(char));
	char *temp = (char*)calloc(20,sizeof(char));
	str[0] = '0' + DEA_RES;
	str[1] = ' ';
	for (int i = 0; card[i] != -1; ++i)
	{
		sprintf(temp,"%d ", card[i]);
		strcat(str, temp);
	}
	printf("%s\n",str);
	return str;
}
char *turnRes(){
	char *str = (char*)calloc(4,sizeof(char));
	str[0] = '0' + TURN_RES;
	str[1] = '\0';
	printf("%s\n", str);
	return str;
}
char *hitRes(int card[]){
	char *str = (char*)calloc(48,sizeof(char));
	char *temp = (char*)calloc(20,sizeof(char));
	str[0] = '0' + HIT_RES;
	str[1] = ' ';
	sprintf(temp,"%d %d %d %d",card[0], card[1], card[2], card[3]);
	strcat(str, temp);
	printf("%s\n",str);
	return str;
}
char *doubleRes(int card[]){
	char *str = (char*)calloc(48,sizeof(char));
	char *temp = (char*)calloc(20,sizeof(char));
	str[0] = '0' + DOU_RES;
	str[1] = ' ';
	sprintf(temp,"%d %d %d %d",card[0], card[1], card[2], card[3]);
	strcat(str, temp);
	printf("%s\n",str);
	return str;
}
char *startGameRes(int id, int turn)
{
	int i = 2,p;
	char *str = (char*)calloc(48,sizeof(char));
	char *temp = (char*)calloc(10,sizeof(char));
	Room *r = getRoombyID(headRoom,id);
	str[0] = '0' + STA_RES;
	str[1] = ' ';
	while(i-2 < strlen(r->player[turn].nickname))
	{
		str[i] = r->player[turn].nickname[i-2];
		i++;
	}
	str[i] = '\0';
	p = processGetScore(str);
	str[i] = ' ';
	sprintf(temp,"%d",p);
	strcat(str, temp);
	sprintf(temp," %d",id);
	strcat(str, temp);
	sprintf(temp," %d",turn);
	strcat(str, temp);
	printf("%s\n",str);
	return str;
}
char *noticeRes(int id)
{
	int i = 4, j, p;
	char *str = (char*)calloc(48,sizeof(char));
	Room *r = getRoombyID(headRoom,id);
	str[0] = '0' + NOT_RES;
	str[1] = ' ';
	str[2] = '0' + NEW_PLAYER;
	str[3] = ' ';
	while(id!=0)
	{
		str[i++] = '0'+id%10;
		id/=10;
	}
	str[i++] = ' ';
	for(int k = 0; strlen(r->player[k].nickname) > 0; k++){
		//printf("%d\n", k);
		if (k == 4) break;
		j = i;
		while(i-j < strlen(r->player[k].nickname))
		{
			str[i] = r->player[k].nickname[i-j];
			i++;
		}
		str[i++] = ' ';
	}
	str[i-1] = '\0';
	printf("%s\n",str);
	return str;
}
char *joinRoomRes(VALUE_RES res,int id)
{
	int i = 4, j;
	char *str = (char*)calloc(50,sizeof(char));
	Room *r = getRoombyID(headRoom,id);
	str[0] = '0' + JOI_RES;
	str[1] = ' ';
	str[2] = '0' + res;
	if(res !=JOIN_SUCCESS)
	{
		str[3] = '\0';
		return str;
	}
	str[3] = ' ';
	while(id!=0)
	{
		str[i++] = '0'+id%10;
		id/=10;
	}
	str[i++] = ' ';
	for(int k = 0; strlen(r->player[k].nickname) > 0; k++){
		//printf("%d\n", k);
		if (k == 4) break;
		j = i;
		while(i - j < strlen(r->player[k].nickname))
		{
			str[i] = r->player[k].nickname[i-j];
			//printf("%c %d ", str[i], i);
			i++;

		}
		str[i++] = ' ';
	}
	str[i-1] = '\0';
	printf("%s\n",str);
	return str;
}

int main()
{
	memset(Link,0,sizeof(Link));
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd!=-1);
	int ruler;
	char *nickname = (char*)calloc(20,sizeof(char));
	headRoom = makeListRoom();    
	struct sockaddr_in saddr,caddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(PORT);
	saddr.sin_addr.s_addr=inet_addr("127.0.0.1");

	int res=bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res!=-1);
	
	//Create listening queue
	listen(sockfd,5);
   //Define fdset collection
    fd_set fdset;
	
	//Define fds array
    int fds[MAXFD];
    int i=0;
    for(;i<MAXFD;++i)
    {
	  	fds[i]=-1;
    }
	LoadUser();
	//Add a file descriptor to the fds array
    fds_add(fds,sockfd);
	while(1)
    {
		FD_ZERO(&fdset);//Clear the fdset array to 0

		int maxfd=-1;

		int i=0;

		//For loop finds the maximum subscript for the ready event in the fds array
		for(;i<MAXFD;i++)
		{
			if(fds[i]==-1)
			{
				continue;
			}

			FD_SET(fds[i],&fdset);

			if(fds[i]>maxfd)
			{
				maxfd=fds[i];
			}
		}

		struct timeval tv={120,0};	//Set timeout of 2 minute

		int n=select(maxfd+1,&fdset,NULL,NULL,&tv);//Selectect system call, where we only focus on read events
		if(n==-1)	//fail
		{
			perror("select error");
		}
		else if(n==0)//Timeout, meaning no file descriptor returned
		{
			printf("time out\n");
		}
		else//Ready event generation
		{
		//Because we only know the number of ready events by the return value of select, we don't know which events are ready.
		//Therefore, each file descriptor needs to be traversed for judgment
			for(i=0;i<MAXFD;++i)
			{
				if(fds[i]==-1)	//If fds[i]==-1, the event is not ready
				{
					continue;
				}
				if(FD_ISSET(fds[i],&fdset))	//Determine if the event corresponding to the file descriptor is ready
				{
			   
				//There are two kinds of cases for judging file descriptors
			   
					if(fds[i]==sockfd)	//A file descriptor is a socket, meaning accept if a new client requests a connection
					{
						//accept
						struct sockaddr_in caddr;
						int len=sizeof(caddr);

						int c=accept(sockfd,(struct sockaddr *)&caddr,&len);	//Accept new client connections
						printf("Connect to %s\n",inet_ntoa(caddr.sin_addr));
						if(c<0)
						{
							continue;
						}
					
						fds_add(fds,c);//Add the connection socket to the array where the file descriptor is stored
					}
					else   //Receive data recv when an existing client sends data
					{
						char buff[128]={0};
						int res=recv(fds[i],buff,127,0);
						if(res<=0)
						{
							close(fds[i]);
							fds[i]=-1;
							printf("one client over\n");
						}
						else
						{

							buff[res] = '\0';
							printf("%s %ld\n", buff, strlen(buff));
							switch(buff[0]-'0')
							{
								case REGISTER:
								{
									ruler = processSignUp(buff,nickname);
									if(ruler == 0)
									{
										send(fds[i],signIURes(REG_RES,EXISTED,nickname),25,0);
									}
									else {
										send(fds[i],signIURes(REG_RES,SUCCESS_SIGNUP,nickname),25,0);
										updateUser();
									}
									break;
								}
								case LOGIN:
								{
									ruler = processSignIn(buff,nickname);
									if(ruler == 0)
									{
										send(fds[i],signIURes(LOG_RES,NOT_EXIST,nickname),25,0);
									} else if(ruler == 1)
									{
										send(fds[i],signIURes(LOG_RES,WRONG_PASS,nickname),25,0);
									} else if(ruler == -1)
									{
										send(fds[i],signIURes(LOG_RES,LOGED_IN,nickname),25,0);
									}else{
										send(fds[i],signIURes(LOG_RES,SUCCESS_SIGNIN,nickname),25,0);
									} 
									break;
								}
								case CREATEROOM:
								{
									//printf("start");
									ruler = processCreateRoom(buff);
									send(fds[i],createRoomRes(ruler),6,0);
									Link[ruler].i1 = i;
									Link[ruler].i2 = -9;
									Link[ruler].i3 = -9;
									Link[ruler].i4 = -9;
									break;
								}
								case GETLIST:
								{
									send(fds[i],listRoomRes(),20,0);
									break;
								}
								case JOINROOM:
								{
									ruler = processJoinRoom(buff);
									if(ruler==0)
										send(fds[i],joinRoomRes(ROOM_NEXIST,ruler),6,0);
									else if(ruler==-1)
										send(fds[i],joinRoomRes(FULL_SLOT,ruler),6,0);
									else if(ruler==-2)
										send(fds[i],joinRoomRes(WRONG_RPASS,ruler),6,0);
									else{
										if(Link[ruler].i2 == -9){
											Link[ruler].i2 = i;
											send(fds[i],joinRoomRes(JOIN_SUCCESS,ruler),50,0);
											send(fds[Link[ruler].i1],noticeRes(ruler),48,0);
										}
										else if(Link[ruler].i3 == -9){
											Link[ruler].i3 = i;
											send(fds[i],joinRoomRes(JOIN_SUCCESS,ruler),50,0);
											send(fds[Link[ruler].i1],noticeRes(ruler),48,0);
											send(fds[Link[ruler].i2],noticeRes(ruler),48,0);
										}else if(Link[ruler].i4 == -9){
											Link[ruler].i4 = i;
											send(fds[i],joinRoomRes(JOIN_SUCCESS,ruler),50,0);
											send(fds[Link[ruler].i1],noticeRes(ruler),48,0);
											send(fds[Link[ruler].i2],noticeRes(ruler),48,0);
											send(fds[Link[ruler].i3],noticeRes(ruler),48,0);
										}
	
									} 
									break;
								}
								case START:
								{
									//printf("start");
									ruler = startGameProcess(buff);
									if (Link[ruler].i4 != -9){
										send(fds[i],startGameRes(ruler, 0),50,0);
										send(fds[Link[ruler].i2],startGameRes(ruler, 1),50,0);
										send(fds[Link[ruler].i3],startGameRes(ruler, 2),50,0);
										send(fds[Link[ruler].i4],startGameRes(ruler, 3),50,0);
									}
									else if (Link[ruler].i3 != -9){
										send(fds[i],startGameRes(ruler, 0),50,0);
										send(fds[Link[ruler].i2],startGameRes(ruler, 1),50,0);
										send(fds[Link[ruler].i3],startGameRes(ruler, 2),50,0);
									}else if (Link[ruler].i2 != -9){
										send(fds[i],startGameRes(ruler, 0),50,0);
										send(fds[Link[ruler].i2],startGameRes(ruler, 1),50,0);
									}else{
										send(fds[i],startGameRes(ruler, 0),50,0);
									}
									break;
								}
								case BET:
								{
									int card[20];
									for (int i = 0; i < 20; ++i)
									{
										card[i] = -1;
									}
									int count = processBet(buff, card);
									
									if (card[1] == 4 && count == 4){
										send(fds[Link[card[0]].i1],betRes(card),50,0);
										send(fds[Link[card[0]].i2],betRes(card),50,0);
										send(fds[Link[card[0]].i3],betRes(card),50,0);
										send(fds[Link[card[0]].i4],betRes(card),50,0);
										send(fds[Link[card[0]].i1],turnRes(),50,0);	
									}else if (card[1] == 3 && count == 3){
										send(fds[Link[card[0]].i1],betRes(card),50,0);
										send(fds[Link[card[0]].i2],betRes(card),50,0);
										send(fds[Link[card[0]].i3],betRes(card),50,0);
										send(fds[Link[card[0]].i1],turnRes(),50,0);	
									}else if (card[1] == 2 && count == 2){
										send(fds[Link[card[0]].i1],betRes(card),50,0);
										send(fds[Link[card[0]].i2],betRes(card),50,0);
										send(fds[Link[card[0]].i1],turnRes(),50,0);	
									}else if (card[1] == 1 && count == 1){
										send(fds[Link[card[0]].i1],betRes(card),50,0);
										send(fds[Link[card[0]].i1],turnRes(),50,0);	
									}
									break;
								}
								case INSURANCE:
								{
									int room[6] = {0};
									int count = 0;
									processInsurance(buff, room);
									for(int i = 2; i < 6; i++){
										if(room[i] != 0){
											count++;
										}
									}
									if (count == room[1]){
										removeRoom(&headRoom, room[0]);
									}
									break;
								}
								case HIT:
								{
									int card[5];
									processHit(buff, card);
									if (Link[card[0]].i1 != -9)
									{
										send(fds[Link[card[0]].i1],hitRes(card),50,0);
									}
									if (Link[card[0]].i2 != -9)
									{
										send(fds[Link[card[0]].i2],hitRes(card),50,0);
									}
									if (Link[card[0]].i3 != -9)
									{
										send(fds[Link[card[0]].i3],hitRes(card),50,0);
									}
									if (Link[card[0]].i4 != -9)
									{
										send(fds[Link[card[0]].i4],hitRes(card),50,0);
									}
									break;
								}
								case DOUBLE:
								{
									int card[5];
									processHit(buff, card);
									if (Link[card[0]].i1 != -9)
									{
										send(fds[Link[card[0]].i1],doubleRes(card),50,0);
									}
									if (Link[card[0]].i2 != -9)
									{
										send(fds[Link[card[0]].i2],doubleRes(card),50,0);
									}
									if (Link[card[0]].i3 != -9)
									{
										send(fds[Link[card[0]].i3],doubleRes(card),50,0);
									}
									if (Link[card[0]].i4 != -9)
									{
										send(fds[Link[card[0]].i4],doubleRes(card),50,0);
									}
									break;
								}
								case SURRENDER:
								{	
									int turn[2];
									
									processStand(buff, turn);
									if (turn[1] == 1){
										send(fds[Link[turn[0]].i2],turnRes(),50,0);
									}
									else if (turn[1] == 2){
										send(fds[Link[turn[0]].i3],turnRes(),50,0);
									}
									else if (turn[1] == 3){
										send(fds[Link[turn[0]].i4],turnRes(),50,0);
									} else {
										int card[20];
										for (int i = 0; i < 20; ++i)
										{
											card[i] = -1;
										}
										processDealer(turn[0], card);
										if (Link[card[0]].i1 != -9)
										{
											send(fds[Link[card[0]].i1],dealerRes(card),50,0);
										}
										if (Link[card[0]].i2 != -9)
										{
											send(fds[Link[card[0]].i2],dealerRes(card),50,0);
										}
										if (Link[card[0]].i3 != -9)
										{
											send(fds[Link[card[0]].i3],dealerRes(card),50,0);
										}
										if (Link[card[0]].i4 != -9)
										{
											send(fds[Link[card[0]].i4],dealerRes(card),50,0);
										}
										processScore(turn[0]);
									}
									break;	
								}
								case STAND:
								{	
									int turn[2];
									
									processStand(buff, turn);
									if (turn[1] == 1){
										send(fds[Link[turn[0]].i2],turnRes(),50,0);
									}
									else if (turn[1] == 2){
										send(fds[Link[turn[0]].i3],turnRes(),50,0);
									}
									else if (turn[1] == 3){
										send(fds[Link[turn[0]].i4],turnRes(),50,0);
									} else {
										int card[20];
										for (int i = 0; i < 20; ++i)
										{
											card[i] = -1;
										}
										processDealer(turn[0], card);
										if (Link[card[0]].i1 != -9)
										{
											send(fds[Link[card[0]].i1],dealerRes(card),50,0);
										}
										if (Link[card[0]].i2 != -9)
										{
											send(fds[Link[card[0]].i2],dealerRes(card),50,0);
										}
										if (Link[card[0]].i3 != -9)
										{
											send(fds[Link[card[0]].i3],dealerRes(card),50,0);
										}
										if (Link[card[0]].i4 != -9)
										{
											send(fds[Link[card[0]].i4],dealerRes(card),50,0);
										}
										processScore(turn[0]);
									}
									break;	
								}
								// case RAISE:
								// {
								// 	int i2 = getOpponentIndex(i); 
								// 	send(fds[i2],buff,res+1,0);
								// 	break;
								// }
								// case ENDGAME:
								// {
								// 	int idd = getIdbyIndex(i);
								// 	int i2 = Link[idd].i1 == i ? Link[idd].i2 : Link[idd].i1;
								// 	ruler = processEnd(buff,idd);
								// 	if(ruler == -1)
								// 	{
								// 		send(fds[i2],makeEndRes(ruler,idd,0),20,0);
								// 	}
								// 	else
								// 	{
								// 		if(i2 == Link[idd].i2)
								// 		{
								// 			send(fds[i],makeEndRes(ruler,idd,1),20,0);
								// 			send(fds[i2],makeEndRes(ruler,idd,2),20,0);
								// 		}
								// 		else
								// 		{
								// 			send(fds[i],makeEndRes(ruler,idd,2),20,0);
								// 			send(fds[i2],makeEndRes(ruler,idd,1),20,0);
								// 		}
								// 		Link[idd].i1 = 0;
								// 		Link[idd].i2 = 0;
								// 	} 
								// 	break;
								// }
								
								case GETSCORE:
								{
									ruler = processGetScore(buff);
									send(fds[i],scoreMessage(ruler),10,0);

									break;
								}
							}
						}
					}
				}
			}
		}
	}
}