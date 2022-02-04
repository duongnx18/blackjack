#include "room.h"

Room *makeListRoom()
{
	Room *new = (Room*)calloc(1,sizeof(struct box));
	memset(new,0,sizeof(struct box));
	return new;
}
Player newPlayer(char* nickname,int chip)
{
	Player new;
	strcpy(new.nickname,nickname);
	return new;
}

void joinRoom(Room *room, Player newPlayer)
{
	if(room->canPlay==1)
	{
		printf("Full slot\n");
		return;
	}
	room->player[room->slot] = newPlayer;
	room->slot++;
	if(room->slot == 4)
		room->canPlay = 1;
}

Room *getRoombyID(Room *headRoom, int id)
{
	Room *r = headRoom;
	while(r!=NULL)
	{
		if(r->id == id)
			return r;
		r = r->next;
	}
	return NULL;
}
void setDeckToRoom(Room *room)
{
	memset(room->deck,0,sizeof(room->deck));
	int * deck = rollcard();
	for(int i = 0; i < 52; i++)
	{
		room->deck[i] = deck[i];
	}
	room->ncard = 0;
}



Room *newRoom(int status, char* password, Player creator_room)
{
	Room *new = (Room*)malloc(sizeof(Room));
	memset(new,0,sizeof(struct box));
	new->status = status;
	new->canPlay = 0;
	strcpy(new->password,password);
	new->player[0] = creator_room;
	new->slot = 1;
	new->next = NULL;
	return new;
}
int pushRoom(Room **headRoom, int status, char* password, Player creator_room)
{
	int id = 0;
	Room *new = newRoom(status,password,creator_room);
	if((*headRoom)->id == 0)
	{
		*headRoom = new;
		(*headRoom)->id = 1;
		return 1;
	}
	Room *r = *headRoom;
	while(r->next!=NULL)
	{
		r = r->next;
	}
	r->next = new;
	id = r->id + 1;
	r->next->id = id;
	return id;
}

void removeRoom(Room *headRoom, int id)
{
	Room *r = getRoombyID(headRoom,id);
	if(r == NULL)
		return;
	if(r->id == headRoom->id)
	{
		headRoom = headRoom->next;
		if(headRoom == NULL)
			headRoom = makeListRoom();
		printf("Delete head\n");
	}
	else if(r->next == NULL)
	{	
		r = headRoom;
		while(r->next->next!=NULL)
			r = r->next;
		r->next = NULL;
		printf("Delete last\n");
	}
	else
	{
		r = headRoom;
		while(r->next->id!= id)
		{
			r = r->next;
		}
		r->next = r->next->next;
		printf("Delete\n");
	}
}
