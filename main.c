#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

#define FOOD 'X'
#define EMPTY '-'
#define SNAKE '#'
#define START_LENGTH 4
#define TICK_LENGTH 0.1f
#define MAX_STATUS_TICK_COUNT 30
#define MAX_LENGTH 50
#define SIZE 26

void drawMap(char* map) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			printf("%c ", map[x + (y * SIZE)]);
		}
		printf("\n");
	}
}

int getIdx(int x, int y) {
	return  x + (y * SIZE);
}

int getKeyPressed(char c) {
	SHORT state = GetKeyState(c);
	return (state & 0x8000) ? 1 : 0;
}

void initMap(char* map, int* snake, int* pos) {
	for (int j = START_LENGTH, y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			int idx = getIdx(x, y);
			char c = EMPTY;
			if (pos[0] == x && j > 0) {
				if (pos[1] - (j - 1) == y) {
					j--;
					snake[j] = idx;
					c = SNAKE;
				}
			}
			map[idx] = c;
		}
	}
}

int main(int argc, char argv[]) {
	int length = START_LENGTH;
	int pos[2] = { SIZE / 2, SIZE / 2 };
	int* snake = (int*)malloc(sizeof(int) * MAX_LENGTH);
	char* map = (char*)malloc(sizeof(char) * SIZE * SIZE);
	initMap(map, snake, pos);

	unsigned char moving = 0;
	unsigned char food = 1;
	unsigned char flags = 0x00; 
	int dirX = 0;
	int dirY = 0;
	clock_t start = clock();
	float dt = 0.f;
	typedef enum {
		WON,
		LOSE,
		NONE
	} Status;
	Status status = NONE;
	int statusTickCount = MAX_STATUS_TICK_COUNT;
	while (1) {
		dt += (float)(clock() - start) / CLOCKS_PER_SEC;
		if (dt > TICK_LENGTH) {
			dt = 0.f;

			if (getKeyPressed('Q'))
				break;

			//move snake
			if (dirY == 0) {
				if (getKeyPressed('W') && flags & (1 << moving))
					dirY = -1;
				else if (getKeyPressed('S'))
					dirY = 1;
				if (dirY != 0)
					dirX = 0;
			}
			if (dirX == 0) {
				if (getKeyPressed('A'))
					dirX = -1;
				else if (getKeyPressed('D'))
					dirX = 1;
				if (dirX != 0)
					dirY = 0;
			}
			if (dirX != 0 || dirY != 0)
				flags = flags | (1 << moving);
			Status prevStatus = status;
			if (status == NONE) {
				if (flags & (1 << moving)) {
					pos[0] += dirX;
					pos[1] += dirY;
					if (pos[0] == SIZE + 1)
						pos[0] = 0;
					else if (pos[0] == -1)
						pos[0] = SIZE - 1;
					if (pos[1] == SIZE + 1)
						pos[1] = 0;
					else if (pos[1] == -1)
						pos[1] = SIZE - 1;

					//update snake on map (pos is start of array, end of array is end of snake)
					int newIdx = getIdx(pos[0], pos[1]);
					if (map[newIdx] == SNAKE)
						status = LOSE;
					if (food && map[newIdx] == FOOD) {
						flags = flags & ~(1 << food);
						length++;
						if (length >= MAX_LENGTH)
							status = WON;
					}
					else
						map[snake[length - 1]] = EMPTY;
					for (int i = length - 1; i > 0; i--)
						snake[i] = snake[i - 1]; //move values down one
					snake[0] = getIdx(pos[0], pos[1]);
					map[snake[0]] = SNAKE;

				}

				//spawn food
				if ((flags & (1 << food)) == 0 && length <= MAX_LENGTH) {
					int x = 1 + (rand() % (SIZE - 2));
					int y = 1 + (rand() % (SIZE - 2));
					int idx = getIdx(x, y);
					if (map[idx] == EMPTY) {
						map[idx] = FOOD;
						flags = flags | (1 << food);
					}
				}
			}

			if (prevStatus != status) {
				flags = flags & ~(1 << food);
				statusTickCount = MAX_STATUS_TICK_COUNT;
				length = START_LENGTH;
				pos[0] = SIZE / 2;
				pos[1] = SIZE / 2;
				dirX = 0;
				dirY = 0;
				initMap(map, snake, pos);
			}

			//clear
			system("cls");

			//draw
			drawMap(map);
			char* statusStr = "  -  ";
			if (status == WON) {
				statusStr = "Won!";
				statusTickCount--;
			}
			else if (status == LOSE){
				statusStr = "Lost!";
				statusTickCount--;
			}
			else
				statusStr = "  -  ";
			if (statusTickCount == 0)
				status = NONE;
			printf("Length: %d\n%s ", length, statusStr);
		}
		start = clock();
	}
	//clean up
	free(map);
	free(snake);
	return 0;
}