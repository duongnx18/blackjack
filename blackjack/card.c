#include "card.h"



char* convertType(int type)
{
	if(type ==1)
		return "♠";
	if(type == 2)
		return "♣";
	if(type == 3)
		return "♦";
	if(type == 4)
		return "♥";
}

char* convertValue(int value)
{
	if(value== 1)
		return "A";
	if(value== 2)
		return "2";
	if(value== 3)
		return "3";
	if(value== 4)
		return "4";
	if(value== 5)
		return "5";
	if(value== 6)
		return "6";
	if(value== 7)
		return "7";
	if(value== 8)
		return "8";
	if(value== 9)
		return "9";
	if(value== 10)
		return "10";
	if(value== 11)
		return "J";
	if(value== 12)
		return "Q";
	if(value== 13)
		return "K";
}

Card newcard(int input)
{
	Card new;	
	new.value = input%13+1;
	new.type =  input/13 + 1;
	strcpy(new.showvalue,convertValue(new.value));
	strcpy(new.showtype,convertType(new.type));
	return new;
}

int* rollcard()
{
	sleep(1);
	int* deck = (int*)calloc(DECK_SIZE,sizeof(int));
	int j, k, temp;
	for(int i = 0; i < 52; i++)
	{
		deck[i] = i;
	}
	srand(time(NULL));
	for(int i = 0;i<200; i++)
	{
		j = rand()%DECK_SIZE;
		k = rand()%DECK_SIZE;
		temp = deck[j];
		deck[j] = deck[k];
		deck[k] = temp;
	}
	return deck;
}

int calc_sum(Card hand[], int ncards)
{
	int i;
	int sum = 0;

	for (i = 0; i < ncards; ++i)
		if (hand[i].value < 10)
			sum += hand[i].value;
		else
			sum += 10;

	for (i = 0; i < ncards; ++i)
		if (hand[i].value == 1)
			if (sum + 10 <= 21)
				sum += 10;

	return sum;
}
void display_state(Card hand[], int ncards)
{
	int i;
	for (i = 0; i < ncards; ++i)
	{
		if (i > 0)
			printf(" ");
		//printf("%s-%s", values[hand_values[i]], suits[hand_suits[i]]);

		printf(" || %s  %s ||", hand[i].showvalue, hand[i].showtype);
		
		if(ncards == 1) printf("  || X  X ||");
	}
	printf("\n");
}