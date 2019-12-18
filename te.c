#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<ncursesw/curses.h>
#include<pthread.h>
#include<termios.h>
#include<time.h>
#include<sys/time.h>
#include<locale.h>

// define #
#define wallp  "ㅁ"
#define nonep  "  "
#define bodyp  "ㅇ"
#define foodp  "★ "
#define MAX_QUEUE_SIZE 5
enum dir { up = 'w', down = 's', left = 'a', right = 'd' };
enum type { wall = -1, none = 0, body = 1, food = 2, forward = 3 };
typedef struct { int type, lock; } tile;
typedef struct { int x, y; } pos;
struct termios info;
struct itimerval timer;

// declare # func
tile** init_setting(int size, pos start_point);
void change_head(char next_dir);
void move_head();
void print();
void* inp_key();
void create_food();

// declare # value
tile** field;
pos head;
char next_dir, backup;
int length;
int size;
char inp_list[MAX_QUEUE_SIZE] = { 0 };
int front = 0, rear = 0;
pthread_mutex_t for_queue = PTHREAD_MUTEX_INITIALIZER;
bool exit_condition = TRUE;

int main()
{
	setlocale(LC_ALL, "ko_KR.utf-8");

	info.c_lflag |= ICANON;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 5000;

	// set size
	initscr();
	clear();
	size = 41;

	// start initialization
	exit_condition = TRUE;
	length = 3;
	next_dir = left;
	backup = left;
	head.x = size / 2, head.y = size / 2;
	field = init_setting(size, head);
	create_food();

	// create thread for insert inp_key
	pthread_t t1;
	pthread_create(&t1, NULL, inp_key, NULL);

	print();
	refresh();
	while (1)
	{
		//fflush(stdin);
		//scanf("%c", &next_dir);
		usleep(100000);
		//setitimer(ITIMER_REAL, &timer, NULL);
		pthread_mutex_lock(&for_queue);
		if (front != rear)
		{
			next_dir = inp_list[front++];
			backup = next_dir;
			if (front == MAX_QUEUE_SIZE)
				front = 0;
		}
		else
		{
			next_dir = backup;
		}
		pthread_mutex_unlock(&for_queue);

		change_head(next_dir);
		if (field[head.y][head.x].lock != 0) // end condition
		{
			exit_condition = FALSE;
			break;
		}
		move_head();
		print();
		refresh();
	}

	move((size / 2) + 1, size + 50);
	addstr("game over");
	refresh();

	pthread_join(t1, NULL);
	endwin();

	return 0;
}

tile** init_setting(int size, pos start_point)
{
	// create field of input size
	tile** tmp = (tile * *)malloc(sizeof(tile*) * size);
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

void change_head(char dir)
{
	// change head pointer
	switch (dir)
	{
	case 'w':
		head.y = head.y - 1;
		break;
	case 's':
		head.y = head.y + 1;
		break;
	case 'a':
		head.x = head.x - 1;
		break;
	case 'd':
		head.x = head.x + 1;
		break;
	}
}

void move_head()
{
	// change field status
	if (field[head.y][head.x].type == food) // if food
	{
		length++;
		create_food();
	}
	else
	{
		for (int i = 1; i < size - 1; i++)
			for (int j = 1; j < size - 1; j++)
				if (field[i][j].lock > 0)
					field[i][j].lock--;
	}

	field[head.y][head.x].type = body;
	field[head.y][head.x].lock = length;

	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			if (field[i][j].lock == 0 && field[i][j].type == body)
				field[i][j].type = none;
}

void print() 
{
	move(size / 2, size + 50);
	addstr("your score: ");
	printw("%d", length - 3);
	refresh();

	move(0, 0);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (field[i][j].type == wall)
				printw(wallp);
			if (field[i][j].type == body)
				printw(bodyp);
			if (field[i][j].type == none)
				printw(nonep);
			if (field[i][j].type == food)
				printw(foodp);
			if (field[i][j].type == forward)
				printw("◎");
		
		}
		move(i + 1, 0);

		refresh();
	}
}

void* inp_key()
{
	while (exit_condition)
	{
		char input;
		scanf("%c", &input);
		pthread_mutex_lock(&for_queue);
		inp_list[rear++] = input;
		if (rear == MAX_QUEUE_SIZE)
			rear = 0;
		pthread_mutex_unlock(&for_queue);
	}
}

void create_food()
{
	int x, y;
	srand((unsigned int)time(NULL));
	x = rand() % (size - 2) + 1;
	y = rand() % (size - 2) + 1;
	if (field[x][y].type != body)
		field[y][x].type = food;
	else
		create_food();
}
