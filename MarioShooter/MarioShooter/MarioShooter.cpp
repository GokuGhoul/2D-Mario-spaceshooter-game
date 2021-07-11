// MarioShooter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//UCS505 Computer Graphics Project Code On 2D-Mario Game
//Submitted to Ms. Archana
/*Submited by.-
*Name no.- Aditya Makhija
*Roll no.- 401803002
*Batch.- Coe-30
*Name no.- Devansh Pahuja
*Roll no.- 401803008
*Batch.- Coe-30
*/

//Code
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <vector>
#include <time.h>
#include <string>
#define Total_defense_squares 20
// Declaration of global variables
GLfloat missile1_y = 0, missile2_y = 0;
GLfloat plane_x = 0, plane_y = 0, missile1_tx = 0, missile2_tx = 0;
//defense
struct def
{
	GLfloat b, x, y;
};
//enemy
struct enemy
{
	GLfloat x, y;
	int type;
};
bool initialized = false, paused; //initialized flag to start variables, pause brand end game
int score = 0, lives = 3;
std::vector<struct def> defense; // contains the defenses
std::vector<struct enemy> invaders; //contains the invaders
std::vector<struct enemy> shots;
bool missile1_moving = false, missile2_moving = false; //starts with stopped missiles
int msec_missile1 = 0, msec_missile2 = 0;
//Initializes positions of defenses and enemies
// this function initializes the vector of defences(contains coordinates for drawing them) and invaders(contains coordinates and type)
void gameOver() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.6, 0.6, 1, 0);
	glLoadIdentity();
}
void initialize()
{
	struct def temp_defense;
	struct enemy temp_invader;
	srand(time(NULL));
	// creating a array for positions of defenses
	// a single shield is made of five squares hence there are four shields each of five rectangles
	// 2 3 4
	// 1 5
	for (int i = 0, k = -13.5; i < Total_defense_squares / 5; i++, k += 8)
	{
		temp_defense.b = 1.0f;
		temp_defense.x = k;
		temp_defense.y = 2;
		defense.push_back(temp_defense); //1
		temp_defense.y = 3;
		defense.push_back(temp_defense); //2
		temp_defense.x = k + 1;
		defense.push_back(temp_defense); //3
		temp_defense.x = k + 2;
		defense.push_back(temp_defense); //4
		temp_defense.y = 2;
		defense.push_back(temp_defense); //5
	}
	for (int i = 0; i < 5; i++)
	{
		for (float j = 0, k = -13; j < 10; j++, k += 2.5)
		{
			temp_invader.type = i + 1; //type of invaders currently there are 3 types
			temp_invader.x = k;
			temp_invader.y = 25.5 - i * 2.5;
			invaders.push_back(temp_invader);
		}
	}
}
//move the enemies
void move_invader(int step)
{
	if (paused) //checking whether game is over or not
		return;
	bool overflow = false; //flag in case it touches corners
	for (int i = 0; i < invaders.size(); i++) //checking whether are touching or not
	{
		if (invaders[i].x + (1.0 * step) / 100 > 18)
		{
			overflow = true;
			break;
		}
		if (invaders[i].x + (1.0 * step) / 100 < -20)
		{
			overflow = true;
			break;
		}
	}
	if (!overflow)
	{ // if they are not touching corners then move them
		for (int i = 0; i < invaders.size(); i++)
		{
			invaders[i].x += (1.0 * step) / 100;
		}
		glutPostRedisplay();
		glutTimerFunc(750 + 2 * (step > 0 ? -step : step), move_invader, step);
	}
	else
	{ //if leaned down and changes the direction of movement on the next call
		for (int i = 0; i < invaders.size(); i++)
		{
			invaders[i].y--;
		}
		glutPostRedisplay();
		if (step > 0) // changing the direction
		{
			glutTimerFunc(750 - 2 * step, move_invader, -(step + 5));
		}
		else
		{
			glutTimerFunc(750 + 2 * step, move_invader, -(step - 5));
		}
	}
}
//Used to write text, arguments are the string, string size, positioning x, posing Y,
//Source size (used to split the original size) and color in RGB
void drawText(const char* text, int length, int x, int y, float size, float r, float g, float b)
{
	glMatrixMode(GL_PROJECTION);
	double* matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	glOrtho(0, 800, 0, 600, -5, 5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	glColor3f(r, g, b);
	glTranslatef(x, y, 0.0f);
	glScalef(1.0f * size, 1.0f * size, 1.0f);
	glRasterPos2i(x, y);
	for (int i = 0; i < length; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, (int)text[i]);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);
}
//Do every shot of the enemy move
void move_shot(int step)
{
	for (int i = 0; i < shots.size(); i++)
	{
		shots[i].y -= (1.1 * step) / 100;
	}
	glutPostRedisplay();
	glutTimerFunc(5, move_shot, step);
}
// move missile 1
void move_missile1(int step)
{
	if (!paused)
	{
		if (!missile1_moving)
			return;
		missile1_y += (1.0 * step) / 100;
		glutPostRedisplay();
		glutTimerFunc(5, move_missile1, step);
	}
}
// move missile 2
void move_missile2(int step)
{
	if (!paused)
	{
		if (!missile2_moving)
			return;
		missile2_y += (1.1 * step) / 100;
		glutPostRedisplay();
		glutTimerFunc(5, move_missile2, step);
	}
}
// FUNCTION TO Draw the basis of the object

void bricks_line_horizontal(int x, int y1, int y2) {
	glBegin(GL_LINES);
	glLineWidth(3);
	glColor3f(0, 0, 0);
	glVertex2f(x, y1);
	glVertex2f(x, y2);
	glEnd();
}

void bricks_line_in_vertical(int x1, int x2, int y) {
	glBegin(GL_LINES);
	glLineWidth(3);
	glColor3f(0, 0, 0);
	glVertex2f(x1, y);
	glVertex2f(x2, y);
	glEnd();
}

void ground()
{
	glColor3f(0.9f, 0.5f, 0.2f);
	glBegin(GL_POLYGON);
	glVertex2f(-20.0, -5.0);
	glVertex2f(-18.0, -5.0);
	glVertex2f(-18.0, -10.0);
	glVertex2f(-20.0, -10.0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex2f(18.0, -10.0);
	glVertex2f(18.0, -5.0);
	glVertex2f(20.0, -5.0);
	glVertex2f(20.0, -10.0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex2f(-19.0, -8.0);
	glVertex2f(19.0, -8.0);
	glVertex2f(19.0, -10.0);
	glVertex2f(-19.0, -10.0);
	glEnd();

	//horizontal line in horizontal block
	glBegin(GL_LINES);
	glLineWidth(3);
	glColor3f(0, 0, 0);
	glVertex2f(-18, -9);
	glVertex2f(18, -9);
	glEnd();

	//vertical in horzontal block
	for (int x = -19; x <= 19; x++) {
		bricks_line_horizontal(x, -8, -10);
	}

	//vertical line in firstblock
	glBegin(GL_LINES);
	glLineWidth(3);
	glColor3f(0, 0, 0);
	glVertex2f(-19, -5);
	glVertex2f(-19, -10);
	glEnd();

	//horizontal line in firstblock
	for (int y = -10; y <= -5; y++) {
		bricks_line_in_vertical(-20, -18, y);
	}

	//vertical line in second vertical block
	glBegin(GL_LINES);
	glLineWidth(3);
	glColor3f(0, 0, 0);
	glVertex2f(19, -5);
	glVertex2f(19, -10);
	glEnd();

	//horizontal line in second block
	for (int y = -10; y <= -5; y++) {
		bricks_line_in_vertical(20, 18, y);
	}
}
void Mario()
{
	glLineWidth(2);
	glColor3f(0.4f, 0.6f, 0.0f);
	glBegin(GL_POLYGON); //arms
	glVertex2f(-0.1, 2);
	glVertex2f(2.7, 2);
	glVertex2f(1.3, 3.8);
	glEnd();
	glBegin(GL_POLYGON); //hair
	glVertex2f(0.1, 3);
	glVertex2f(0.5, 3);
	glVertex2f(0.5, 3.9);
	glVertex2f(0.1, 3.9);
	glEnd();
	glBegin(GL_POLYGON); //hair
	glVertex2f(0.2, 3.9);
	glVertex2f(1.1, 3.9);
	glVertex2f(1.1, 4.5);
	glVertex2f(0.2, 4.5);
	glEnd();
	glColor3f(0.9f, 0.8f, 0.1f);
	glBegin(GL_POLYGON); //handsleft
	glVertex2f(0, 1);
	glVertex2f(0.8, 1);
	glVertex2f(0.8, 2);
	glVertex2f(-0.1, 2);
	glEnd();
	glBegin(GL_POLYGON); //handsright
	glVertex2f(1.7, 1);
	glVertex2f(2.7, 1);
	glVertex2f(2.7, 2);
	glVertex2f(1.8, 2);
	glEnd();
	glBegin(GL_POLYGON); //head
	glVertex2f(0.5, 2.8);
	glVertex2f(2.3, 2.8);
	glVertex2f(2.3, 3.3);
	glVertex2f(0.5, 3.3);
	glEnd();
	glBegin(GL_POLYGON); //nose
	glVertex2f(0.5, 3.1);
	glVertex2f(2.6, 3.1);
	glVertex2f(2.6, 4);
	glVertex2f(0.5, 4);
	glEnd();
	glBegin(GL_POLYGON); //nose extension
	glVertex2f(0.3, 3.3);
	glVertex2f(2.8, 3.3);
	glVertex2f(2.8, 3.6);
	glVertex2f(0.3, 3.6);
	glEnd();
	glBegin(GL_POLYGON); //ear extension
	glVertex2f(0.3, 3.3);
	glVertex2f(0.5, 3.3);
	glVertex2f(0.5, 3.9);
	glVertex2f(0.3, 3.9);
	glEnd();
	glBegin(GL_POLYGON); //top
	glVertex2f(0.7, 3.9);
	glVertex2f(2.2, 3.9);
	glVertex2f(2.2, 4.5);
	glVertex2f(0.7, 4.5);
	glEnd();
	glColor3f(0.4f, 0.6f, 0.0f);
	glBegin(GL_POLYGON); //legs
	glVertex2f(-0.1, 0.0);
	glVertex2f(0.8, 0.0);
	glVertex2f(0.8, 0.4);
	glVertex2f(-0.1, 0.4);
	glEnd();
	glBegin(GL_POLYGON); //legs
	glVertex2f(1.9, 0.0);
	glVertex2f(2.8, 0.0);
	glVertex2f(2.8, 0.4);
	glVertex2f(1.9, 0.4);
	glEnd();
	glBegin(GL_POLYGON); //eyes
	glVertex2f(1.6, 4.5);
	glVertex2f(1.9, 4.5);
	glVertex2f(1.9, 3.9);
	glVertex2f(1.6, 3.9);
	glEnd();
	glBegin(GL_POLYGON); //mouth
	glVertex2f(1.6, 3.1);
	glVertex2f(2.6, 3.1);
	glVertex2f(2.6, 3.4);
	glVertex2f(1.6, 3.4);
	glEnd();
	glBegin(GL_POLYGON); //mouth
	glVertex2f(1.9, 3.4);
	glVertex2f(2.2, 3.4);
	glVertex2f(2.2, 3.8);
	glVertex2f(1.9, 3.8);
	glEnd();
	glColor3f(0.5f, 0.0f, 0.0f);
	glBegin(GL_POLYGON); //upperlegs
	glVertex2f(0.2, 0.4);
	glVertex2f(1.0, 0.4);
	glVertex2f(1.0, 0.8);
	glVertex2f(0.2, 0.8);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex2f(1.6, 0.4);
	glVertex2f(2.4, 0.4);
	glVertex2f(2.4, 0.8);
	glVertex2f(1.6, 0.8);
	glEnd();
	glColor3f(0.5f, 0.0f, 0.0f);
	glBegin(GL_POLYGON); //stomach
	glVertex2f(0.5, 0.6);
	glVertex2f(2.1, 0.6);
	glVertex2f(2.1, 1.5);
	glVertex2f(0.5, 1.5);
	glEnd();
	glBegin(GL_POLYGON); //chest
	glVertex2f(0.7, 1.5);
	glVertex2f(1.9, 1.5);
	glVertex2f(1.9, 1.8);
	glVertex2f(0.7, 1.8);
	glEnd();
	glBegin(GL_POLYGON); //l-ribchest
	glVertex2f(0.7, 1.5);
	glVertex2f(1.1, 1.5);
	glVertex2f(1.1, 2.8);
	glVertex2f(0.7, 2.8);
	glEnd();
	glBegin(GL_POLYGON); //r-ribonchest
	glVertex2f(1.5, 1.5);
	glVertex2f(1.9, 1.5);
	glVertex2f(1.9, 2.5);
	glVertex2f(1.5, 2.5);
	glEnd();
	glBegin(GL_POLYGON); //midchest
	glVertex2f(1, 1.5);
	glVertex2f(1.5, 1.5);
	glVertex2f(1.5, 2.0);
	glVertex2f(1, 2.0);
	glEnd();
	glBegin(GL_POLYGON); //hat
	glVertex2f(0.3, 4.5);
	glVertex2f(2.6, 4.5);
	glVertex2f(2.6, 4.8);
	glVertex2f(0.3, 4.8);
	glEnd();
	glBegin(GL_POLYGON);//hat
	glVertex2f(0.5, 4.8);
	glVertex2f(2, 4.8);
	glVertex2f(2, 5);
	glVertex2f(0.5, 5);
	glEnd();
	glColor3f(0.9f, 0.8f, 0.1f);
	glBegin(GL_POLYGON); //buttonl
	glVertex2f(0.8, 1.5);
	glVertex2f(1.1, 1.5);
	glVertex2f(1.1, 1.2);
	glVertex2f(0.8, 1.2);
	glEnd();
	glBegin(GL_POLYGON); //buttonr
	glVertex2f(1.5, 1.5);
	glVertex2f(1.8, 1.5);
	glVertex2f(1.8, 1.2);
	glVertex2f(1.5, 1.2);
	glEnd();
}
//enemy draws more up
// drawing triangles and rectangles to create a enemy
void Draw_enemy1(struct enemy inv)
{
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(inv.x, inv.y + 1.2); //left side head
	glVertex2f(inv.x, inv.y + 2);
	glVertex2f(inv.x + 1, inv.y + 2);
	glVertex2f(inv.x + 2, inv.y + 1.2); //right head
	glVertex2f(inv.x + 2, inv.y + 2);
	glVertex2f(inv.x + 1, inv.y + 2);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(inv.x - 0.1, inv.y); // mouth
	glVertex2f(inv.x + 1, inv.y + 0.9);
	glVertex2f(inv.x + 2, inv.y);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS); // eyes
	glVertex2f(inv.x + 0.5, inv.y + 1.3); //left
	glVertex2f(inv.x + 0.5, inv.y + 1.8);
	glVertex2f(inv.x + 0.7, inv.y + 1.8);
	glVertex2f(inv.x + 0.7, inv.y + 1.3);
	glVertex2f(inv.x + 1.3, inv.y + 1.3); //right
	glVertex2f(inv.x + 1.3, inv.y + 1.8);
	glVertex2f(inv.x + 1.5, inv.y + 1.8);
	glVertex2f(inv.x + 1.5, inv.y + 1.3);
	glEnd();
}
//draw middle enemy
void Draw_enemy2(struct enemy inv)
{
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(inv.x - 0.2, inv.y + 2); //hole forehead
	glVertex2f(inv.x + 2.2, inv.y + 2);
	glVertex2f(inv.x + 1, inv.y + 1.4);
	glVertex2f(inv.x, inv.y + 0.9); // left head side
	glVertex2f(inv.x, inv.y + 1.7);
	glVertex2f(inv.x + 0.6, inv.y + 1.4);
	glVertex2f(inv.x + 2, inv.y + 0.9); // right side head
	glVertex2f(inv.x + 2, inv.y + 1.7);
	glVertex2f(inv.x + 1.4, inv.y + 1.4);
	glEnd();
	glBegin(GL_QUADS); // eyes
	glVertex2f(inv.x + 0.5, inv.y + 0.7); //left
	glVertex2f(inv.x + 0.5, inv.y + 0.9);
	glVertex2f(inv.x + 0.7, inv.y + 0.9);
	glVertex2f(inv.x + 0.7, inv.y + 0.7);
	glVertex2f(inv.x + 1.3, inv.y + 0.7); //right
	glVertex2f(inv.x + 1.3, inv.y + 0.9);
	glVertex2f(inv.x + 1.5, inv.y + 0.9);
	glVertex2f(inv.x + 1.5, inv.y + 0.7);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(inv.x + 0.9, inv.y + 0.3); //mustache
	glVertex2f(inv.x + 0.9, inv.y + 0.5);
	glVertex2f(inv.x + 1.1, inv.y + 0.5);
	glVertex2f(inv.x + 1.1, inv.y + 0.3);
	glEnd();
}
//draw the enemy lower down
void Draw_enemy3(struct enemy inv)
{
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(inv.x, inv.y); //mouth
	glVertex2f(inv.x + 2, inv.y);
	glVertex2f(inv.x + 1, inv.y + 0.9);
	glVertex2f(inv.x, inv.y + 1.5); // left head side
	glVertex2f(inv.x, inv.y + 2);
	glEnd();
	glBegin(GL_QUADS); // eyes
	glVertex2f(inv.x + 0.5, inv.y + 1.3); //left
	glVertex2f(inv.x + 0.5, inv.y + 1.5);
	glVertex2f(inv.x + 0.8, inv.y + 1.5);
	glVertex2f(inv.x + 0.8, inv.y + 1.3);
	glVertex2f(inv.x + 1.2, inv.y + 1.3); //right
	glVertex2f(inv.x + 1.2, inv.y + 1.5);
	glVertex2f(inv.x + 1.5, inv.y + 1.5);
	glVertex2f(inv.x + 1.5, inv.y + 1.3);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(inv.x + 0.8, inv.y + 0.4); //mustache
	glVertex2f(inv.x + 0.8, inv.y + 0.6);
	glVertex2f(inv.x + 1.2, inv.y + 0.6);
	glVertex2f(inv.x + 1.2, inv.y + 0.4);
	glEnd();
}
//draws the incoming enemy, calling the correct function for each type
void Draw_Enemies_Wrapper(struct enemy inv)
{
	if (inv.type >= 4)
	{
		glColor3f(0.6f, 0.6f, 0.6f);
		glBegin(GL_QUADS);
		glVertex2f(inv.x, inv.y);
		glVertex2f(inv.x, inv.y + 2);
		glVertex2f(inv.x + 2, inv.y + 2);
		glVertex2f(inv.x + 2, inv.y);
		glEnd();
		Draw_enemy3(inv);
	}
	else if (inv.type >= 2)
	{
		glColor3f(0.0f, 0.6f, 0.6f);
		glBegin(GL_QUADS);
		glVertex2f(inv.x, inv.y);
		glVertex2f(inv.x, inv.y + 2);
		glVertex2f(inv.x + 2, inv.y + 2);
		glVertex2f(inv.x + 2, inv.y);
		glEnd();
		Draw_enemy2(inv);
	}
	else
	{
		glColor3f(0.6f, 0.1f, 0.1f);
		glBegin(GL_QUADS);
		glVertex2f(inv.x, inv.y);
		glVertex2f(inv.x, inv.y + 2);
		glVertex2f(inv.x + 2, inv.y + 2);
		glVertex2f(inv.x + 2, inv.y);
		glEnd();
		Draw_enemy1(inv);
	}
}
//draws the defense
void Draw_Defense(struct def defense)
{
	glColor3f(0.0f, 0.0f, defense.b);
	glBegin(GL_QUADS);
	glVertex2f(defense.x, defense.y);
	glVertex2f(defense.x, defense.y + 1);
	glVertex2f(defense.x + 1, defense.y + 1);
	glVertex2f(defense.x + 1, defense.y);
	glEnd();
}
//draw the enemy's shot
void Draw_EnemyShot(GLfloat x, GLfloat y)
{
	glColor3f(1.0f, 0.0f, 0.1f);
	glLineWidth(2);
	glBegin(GL_QUADS);
	glVertex2f(x - 0.1f, y);
	glVertex2f(x + 0.1f, y);
	glVertex2f(x + 0.1f, y + 0.4f);
	glVertex2f(x - 0.1f, y + 0.4f);
	glEnd();
}
//draw the player's missiles
void Draw_Player_Missile()
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(2);
	glBegin(GL_POLYGON);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(-1.0f, -0.7f);
	glVertex2f(-0.9f, -0.6f);
	glVertex2f(-0.8f, -0.7f);
	glVertex2f(-0.8f, -1.0f);
	glEnd();
}
// reset the m missile to be able to shoot again
void Reset_Missile(int m)
{
	if (m == 1)
	{
		missile1_y = 0;
		missile1_moving = false;
		missile1_tx = 0;
	}
	else
	{
		missile2_y = 0;
		missile2_moving = false;
		missile2_tx = 0;
	}
}
// given an enemy (alien) makes him shoot
void enemy(int alien)
{
	shots.push_back(invaders[alien]);
}
// choose an enemy randomly every 4 sec
// it calls faster with time until it reaches 1 per second
void Choose_Alien(int step)
{
	if (!paused) {
		if (invaders.size() == 0)
			return;
		int alien = rand() % invaders.size();
		int alien2 = rand() % invaders.size();
		enemy(alien);
		enemy(alien2);
		glutTimerFunc((4000 - step > 1000 ? 4000 - step : 1000) / 5, Choose_Alien, step + 30);
	}
}
// View Window Redraw callback function
void Draw(void)
{
	// Switch to model coordinate system
	glMatrixMode(GL_MODELVIEW);
	// Initializes the current transformation matrix
	glLoadIdentity();
	// Clear the preview window with color
	// background defined previously
	glClear(GL_COLOR_BUFFER_BIT);
	ground();
	//write the score on the screen
	std::string pts = ("score ");
	pts += std::to_string(score);
	drawText(pts.c_str(), pts.length(), 0, 570, 0.2, 1, 1, 1);
	//write how many lives on the screen
	pts = ("lives ");
	pts += std::to_string(lives);
	drawText(pts.c_str(), pts.length(), 0, 550, 0.2, 1, 1, 1);
	// have the defenses drawn
	for (int i = 0; i < defense.size(); i++)
	{
		Draw_Defense(defense[i]);
	}
	// have the invaders drawn
	for (int i = 0; i < invaders.size(); i++)
	{
		Draw_Enemies_Wrapper(invaders[i]);
		if (invaders[i].y < 2) //if invaders too close(stepu defenses) game over
			paused = true;
	}
	//see if the shots given by the intruders hit something
	for (int i = 0; i < shots.size(); i++)
	{
		if (i < shots.size() && shots[i].y <= 4)
		{ //distance at which the defenses are, so check them
			for (int j = 0; j < defense.size(); j++)
			{
				if ((shots[i].x - defense[j].x >= 0 && shots[i].x - defense[j].x <= 1) && shots[i].y -
					defense[j].y <= 1)
				{
					shots.erase(shots.begin() + i);
					defense[j].b -= 0.25f;
					if (defense[i].b <= 0)
					{
						defense.erase(defense.begin() + i);
					}
					break;
				}
			}
		}
		if (i < shots.size() && shots[i].y - plane_y <= 0 && shots[i].y - plane_y > -2 && (shots[i].x - plane_x > -1
			&& shots[i].x - plane_x < 1))
		{ //distance where the player is, check it
			lives--;
			paused = (lives > 0) ? false : true;
			shots.erase(shots.begin() + i);
		}
		if (i < shots.size() && shots[i].y <= -9)
		{
			shots.erase(shots.begin() + i);
		}
		if (i < shots.size())
			Draw_EnemyShot(shots[i].x, shots[i].y);
	}
	//if "paused" writes game over on the screen
	//if there are no more enemies write you win
	if (paused)
	{
		gameOver();
		{
			drawText("Game Over", 9, 200, 350, 0.5, 0.1, 0, 0.1);
			/*drawText("Press-1 To Restart", 20, 50, 570, 0.2, 1, 1, 1);*/
			drawText("Press-2 To Quit", 15, 50, 500, 0.2, 1, 1, 1);
		}
	}
	if (invaders.size() == 0)
	{
		drawText("You Win", 7, 230, 250, 0.5, 1, 1, 1);
	}
	glTranslatef(plane_x, plane_y, 0.0f);
	glTranslatef(0.0f, -0.7f, 0.0f);
	glPushMatrix();
	// see if the shot left the scene
	if (missile1_y > 30)
	{
		Reset_Missile(1);
	}
	if (missile2_y > 30)
	{
		Reset_Missile(2);
	}
	// see if the shot hit the defense itself

	for (int i = 0; i < defense.size(); i++)
	{
		if ((missile1_tx - defense[i].x >= 1 && missile1_tx - defense[i].x <= 2) && (missile1_y
	> defense[i].y + 1.5))
		{
			defense[i].b -= 0.25f;
			Reset_Missile(1);
			glutPostRedisplay();
			if (defense[i].b <= 0)
			{
				defense.erase(defense.begin() + i);
			}
		}
		else if ((missile2_tx - defense[i].x >= -1 && missile2_tx - defense[i].x <= 0) &&
			(missile2_y > defense[i].y + 1.5))
		{
			defense[i].b -= 0.25f;
			Reset_Missile(2);
			glutPostRedisplay();
			if (defense[i].b <= 0)
			{
				defense.erase(defense.begin() + i);
			}
		}
	}
	//see if the shot hit an invader
	for (int i = 0; i < invaders.size(); i++)
	{
		if ((missile1_tx - invaders[i].x >= 1 && missile1_tx - invaders[i].x <= 3) &&
			(missile1_y > invaders[i].y + 1.5))
		{
			Reset_Missile(1);
			glutPostRedisplay();
			score += invaders[i].type > 3 ? 10 : (invaders[i].type > 1 ? 20 : 40);
			invaders.erase(invaders.begin() + i);
		}
		else if ((missile2_tx - invaders[i].x >= -1 && missile2_tx - invaders[i].x <= 1) &&
			(missile2_y > invaders[i].y + 1.5))
		{
			Reset_Missile(2);
			glutPostRedisplay();
			score += invaders[i].type > 3 ? 10 : (invaders[i].type > 1 ? 20 : 40);
			invaders.erase(invaders.begin() + i);
		}
	}
	if (missile2_moving)
	{
		glTranslatef(-plane_x, -plane_y, 0.0f);
		glTranslatef(missile2_tx, 0.0f, 0.0f);
	}
	//Missile 2;
	glTranslatef(2.7f, -0.1f, 0.0f);
	glTranslatef(0.0f, missile2_y, 0.0f);
	Draw_Player_Missile();
	glPopMatrix(); // Load identity = Clear transformation matrix.
	glPushMatrix();
	if (missile1_moving)
	{
		glTranslatef(-plane_x, -plane_y, 0.0f);
		glTranslatef(missile1_tx, 0.0f, 0.0f);
	}
	//Missile 1.
	glTranslatef(-0.1f, -0.1f, 0.0f);
	glTranslatef(-0.2f, missile1_y, 0.0f);
	Draw_Player_Missile();
	glPopMatrix(); //For the Mario not to leave with the missile 1.
	// Design the Mario.

	glTranslatef(-1.0f, -3.0, 0.0f);
	Mario();
	// Execute OpenGL commands
	glFlush();
}
// Callback function called to handle special key events
void Special_keys(int key, int x, int y)
{
	// move the base
	if (key == GLUT_KEY_LEFT && !paused)
	{
		plane_x -= 0.5f;
		if (plane_x < -17)
			plane_x = -17;
	}
	if (key == GLUT_KEY_RIGHT && !paused)
	{
		plane_x += 0.5f;
		if (plane_x > 16)
			plane_x = 16;
	}
	if (key == GLUT_KEY_UP && !paused)
	{
		plane_y += 0.5f;
		if (plane_y > 30)
			plane_y = 30;
	}
	if (key == GLUT_KEY_DOWN && !paused)
	{
		plane_y -= 0.5f;
		if (plane_y < -4.5)
			plane_y = -4.5;
	}
	glutPostRedisplay();
}
// Callback function called to handle key events
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		//case '1':
		//	if (paused)
		//		Initialize();
		//	break;
	case '2':
		if (paused)
			exit(0);
		break;
	case 27:
		exit(0);
	}
	if (key == 'f' && missile1_moving != true)
	{
		missile1_moving = true;
		missile1_tx = plane_x;
		missile1_y = plane_y;
		glutTimerFunc(10, move_missile1, 20);
	}
	if (key == 'f' && missile2_moving != true)
	{
		missile2_moving = true;
		missile2_tx = plane_x;
		missile2_y = plane_y;
		glutTimerFunc(10, move_missile2, 20);
	}
}
// Responsible function for initializing parameters and variables
void Initialize(void)
{
	// Set viewport background color to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	gluOrtho2D(-20, 20, -10, 31);
	glViewport(0, 0, 800, 600);
}
// Main Program
void main(int argc, char* argv[])
{
	glutInit(&argc, argv); // Start an instance of glut
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(5, 5);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Mario-Shooter");
	if (!initialized)
	{
		initialize();
		initialized = true;
	}
	glutTimerFunc(0, move_invader, 60); // make the call for enemies to move
	glutTimerFunc(0, Choose_Alien, 100); // make the call to choose which to shoot
	move_shot(10); // make the shot move
	// Register viewport redraw callback function
	glutDisplayFunc(Draw);
	glMatrixMode(GL_PROJECTION);
	// Register the callback function for handling special keys
	glutSpecialFunc(Special_keys);
	// Register the callback function for handling ASCII keys
	glutKeyboardFunc(Keyboard);
	// Call the responsible for making the initializations
	Initialize();
	glutTimerFunc(0, move_missile1, 0); // Timer to move missile 1
	glutTimerFunc(0, move_missile2, 0); //..........................2
	// Start processing and wait for user interactions
	glutMainLoop();
}