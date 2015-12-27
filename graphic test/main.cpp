#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <conio.h>
#include <time.h>
#define NEWGAME 1
#define LOADGAME 2
#define RANKINGLIST 3
#define EXIT 0

#define LEFT  75
#define RIGHT 77
#define UP    72
#define DOWN  80

#define STARTLEN 4
#define MAPSIZE 28

#define NOTHING 0
#define WALL 1
#define SNAKE 2
#define FOOD 3
#define BOMB 4
#define DRUG 5
#define HUNGRY 6
#define TOOSHORT 7

int map[MAPSIZE][MAPSIZE] = { 0 };

int pause = 0, gameOver = 0, length = STARTLEN, score = 0, key = UP;  //��ͣ����Ϸ�����������ȣ���ǰ�÷֣�����
int foodExist = 0, drugExist = 0, remainingBomb;                  //ʳ���Ƿ���ڣ�ը����ʣ������
int life = 60;

struct snake {  //�ߵĽṹ��
	int x;
	int y;
	struct snake *previous;
	struct snake *next;
}*head, *tail;

struct food {   //ʳ��ṹ��
	int x, y;
}food, drug[9], bomb[9];

enum direction {   //����ö��
	up,
	down,
	left,
	right
}direction;

enum stage {
	easy = 1, normal, hard, allClear
}stage;

int mainMenu(void) {   //���˵�
	FlushMouseMsgBuffer();
	setbkcolor(RGB(190, 231, 233));
	cleardevice();
	MOUSEMSG m;
	settextcolor(RGB(19, 134, 232));
	settextstyle(65, 0, _T("΢���ź�"));
	outtextxy(275, 75, "������С��");
	for (int i = 200; i < 450; i += 80)
	{
		setcolor(RGB(70, 77, 89));
		roundrect(340, i, 460, i + 60, 10, 10);
	}
	settextstyle(27, 0, _T("΢���ź�"));
	outtextxy(360, 215, "��ʼ��Ϸ");
	outtextxy(360, 295, "������Ϸ");
	outtextxy(370, 375, "���а�");
	outtextxy(380, 455, "�˳�");
	while (1)
	{
		m = GetMouseMsg();
		if (m.mkLButton && m.x > 300 && m.x < 500)
		{
			if (m.y > 200 && m.y < 260)
			{
				return NEWGAME;
			}
			else if (m.y > 280 && m.y < 340)
			{
				return LOADGAME;
			}
			else if (m.y > 360 && m.y < 420)
			{
				return RANKINGLIST;
			}
			else if (m.y > 440 && m.y < 500)
			{
				return EXIT;
			}
		}
	}
}

void selectStage(void) {
	setbkcolor(RGB(190, 231, 233));
	cleardevice();
	FlushMouseMsgBuffer();
	MOUSEMSG m;
	settextcolor(RGB(19, 134, 232));
	settextstyle(65, 0, _T("΢���ź�"));
	outtextxy(300, 75, "ѡ���Ѷ�");
	for (int i = 200; i <= 360; i += 80)
	{
		setcolor(RGB(70, 77, 89));
		roundrect(340, i, 460, i + 60, 10, 10);
	}
	settextstyle(27, 0, _T("΢���ź�"));
	outtextxy(380, 215, "Easy");
	outtextxy(365, 295, "Normal");
	outtextxy(377, 375, "Hard");
	while (1)
	{
		m = GetMouseMsg();
		if (m.mkLButton && m.x > 300 && m.x < 500)
		{
			if (m.y > 200 && m.y < 260)
			{
				stage = easy;
				return;
			}
			else if (m.y > 280 && m.y < 340)
			{
				stage = normal;
				return;
			}
			else if (m.y > 360 && m.y < 420)
			{
				stage = hard;
				return;
			}
		}
	}
}

void loadGame(void) {    //������Ϸ��δ��ɣ�
	cleardevice();
	settextstyle(30, 0, _T("����"));
	outtextxy(325, 120, "�����ڴ�(������˳�)");
	_getch();
}

void printrankingList(void)   //���а�δ��ɣ�
{
	cleardevice();
	settextstyle(30, 0, _T("����"));
	outtextxy(325, 120, "�����ڴ�(������˳�)");
	_getch();
}


void initSnake(void)	 {    //��ʼ����������������Ĺ���������ĳ�ʼ����
	int i;
	struct snake *newNode, *current;

	head = (struct snake *)malloc(sizeof(struct snake));
	head->previous = NULL;
	current = head;
	for (i = 1; i < STARTLEN; i++) {
		newNode = (struct snake *)malloc(sizeof(struct snake));
		current->next = newNode;
		newNode->previous = current;
		current = current->next;
		tail = newNode;
	}
	tail->next = NULL;

	int m = 15, n = 15;
	current = head;
	while (current != NULL) {
		current->x = m;
		current->y = n;
		map[m][n] = SNAKE;
		n++;
		current = current->next;
	}
}

void initMap(int stage) {  //��ʼ����ͼ�������Ѷ�
	int i, j;
	map[MAPSIZE][MAPSIZE] = { 0 };
	for (i = 7; i < MAPSIZE-6; i++) {
		map[i][0] = WALL;
		map[0][i] = WALL;
		map[i][MAPSIZE - 1] = WALL;
		map[MAPSIZE - 1][i] = WALL;
	}

	if (stage == normal || stage == hard) {
		for (i = 11; i < 17; i++)
			for (j = 0; j < 5; j++) {
				map[i][j] = WALL;
				map[i][MAPSIZE - 1 - j] = WALL;
			}
		for (i = 0; i < 5; i++)
			for (j = 10; j < 16; j++) {
				map[i][j] = WALL;
				map[MAPSIZE - 1 - i][j] = WALL;
			}
		if (stage == hard) {
			for (i = 8; i < 20; i += 3) {
				map[i][8] = WALL;
				map[i][20] = WALL;
			}
			for (j = 8; j <= 20; j += 3) {
				map[8][j] = WALL;
				map[20][j] = WALL;
			}
		}
	}
}

void clearData(void) {
	if (gameOver) {
		score = 0;
	}

	direction = up;
	pause = 0, gameOver = 0, length = STARTLEN, key = UP;
	foodExist = 0, drugExist = 0;
	life = 60;
	for (int i = 0; i < MAPSIZE; i++) {
		for (int j = 0; j < MAPSIZE; j++) {
			map[i][j] = 0;
		}
	}

	struct snake *current, *del;
	current = head;
	while (current != NULL) {
		del = current;
		current = current->next;
		free(del);
	}
	head = tail = NULL;
	FlushMouseMsgBuffer();
}

void initialPrint(void) {     //��ӡ��ʼԪ�ء�������ͼ���������Ϸ��Ϣ
	cleardevice();
	rectangle(19, 19, 581, 581);
	int i, j, x, y;
	struct snake *current;
	for (i = 0; i < MAPSIZE; i++) {       //��ӡ��ͼ
		for (j = 0; j < MAPSIZE; j++) {
			x = 20 * i + 30;
			y = 20 * j + 30;
			if (map[i][j] == WALL) {
				setfillcolor(RED);
				solidrectangle(x - 10, y - 10, x + 10, y + 10);
			}
		}
	}

	for (current = head; current != NULL; current = current->next) {   //��ӡ��ʼ����
		x = 20 * current->x + 30;
		y = 20 * current->y + 30;
		setfillcolor(RGB(180,186,20));
		solidcircle(x, y, 9);
	}
	settextstyle(30, 0, _T("΢���ź�"));
	outtextxy(600, 14, "����Tips:");
	outtextxy(600, 110, "��ǰ�Ѷ�:");
	if (stage == 1) {
		outtextxy(650, 140, "Easy");
	}
	else if (stage == 2) {
		outtextxy(650, 140, "Normal");
	}
	else if (stage == 3) {
		outtextxy(650, 140, "Hard");
	}
	outtextxy(600, 180, "��ǰ����:");
	outtextxy(600, 250, "ͨ������:");
	if (stage == easy) {
		settextstyle(16, 0, _T("����"));
		outtextxy(600, 50, "   ʳ��   ����   ����");
		outtextxy(600, 70, "���� space ����ͣ");
		outtextxy(600, 90, "���޺�ɫǽ�ڵı߽�ɴ�ǽ");
		setfillcolor(BLUE);
		solidcircle(610, 58, 7);
		setfillcolor(GREEN);
		solidcircle(667, 58, 7);
		setfillcolor(BLACK);
		solidcircle(725, 58, 7);
	}
	else if (stage == normal) {
		settextstyle(16, 0, _T("����"));
		outtextxy(590, 50, "���Ե�ը���Ժ�÷ֻ����");
		outtextxy(590, 70, "��ѡ�ó�ը��ʱ���ɶ�÷�");
		outtextxy(590, 90, "���뱣֤�����ȴ�������");
	}
	else if (stage == hard) {
		settextstyle(16, 0, _T("����"));
		outtextxy(590, 50, "������ֵÿ��һ�����һ");
		outtextxy(590, 70, "  ���Ե�ʳ�������");
		outtextxy(590, 90, "���������ͬʱ�����˳Է�");
	}
}

void printGameOver(int a) {
	cleardevice();
	settextstyle(100, 0, "΢���ź�");
	setcolor(RGB(70, 77, 89));
	outtextxy(190, 100, "Game Over");
	outtextxy(295, 200, "�����");
	settextstyle(40, 0, "΢���ź�");
	if (a == WALL) {
		outtextxy(290, 320, "ԭ��ײǽ����");
	}
	else if (a == SNAKE) {
		outtextxy(290, 320, "ԭ����ɱ����");
	}
	else if (a == HUNGRY) {
		outtextxy(290, 320, "ԭ�������ľ�");
	}
	else if (a == TOOSHORT) {
		outtextxy(290, 320, "ԭ������̫��");
	}
	char string[5];
	sprintf_s(string, "%d", score);
	outtextxy(290, 380, "�÷֣�");
	outtextxy(400, 380, string);
	
	Sleep(1500);
	settextstyle(40, 0, "΢���ź�");
	outtextxy(265, 450, "��������������˵�");
	_getch();
}

void getKeyboard(void) {    // ��Ӧ���̲���
	if (_kbhit()) {
		key = _getch();
		if (key == 0xE0 || key == ' ') {
			key = _getch();	
		}
	}
	switch (key) {
	case UP:
		if (direction != down) {
			direction = up;
		} break;
	case DOWN:
		if (direction != up) {
			direction = down;
		} break;
	case LEFT:
		if (direction != right) {
			direction = left;
		} break;
	case RIGHT:
		if (direction != left) {
			direction = right;
		} break;
	case ' ':            // �ո����ͣ
		if (!pause) {
			_getch();
			pause = 1;
		}
	default:
		break;
	}
}

int checkStatus(int x, int y) {     //����ص�ǰ������ʻ��һ����Ҫ���ֵ����ݲ������ж�
	if (life > 0 && length >= 2) {
		if (map[x][y] == WALL)
			return WALL;
		else if (map[x][y] == SNAKE)
			return SNAKE;
		else if (map[x][y] == NOTHING)
			return NOTHING;
		else if (map[x][y] == FOOD)
			return FOOD;
		else if (map[x][y] == BOMB)
			return BOMB;
		else if (map[x][y] == DRUG)
			return DRUG;
	}
	else if (life <= 0)
		return HUNGRY;
	else if (length <= 2)
		return TOOSHORT;
	else return 0;
}

void delNode(void) {       //ɾ��һ��β���
	struct snake *current;
	current = tail;
	tail = tail->previous;
	tail->next = NULL;
	clearcircle(20 * current->x + 30, 20 * current->y + 30, 9);
	map[current->x][current->y] = 0;
	free(current);
}

void addNode(int x, int y) {
	struct snake *newNode;
	newNode = (struct snake *)malloc(sizeof(struct snake));
	head->previous = newNode;
	newNode->next = head;
	newNode->previous = NULL;
	head = newNode;
	head->x = x;
	head->y = y;
	if (map[x][y] != WALL) {
		map[x][y] = SNAKE;
		setfillcolor(RGB(180, 186, 20));
		solidcircle(20 * x + 30, 20 * y + 30, 9);
	}
}
 
void setFood(void) {
	if (!foodExist) {
		foodExist = 1;
		srand((unsigned)time(NULL));
		food.x = rand() % 26 + 1;
		food.y = rand() % 26 + 1;

		while (map[food.x][food.y]) {  //������Ϊ0ʱ˵����ͼ����һ��Ϊ�գ�����ʳ���������ѭ����-
			food.x = rand() % 26 + 1;
			food.y = rand() % 26 + 1;
		}
		map[food.x][food.y] = FOOD;
		setfillcolor(BLUE);
		solidcircle(20 * food.x + 30, 20 * food.y + 30, 7);
	}
}

void setDrug(int stage) {
	int i;
	srand((unsigned)time(NULL));
	for (i = 0; i < 3*stage; i++) {
		drug[i].x = (rand() % 26) + 1;
		drug[i].y = (rand() % 26) + 1;
		while (map[drug[i].x][drug[i].y]) {
			drug[i].x = rand() % 26 + 1;
			drug[i].y = rand() % 26 + 1;
		}
		map[drug[i].x][drug[i].y] = DRUG;
		setfillcolor(GREEN);
		solidcircle(20 * drug[i].x + 30, 20 * drug[i].y + 30, 7);
	}
	drugExist = 1;
}

void delDrug(int stage) {
	int i;
	for (i = 0; i < 3*stage; i++) {
		map[drug[i].x][drug[i].y] = NOTHING;
		clearcircle(20 * drug[i].x + 30, 20 * drug[i].y + 30, 7);
		drug[i].x = 0;
		drug[i].y = 0;
	}
	drugExist = 0;
}

void drugTwinkle(void)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3 * stage; j++)
		{
			clearcircle(20 * drug[j].x + 30, 20 * drug[j].y + 30, 7);
		}
		Sleep(30);
		for (int j = 0; j < 3 * stage; j++)
		{
			setfillcolor(GREEN);
			solidcircle(20 * drug[j].x + 30, 20 * drug[j].y + 30, 7);
		}

	}
}

void setBomb(int stage) {
	int i;
	srand((unsigned)time(NULL));
	for (i = 0; i < 3*stage; i++) {
		bomb[i].x = (rand() % 26) + 1;
		bomb[i].y = (rand() % 26) + 1;
		while (map[bomb[i].x][bomb[i].y]) { 
			bomb[i].x = rand() % 26 + 1;
			bomb[i].y = rand() % 26 + 1;
		}
		map[bomb[i].x][bomb[i].y] = BOMB;
		setfillcolor(BLACK);
		solidcircle(20 * bomb[i].x + 30, 20 * bomb[i].y + 30, 7);
	}
	remainingBomb = 3 * stage;
}

void move(void) {       //���ݵ�ǰ�����ƶ�������Ӧ�жϽ��
	int x, y;
	x = head->x;
	y = head->y;
	switch (direction) {
	case up:
		y = (y + 27) % 28;
		break;
	case down:
		y = (y + 29) % 28; 
		break;
	case left:  
		x = (x + 27) % 28; 
		break;
	case right: x = (x + 29) % 28;
		break;
	default:
		break;
	}
	if (stage == hard) {
		if (checkStatus(x, y) == FOOD) {
			life = 60;
		}
		else life--;
	}
	if (checkStatus(x, y) == NOTHING) {
		map[head->x][head->y] = 2;
		addNode(x, y);          //����һ��ͷ��㣬ɾ��һ��β��㡣��ʾ��ǰ��
		delNode();
	}
	else if (checkStatus(x, y) == FOOD) {
		addNode(x, y);          //���Ե�ʳ��ʱ��ֻ���Ӳ�ɾ��	
		foodExist = 0;
		++length;
		score += 10;
	}
	else if (checkStatus(x, y) == BOMB) {
		int i;
		addNode(x, y);
		delNode();
		for (i = 1; i <= (length+1) / 2; i++) {
			delNode();
		}
		length = length / 2;
		score = score / 2;
		remainingBomb--;
	}
	else if (checkStatus(x, y) == DRUG) {

		addNode(x, y);
		delNode();
		delNode();
		--length;
		score -= 10;
	}
	else if(checkStatus(x,y) == WALL) {
		gameOver = 1;
		printGameOver(WALL);
	}
	else if (checkStatus(x, y) == SNAKE) {
		gameOver = 1;
		printGameOver(SNAKE);
	}
	else if (checkStatus(x, y) == HUNGRY) {
		gameOver = 1;
		printGameOver(HUNGRY);
	}
	else if (checkStatus(x, y) == TOOSHORT) {
		gameOver = 1;
		printGameOver(TOOSHORT);
	}

}

void printGameInfo(void) {
	char string1[10],string2[10],string3[10];
	if (score < 10) {
		sprintf_s(string1, "00%d", score);
	}
	else if (score < 100) {
		sprintf_s(string1, "0%d", score);
	}
	else {
		sprintf_s(string1, "%d", score);
	}
	settextstyle(30, 0, "΢���ź�");
	outtextxy(650, 215, string1);
	settextstyle(22, 0, "΢���ź�");
	outtextxy(620, 290, "������ﵽ20��");
	outtextxy(630, 315, "������ ");
	if (length < 10) {
		sprintf_s(string2, "0%d / 20", length);
	}
	else {
		sprintf_s(string2, "%d / 20", length);
	}
	outtextxy(700, 315, string2);
	if (stage == normal || stage == hard) {
		outtextxy(620, 355, "���������ը��");
		outtextxy(630, 380, "ʣ��ը�� ");
		if (remainingBomb < 10) {
			sprintf_s(string3, "0%d / 0%d", remainingBomb, 3 * stage);
		}
		else {
			sprintf_s(string3, "%d / 0%d", remainingBomb, 3 * stage);
		}
		outtextxy(700, 380, string3);
	}
	if (stage == hard) {
		outtextxy(620, 420, "��ȷ���߲������� ");
		outtextxy(630, 445, "����ֵ");
		if (life < 10) {
			sprintf_s(string3, "0%d / 60", life);
		}
		else {
			sprintf_s(string3, "%d / 60", life);
		}
		outtextxy(700, 445, string3);

	}
}

int win(int stage){
	if (stage == easy) {
		if (length == 20)
			return 1;
		else return 0;
	}
	else {
		if (length == 20 && remainingBomb == 0)
			return 1;
		else return 0;
	}
}

void congratulation(int stage) {
	cleardevice();
	if (stage != hard) {
		settextstyle(80, 0, "΢���ź�");
		setcolor(RGB(70, 77, 89));
		outtextxy(260, 200, "��ϲͨ��");
		settextstyle(40, 0, "΢���ź�");
		outtextxy(285, 450, "3��������һ��");
	}
	else
	{
		outtextxy(260, 200, "��ϲͨȫ��");
		outtextxy(285, 450, "3��󷵻����˵�");
	}
	gameOver = 0;
	Sleep(3000);
}

void gotoNextStage(void) {
	if (win(stage)) {
		if (stage == easy) {
			congratulation(easy);
			clearData();
			stage = normal;
		}
		else if (stage == normal) {
			congratulation(normal);
			clearData();
			stage = hard;
		}
		else if (stage == hard) {
			congratulation(hard);
			clearData();
			stage = allClear;
		}
	}
}

void gamePlay(int stage) {
	int sleepTime;
	switch (stage) {
	case easy:
		sleepTime = 200;
		break;
	case normal:
		sleepTime = 100;
		break;
	case hard:
		sleepTime = 50;
		break;
	default:
		break;
	}
	setFood();
	setBomb(stage);
	Sleep(2000);
	while (1) {
		setFood();
		if (time(NULL) % 10 == 1 && !drugExist) {
			setDrug(stage);
		}
		else if (time(NULL) % 10 == 9 && drugExist) {
			delDrug(stage);
		}
		if (drugExist)
			drugTwinkle();
		else Sleep(90);

		getKeyboard();
		move();
		if (gameOver || win(stage))
			break;
		printGameInfo();
		Sleep(sleepTime);
	}
}

int main(void) 
{ 
	initgraph(800, 600);
	int menuSelection;
	while(1)
	{
		cleardevice();
		menuSelection = mainMenu();
		switch (menuSelection) 
		{
		case NEWGAME:
			selectStage();
			while (!gameOver && stage != allClear) 
			{
				initMap(stage);
				initSnake();
				initialPrint();
				gamePlay(stage);
				gotoNextStage();
			}
			break;
		case LOADGAME:
			loadGame();
			break;
		case RANKINGLIST:
			printrankingList();
			break;
		case EXIT:
			closegraph();
			exit(0);
			break;
		default:
			break;
		}
		clearData();
	}
	return 0;
 }