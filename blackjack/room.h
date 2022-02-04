#ifndef __ROOM__
#define __ROOM__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"


typedef struct
{
	char nickname[20];
	Card hand[10];
	int ncard;
	int point;
}Player;

typedef struct box 
{
	int id;
	int status;//1 = private or  0 = public
	int slot;
	int ncard;
	char password[20];
	int canPlay;
	Card dealerHand[10];
	int ncardealer;
	Player player[4];//creator
	int start;//slot start
	int bet[4];
	int deck[52];
	struct box* next;
}Room;
Room *makeListRoom();
Player newPlayer(char* nickname,int money);
void joinRoom(Room *headRoom, Player newPlayer);
Room *getRoombyID(Room *headRoom, int id);
int pushRoom(Room **headRoom, int status, char* password, Player creator_room);
//int* newround(Room *headRoom,int bet[],Card playercard[][7]);
void setDeckToRoom(Room *room);
void removeRoom(Room *headRoom,int id);
#endif