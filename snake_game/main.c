#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<curses.h>
#include<pthread.h>

// define #
enum dir { up = 'w', down = 's', left = 'a', right = 'd' };
enum type { wall = -1, none = 0, body = 1, food = 2 };
typedef struct { int type, lock; } tile;
typedef struct { int x, y; } pos;

// declare # func
tile** init_setting(int size, pos start_point);
void change_head(int next_dir);
void move_head();
void print();

// declare # value
tile** field;
pos head;
int next_dir;
int length;
int size;

int main(void)
{
	// set size
	while (1)
	{
		printf("Set field size greater than 10. ( If type 0, set default 51 )\nSize : ");
		scanf("%d", &size);
		if (size == 0)
			size = 51;
		if (size > 10)
			break;
		if (size <= 10)
			printf("Set too small size, set again greater than 10.\n");
	}

	// start initialization
	length = 3;
	next_dir = left;
	head.x = size / 2, head.y = size / 2;
	field = init_setting(size, head);

	// start game
	initscr();
	print();
	while (1)
	{
		scanf("%d", &next_dir);
		change_head(next_dir);
		if (field[head.y][head.x].lock != 0) // end condition
			break;
		move_head();
		print();
		refresh();
	}
	endwin();

	return 0;
}

tile** init_setting(int size, pos start_point)
{
	// create field of input size
	tile** tmp = (tile**)malloc(sizeof(tile*) * size);
	for (int i = 0; i < size; i++)
	{
		tmp[i] = (tile*)malloc(sizeof(tile) * size);
		for (int j = 0; j < size; j++)
		{
			tmp[i][j].lock = 0;
			tmp[i][j].type = none;
		}
	}

	// make basic map
	for (int i = 0; i < size; i++)
	{
		tmp[0][i].type = wall;
		tmp[0][i].lock = -1;
	}
	for (int i = 1; i < size - 1; i++)
	{
		tmp[i][0].type = wall;
		tmp[i][0].lock = -1;
		tmp[i][size - 1].type = wall;
		tmp[i][size - 1].lock = -1;
	}
	for (int i = 0; i < size; i++)
	{
		tmp[size - 1][i].type = wall;
		tmp[size - 1][i].lock = -1;
	}

	// get ready
	for (int i = length; i > 0; i--)
	{
		tmp[start_point.y][start_point.x + length - i].lock = i;
		tmp[start_point.y][start_point.x + length - i].type = body;
	}

	return tmp;
}

void change_head(int dir)
{
	// change head pointer
	switch (dir)
	{
	case up:
		head.y = head.y - 1;
		break;
	case down:
		head.y = head.y + 1;
		break;
	case left:
		head.x = head.x - 1;
		break;
	case right:
		head.x = head.x + 1;
		break;
	}
}

void move_head()
{
	// change field status
	field[head.y][head.x].lock = length;
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			if (field[i][j].lock > 0)
				field[i][j].lock--;
}

void print()
{
	move(0, 0);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			switch (field[i][j].type)
			{
			case wall:
				//				printf("��");
//				addstr("��");
				addstr("*");
				break;
			case body:
				//				printf("��");
				addstr("@");
				break;
			case none:
				//				printf("��");
				addstr(" ");
				break;
			case food:
				//				printf("��");
				break;
			}
		}
		move(i, 0);
	}
}