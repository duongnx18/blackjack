#ifndef __PROTOCOL__
#define __PROTOCOL__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#define MSG_SIZE 1024

typedef enum{
	REGISTER,
	LOGIN,
	CREATEROOM,
	GETLIST,
	JOINROOM,
	BET,
	GETSCORE,
	START,
	HIT,
	STAND,
	DOUBLE,
	SURRENDER
}MSG_TYPE;

typedef enum{
	REG_RES,
	LOG_RES,
	CRE_RES,
	GET_RES,
	JOI_RES,
	NOT_RES,
	BET_RES,
	END_RES,
	STA_RES,
	SCORE_RES,
	TURN_RES,
	HIT_RES,
	SUR_RES,
	DOU_RES,
	DEA_RES
}RES_TYPE;

typedef enum{
	SUCCESS_SIGNIN,//dang nhap thanh cong
	NOT_EXIST,//tai khoan khong ton tai
	WRONG_PASS,//sai mat khau
	LOGED_IN,//dang nhap o noi khac roi
	SUCCESS_SIGNUP,//dang ky thanh cong
	EXISTED,//tai khoan da ton tai
	WRONG_RPASS,// sai pass room
	ROOM_NEXIST,//phong khong ton tai
	FULL_SLOT,//phong day
	JOIN_SUCCESS,//vao phong thanh cong
	CRE_SUCCESS,
	NEW_PLAYER,
}VALUE_RES;


char *makeSignInMessage();
char *makeSignUpMessage();
char *startGameMessage(char id[]);
char *makeCreateRoomMessage(char* nickname);
char *getAvailableRoomMessage();
char *makeJoinRoomMessage(char *nickname);
char *makeBetMessage(int newbet, int id, int turn);
char *makePlayMessage(int option, int id, int turn);
char *getScoreMessage(char *nickname);

#endif