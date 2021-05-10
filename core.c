#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "turtle.h"

#define _CRT_SECURE_NO_WARNINGS
#define ROW_LENGTH 80
#define MAX_VALUE_LENGTH 60

typedef struct Field 
{
	int current;
	int next;
} Field;

typedef struct IntListNode
{
	int value;
	struct IntListNode* next;
} IntList;

typedef struct PositionListNode
{
	int column;
	int row;
	struct PositionListNode* next;
} PositionList;


/* Zwalnianie pamiêci                                                        */
void free_mem(IntList*, IntList*, PositionList*, Field**, int);
/* Generuje obraz */
void draw_frame(Field**, int, int);
/* Wypisanie aktualnego stanu planszy                                        */
void print_board(FILE* output, Field**, int);
/* Nadpisanie aktualnych stanów komórek nastêpnymi i wyzerowanie nastêpnych  */
void apply_next_state(Field**, int);
/* Przygotowanie nastêpnych stanów komórek                                   */
void prepare_next_state(Field**, int, IntList*, IntList*);
/* Aktywowanie komórek zawartych w liœcie                                    */
void init_state(Field**, int, PositionList*);
/* Alokowanie pamiêci dla planszy                                            */
Field** init_board(int);
/* Wczytanie danych z pliku konfiguracyjnego                                 */
void load_config(IntList*, IntList*, PositionList*, int*, int*, int*, int*);

int main()
{
	int i, iter = 0, fieldSize = 0, boardSize = 0, outputMode = 0;
	FILE* output = stdout;
	Field** board = NULL;
	IntList* bornConditions = calloc(sizeof(IntList),1);
	IntList* surviveConditions = calloc(sizeof(IntList), 1);
	PositionList* initPositions = calloc(sizeof(PositionList),1);

	load_config(bornConditions, surviveConditions, initPositions, &iter, &fieldSize, &boardSize, &outputMode);
	board = init_board(boardSize);

	init_state(board, boardSize, initPositions);

	for (i = 0; i < iter; i++)
	{
		if (outputMode & 1)
		{
			draw_frame(board, boardSize, fieldSize);
			turtle_save_frame();
			turtle_cleanup();
		}
		
		prepare_next_state(board, boardSize, bornConditions, surviveConditions);
		apply_next_state(board, boardSize);
	}

	if (outputMode & 2)
	{
		draw_frame(board, boardSize, fieldSize);
		turtle_save_frame();
		turtle_save_bmp("output.bmp");
		turtle_cleanup();
	}

	if(outputMode & 4)
		print_board(output, board, boardSize);

	if (outputMode & 8)
	{
		output = fopen("output.txt", "w");
		print_board(output, board, boardSize);
	}
	
	fclose(output);
	free_mem(bornConditions, surviveConditions, initPositions, board, boardSize);
	return 0;
}

/* Load Config */
void add_int_to_list(IntList* list, int value)
{
	if (list->next)
		add_int_to_list(list->next, value);
	else
	{
		list->next = calloc(sizeof(IntList), 1);
		list->next->value = value;
	}
}
void add_position_to_list(PositionList* list, int column, int row)
{
	if (list->next)
		add_position_to_list(list->next, column, row);
	else
	{
		list->next = calloc(sizeof(PositionList), 1);
		list->next->column = column;
		list->next->row = row;
	}
}
int read_config(char* fields[])
{
	FILE* config = NULL;
	config = fopen("config", "r");
	char buff[ROW_LENGTH] = { '\0' };
	char subbuff[MAX_VALUE_LENGTH] = { '\0' };
	int oindex = 0;
	int i;

	if (config == NULL)
	{
		perror("Nie mozna otworzyc pliku config.");
		exit(1);
	}

	while (fgets(buff, sizeof(buff), config) != NULL)
	{
		if (buff[0] != '#')
		{
			for (i = 0; buff[i] != '=' && i < ROW_LENGTH; i++);

			strncpy(subbuff, buff + i + 1, strlen(buff) - i - 1);
			strcpy(fields[oindex++], subbuff);
		}
	}

	fclose(config);
	return 1;
}
void load_config(IntList* born, IntList* survive, PositionList* initPositions, int* iter, int* fieldSize, int* size, int* outputMode)
{
	char chr, neastChr;
	int i = -1, j = -1, k = -1, row = -1, column = -1;
	char buff[MAX_VALUE_LENGTH] = { '\0' };
	char bornStr[MAX_VALUE_LENGTH] = { '\0' };
	char surviveStr[MAX_VALUE_LENGTH] = { '\0' };
	char iterStr[MAX_VALUE_LENGTH] = { '\0' };
	char fieldSizeStr[MAX_VALUE_LENGTH] = { '\0' };
	char sizeStr[MAX_VALUE_LENGTH] = { '\0' };
	char initStr[MAX_VALUE_LENGTH] = { '\0' };
	char outputModeStr[MAX_VALUE_LENGTH] = { '\0' };

	char* config[7] = { bornStr, surviveStr, iterStr, fieldSizeStr, sizeStr, outputModeStr, initStr };
	initPositions->row = -1;
	initPositions->column = -1;
	born->value = -1;
	survive->value = -1;
	
	read_config(config);

	*size = atoi(sizeStr);
	*fieldSize = atoi(fieldSizeStr);
	*iter = atoi(iterStr);
	*outputMode = atoi(outputModeStr);
	
	while ((chr = bornStr[++i]) != '\0')
		if (chr != ',' && chr != '\n')
		{
			k = chr - '0';
			add_int_to_list(born, k);
		}
			

	i = -1;

	while ((chr = surviveStr[++i]) != '\0')
		if (chr != ',' && chr != '\n')
		{
			k = chr - '0';
			add_int_to_list(survive, k);
		}
			

	i = -1;

	while ((chr = initStr[++i]) != '\0')
	{
		j = i - 1;
		while ((neastChr = initStr[++j]) != '-');
		strncpy(buff, initStr + i, j - i);
		column = atoi(buff);

		k = j;
		memset(buff, 0, MAX_VALUE_LENGTH);

		while ((neastChr = initStr[++j]) != ',');
		strncpy(buff, initStr + k + 1, j - k - 1);
		row = atoi(buff);

		memset(buff, 0, MAX_VALUE_LENGTH);

		add_position_to_list(initPositions, column, row);

		i = j;
	}
		
}

/* Init board */
Field** init_board(int size)
{
	int i;
	Field** board = calloc(sizeof(Field), size);
	for (i = 0; i < size; i++)
		board[i] = calloc(sizeof(Field), size);

	return board;
}

/* Init state */
int position_list_contain(PositionList* list, int column, int row)
{
	if (list)
	{
		if (list->column == column && list->row == row)
			return 1;
		return position_list_contain(list->next, column, row);
	}
	return 0;
}
void init_state(Field** board, int boardSize, PositionList* positions)
{
	int i, j;

	for (i = 0; i < boardSize; i++)
		for (j = 0; j < boardSize; j++)
			if (position_list_contain(positions, i, j))
				(*(board + i) + j)->current = 1;
}

/* Prepare next state */
int int_list_contain(IntList* list, int value)
{
	if (list)
	{
		if (list->value == value)
			return 1;
		return int_list_contain(list->next, value);
	}
	return 0;
}
int modulo(int val, int mod)
{
	if (val < 0)
		return mod + val;
	return val % mod;
}
int count_neighbour(Field** board, int boardSize, int currentColumn, int currentRow)
{
	int i, j, columnAdr, rowAdr, output = 0;

	for (i = -1; i <= 1; i++)
		for (j = -1; j <= 1; j++)
		{
			if (i == 0 && j == 0)
				continue;

			columnAdr = modulo(j + currentColumn, boardSize);
			rowAdr = modulo(i + currentRow, boardSize);

			if ((*(board + columnAdr) + rowAdr)->current == 1)
				output++;
		}

	return output;
}
void prepare_next_state(Field** board, int boardSize, IntList* born, IntList* survive)
{
	int i, j;

	for (i = 0; i < boardSize; i++)
		for (j = 0; j < boardSize; j++)
		{
			if ((*(board + j) + i)->current == 1)
			{
				if (!int_list_contain(survive, count_neighbour(board, boardSize, j, i)))
					(*(board + j) + i)->next = 0;
				else
					(*(board + j) + i)->next = 1;
			}
			else
			{
				if (int_list_contain(born, count_neighbour(board, boardSize, j, i)))
					(*(board + j) + i)->next = 1;
			}
		}
}

/* Apply next state */
void apply_next_state(Field** board, int boardSize)
{
	int i, j;

	for (i = 0; i < boardSize; i++)
		for (j = 0; j < boardSize; j++)
		{
			(*(board + j) + i)->current = (*(board + j) + i)->next;
			(*(board + j) + i)->next = 0;
		}
}

/* Print board */
void print_board(FILE* output, Field** board, int boardSize)
{
	int i, j;

	for (i = 0; i < boardSize; i++)
	{
		for (j = 0; j < boardSize; j++)
			fprintf(output, " %c ", (*(board + j) + i)->current == 1 ? '@' : '_');
		fprintf(output, "\n");
	}
		
}

void draw_sqr(int size)
{
	int i;

	turtle_begin_fill();

	for (i = 0; i < 4; i++)
	{
		turtle_forward(size-1);
		turtle_turn_right(90);
	}

	turtle_end_fill();
}
void move_turtle_to(int column, int row, int boardSize, int fieldSize)
{
	int boardSizeInPixels = boardSize * fieldSize;

	turtle_pen_up();
	turtle_goto(-boardSizeInPixels / 2 + (column*fieldSize), boardSizeInPixels / 2 - fieldSize - (row * fieldSize));
	turtle_pen_down();
}
void draw_frame(Field** board, int boardSize, int fieldSize)
{
	int i, j, boardSizeInPixels = boardSize * fieldSize;

	turtle_init(boardSizeInPixels, boardSizeInPixels);
	turtle_set_fill_color(0, 0, 0);
	
	turtle_turn_left(90);

	for (i = 0; i < boardSize; i++)
		for (j = 0; j < boardSize; j++)
			if ((*(board + j) + i)->current == 1)
			{
				move_turtle_to(j, i, boardSize, fieldSize);
				draw_sqr(fieldSize);
			}
}

/* Free memory */
void destroy_int_list(IntList* list)
{
	if (list->next)
		destroy_int_list(list->next);

	free(list);
}
void destroy_position_list(PositionList* list)
{
	if (list->next)
		destroy_position_list(list->next);

	free(list);
}
void free_mem(IntList* born, IntList* survive, PositionList* initPositions, Field** board, int boardSize)
{
	int i;

	destroy_int_list(born);
	destroy_int_list(survive);
	destroy_position_list(initPositions);

	for (i = 0; i < boardSize; i++)
		free(*(board + i));		
}
