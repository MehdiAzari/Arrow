#include "SBDL.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

#define startingSpeed 1;
using namespace std;

bool startStatus = true;
const int window_width = 500;
const int window_height = 500;
const int stageDistant = 20000;
int speed = startingSpeed;
int arrowSpeed = 2;
int passedDistance = 0;
int score = 0;
double arrow_angle = 0;
double settingArrowAngle = 0;

Texture background;
Texture LeftBlockTexture;
Texture RightBlockTexture;
Texture CurrentArrow;
Texture gem_texture;
Texture gem1;
Texture gem2;
Texture arrow1;
Texture arrow2;
Texture purpleGemTexture;
Texture scoreBar; // 500 * 47 px
Texture scoreText;
Texture YouLostText;
Texture recordText;

//button
Texture button_tryAgain;
Texture button_continue;
Texture button_menu;
Texture button_setting;
Texture button_play;
Texture button_exit;
Texture button_unmute;
Texture button_mute;

Font* scoreFont;
Font* YouLost;

Music* bgmusic;
Sound* OGI;
Sound* PGI;

struct gem {
	Texture texture;
	SDL_Rect rect = { 250, -20, 20, 20 };
	bool NoIntersection = true;
};

enum Status {
	moving, intersection, disable, play, pause, menu, lose, setting
};



SDL_Rect scoreBar_rect = { 0, 30, 500, 47 };

SDL_Point arrow_point = { 250 + 10, 250 };
SDL_Rect arrow_rect = { 250, 250, 20, 20 };

SDL_Rect lostStatusBG = { 0,0,window_width,window_height };

//************ Button
SDL_Rect button_continue_rect = { 180,190,131,45 };
SDL_Rect button_try_rect = { 180,130,131,45 };
SDL_Rect button_menu_rect = { 180,250,131,45 };
SDL_Rect button_play_rect = { 180,130,131,45 };
SDL_Rect button_setting_rect = { 180,190,131,45 };
SDL_Rect button_exit_rect = { 180,250,131,45 };
SDL_Rect button_back_rect = { 180,20,131,45 };

SDL_Rect button_mute_rect = { 20,350,80,80 };
SDL_Rect button_unmute_rect = { 20,350,80,80 };


SDL_Rect right_block[10];
SDL_Rect left_block[10];

SDL_Rect* LB1 = new SDL_Rect[10];
SDL_Rect* RB1 = new SDL_Rect[10];

SDL_Rect* LB2 = new SDL_Rect[10];
SDL_Rect* RB2 = new SDL_Rect[10];

SDL_Rect settingArrowRect = { 210,220,30,30 };


bool NeedToUpdateBlocks1 = true;
bool NeedToUpdateBlocks2 = true;
bool isMusicPlaying = true;

Status gStatus = moving;
Status pgStatus = moving;
Status gameStatus = menu;

gem orange_gem;
gem purpleGem;


////////////////////////////////////////////////////////////           Functions              \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
void initObejects() {
	CurrentArrow = SBDL::loadTexture("assets/arrow.png");
	background = SBDL::loadTexture("assets/wall_m.png");
	RightBlockTexture = SBDL::loadTexture("assets/RoB.png");
	LeftBlockTexture = SBDL::loadTexture("assets/lob.png");
	gem_texture = SBDL::loadTexture("assets/gem.png");
	scoreBar = SBDL::loadTexture("assets/sbar.png", 220);
	orange_gem.texture = gem_texture;
	purpleGem.texture = SBDL::loadTexture("assets/pgem.png");
	button_tryAgain = SBDL::loadTexture("assets/but_try.png");
	button_continue = SBDL::loadTexture("assets/but_con.png");
	button_menu = SBDL::loadTexture("assets/butmenu.png");
	button_setting = SBDL::loadTexture("assets/butset.png");
	button_play = SBDL::loadTexture("assets/butplay.png");
	button_exit = SBDL::loadTexture("assets/butexit.png");
	button_mute = SBDL::loadTexture("assets/mute.png");
	button_unmute = SBDL::loadTexture("assets/unmute.png");

	gem1 = SBDL::loadTexture("assets/gem.png");
	gem2 = SBDL::loadTexture("assets/gem2.png");

	arrow1 = SBDL::loadTexture("assets/arrow.png");
	arrow2 = SBDL::loadTexture("assets/arrow2.png");


	//sounds
	bgmusic = SBDL::loadMusic("assets/music.wav");
	OGI = SBDL::loadSound("assets/blip.wav");
	PGI = SBDL::loadSound("assets/chime.wav");


	purpleGem.rect = { 250, -window_height * 3, 20, 20 };
	// init RIGHT Blocks
	for (int i = 0; i < 10; ++i) {
		right_block[i] = { window_width - i * 20 - 30, i * 50, i * 20 + 30, 50 };
	}
	// init LEFT blocks
	for (int i = 0; i < 10; ++i) {
		left_block[i] = { 0, i * 50, i * 20 + 60, 50 };
	}
}
void showScore();

void showGem();

void showPurpleGem();

void renderBackground(Texture bg, int img_w, int img_h, int scr_w, int scr_h);

void showArrow();

void showBlocks();

void moveBlocks();

void updateBlockPos();

void speedUp();

bool wallIntersection();

void restartGame();

void arrowSetttingAnimation(Texture chosenArrow);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	srand(time(NULL));


	SBDL::InitEngine("sbdl", window_width, window_height);

	initObejects();

	SBDL::playMusic(bgmusic, -1);


	scoreFont = SBDL::loadFont("assets/Font/Jetpackia.ttf", 14);
	YouLost = SBDL::loadFont("assets/Font/Jetpackia.ttf", 32);
	YouLostText = SBDL::createFontTexture(YouLost, " Y O U    L O S T", 200, 0, 0);
	

	const int FPS = 60; //frame per second
	const int delay = 1000 / FPS; //delay we need at each frame


	while (SBDL::isRunning()) {

		int startTime = SBDL::getTime();

		SBDL::updateEvents();
		SBDL::clearRenderScreen();

		//Game logic code

		//***********************************************************				   background				      ************************************************


		renderBackground(background, 255, 255, window_width, window_height);



		switch (gameStatus)
		{
		case play:
		{
			updateBlockPos();
			moveBlocks();
			showBlocks();
			speedUp();
			showArrow();
			if (wallIntersection())
				gameStatus = lose;

			if (LB1[9].y > 499)
				NeedToUpdateBlocks1 = true;
			if (LB1[0].y > 450)
				startStatus = false;
			if (LB2[9].y > 499)
				NeedToUpdateBlocks2 = true;

			showGem();
			showPurpleGem();
			showScore();

			if (SBDL::keyPressed(SDL_SCANCODE_P))
			{
				gameStatus = pause;

			}

			break;
		}
		case lose:
		{
			showBlocks();
			SBDL::showTexture(CurrentArrow, arrow_rect);
			showScore();
			SBDL::drawRectangle(lostStatusBG, 0, 0, 0, 160);

			SBDL::showTexture(button_tryAgain, 180, 130);
			SBDL::showTexture(button_continue, 180, 190);
			SBDL::showTexture(button_menu, 180, 250);

			SBDL::showTexture(YouLostText, 150, 50);

			if (SBDL::mouseInRect(button_continue_rect))
			{
				if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
				{
					gameStatus = play;
					arrow_rect.x = 255;
					arrow_point.x = 255;
				}
			}



			if (SBDL::mouseInRect(button_try_rect))
			{
				if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
				{
					restartGame();
					gameStatus = play;
					//cout << gameStatus << "game is restarted \n";

				}
			}

			if (SBDL::mouseInRect(button_menu_rect))
			{
				if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
				{
					restartGame();
					gameStatus = menu;
					//cout << gameStatus << "im in menu\n";

				}
			}

			break;
		}

		case pause:
		{
			showBlocks();
			SBDL::showTexture(CurrentArrow, arrow_rect);
			showScore();
			SBDL::drawRectangle(lostStatusBG, 0, 0, 0, 160);

			SBDL::showTexture(button_tryAgain, 180, 130);
			SBDL::showTexture(button_continue, 180, 190);
			SBDL::showTexture(button_menu, 180, 250);

			if (SBDL::mouseInRect(button_continue_rect))
			{
				if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
				{
					gameStatus = play;
					arrow_rect.x = 255;
					arrow_point.x = 255;
				}
			}



			if (SBDL::mouseInRect(button_try_rect))
			{
				if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
				{
					restartGame();
					gameStatus = play;
					//cout << gameStatus << "game is restarted \n";

				}
			}

			if (SBDL::mouseInRect(button_menu_rect))
			{
				if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
				{
					restartGame();
					gameStatus = menu;
					//cout << gameStatus << "im in menu\n";

				}
			}

			break;
		}

		case menu:
		{
			SBDL::showTexture(button_play, button_play_rect);
			SBDL::showTexture(button_setting, button_setting_rect);
			SBDL::showTexture(button_exit, button_exit_rect);

			if (SBDL::mouseInRect(button_play_rect))
			{
				if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
				{
					gameStatus = play;
					break;

				}
			}

			if (SBDL::mouseInRect(button_setting_rect))
			{
				if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
				{
					gameStatus = setting;
					break;

				}
			}

			if (SBDL::mouseInRect(button_exit_rect))
			{
				if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
				{
					return 0;
					break;

				}
			}


			break;
		}

		case setting:
		{
			SBDL::showTexture(button_exit, button_back_rect);
			SDL_Rect ar1 = { 400,300,60,60 };
			SDL_Rect ar2 = { 400,385,60,60 };


			SBDL::showTexture(arrow1, ar1);
			SBDL::showTexture(arrow2, ar2);

			if (isMusicPlaying)
			{

				SBDL::showTexture(button_unmute, button_unmute_rect);
				if (SBDL::mouseInRect(button_mute_rect))
				{
					if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
					{
						isMusicPlaying = false;
						SBDL::stopMusic();
					}
				}
			}
			else
			{
				SBDL::showTexture(button_mute, button_mute_rect);

				if (SBDL::mouseInRect(button_unmute_rect))
				{
					if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
					{
						isMusicPlaying = true;
						SBDL::playMusic(bgmusic, -1);
					}
				}
			}

			if (SBDL::mouseInRect(button_back_rect))
			{
				if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
				{
					gameStatus = menu;
				}
			}

			if (SBDL::mouseInRect(ar1))
			{
				if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
				{
					CurrentArrow = arrow1;
					orange_gem.texture = gem1;
				}
			}

			if (SBDL::mouseInRect(ar2))
			{
				if (SBDL::Mouse.clicked(SDL_BUTTON_LEFT, 1, SDL_PRESSED))
				{
					CurrentArrow = arrow2;
					orange_gem.texture = gem2;
				}
			}

			arrowSetttingAnimation(CurrentArrow);




			break;
		}
		}


		SBDL::updateRenderScreen();

		int elapsedTime = SBDL::getTime() - startTime;

		if (elapsedTime < delay)
			SBDL::delay(delay - elapsedTime);

	}


}
void showScore()
{
	SBDL::showTexture(scoreBar, scoreBar_rect);
	scoreText = SBDL::createFontTexture(scoreFont, to_string(score), 255, 255, 255);

	recordText = SBDL::createFontTexture(scoreFont, to_string(passedDistance / 100), 255, 255, 255);
	SBDL::showTexture(scoreText, 58, 33);
	SBDL::showTexture(recordText, 64, 52);

}

void renderBackground(Texture bg, int img_w, int img_h, int scr_w, int scr_h) {
	for (int i = 0; i < scr_w; i += img_w) {
		for (int j = 0; j < scr_h; j += img_h) {
			SBDL::showTexture(bg, i, j);

		}
	}
}

void showGem() {

	switch (gStatus) {
	case moving: {
		if (SBDL::pointInRect(arrow_point.x, arrow_point.y, orange_gem.rect)) {
			orange_gem.NoIntersection = false;
			gStatus = intersection;
			score++;
			SBDL::playSound(OGI, 1);
		}

		if (orange_gem.rect.y > 500) {
			orange_gem.rect.y = -(rand() % 300 + 140);
		}
		if (orange_gem.NoIntersection) {
			SBDL::showTexture(orange_gem.texture, orange_gem.rect);
			orange_gem.rect.y += speed;

		}
		break;
	}
	case intersection:

		SBDL::showTexture(orange_gem.texture, orange_gem.rect);

		orange_gem.rect.h--;
		orange_gem.rect.w--;
		orange_gem.rect.x = rand() % 30 + 240;
		if (orange_gem.rect.h == 0)
			gStatus = disable;


		break;

	case disable:
		orange_gem.rect.y++;
		if (orange_gem.rect.y > 500) {
			orange_gem.NoIntersection = true;
			orange_gem.rect.y = -(rand() % 30);
			orange_gem.rect.h = 20;
			orange_gem.rect.w = 20;
			gStatus = moving;
		}
		break;
	}
}


void showPurpleGem() {

	switch (pgStatus) {
	case moving: {
		if (SBDL::pointInRect(arrow_point.x, arrow_point.y, purpleGem.rect)) {
			purpleGem.NoIntersection = false;
			pgStatus = intersection;
			score += 10;
			SBDL::playSound(PGI, 1);
		}

		if (purpleGem.rect.y > 500) {
			purpleGem.rect.y = -(rand() % 300 + window_height * 3);
		}

		if (purpleGem.NoIntersection) {
			SBDL::showTexture(purpleGem.texture, purpleGem.rect);
			purpleGem.rect.y += speed;
		}

		break;
	}
	case intersection:

		SBDL::showTexture(purpleGem.texture, purpleGem.rect);

		purpleGem.rect.h--;
		purpleGem.rect.w--;
		purpleGem.rect.x = rand() % 30 + 240;
		if (purpleGem.rect.h == 0)
			pgStatus = disable;


		break;

	case disable:
		purpleGem.rect.y++;
		if (purpleGem.rect.y > 500) {
			purpleGem.NoIntersection = true;
			purpleGem.rect.y = -(rand() % 30 + window_height * 3);
			purpleGem.rect.h = 20;
			purpleGem.rect.w = 20;
			pgStatus = moving;
		}
		break;
	}
}
void showArrow() {
	if (SBDL::keyHeld(SDL_SCANCODE_SPACE)) {
		SBDL::showTexture(CurrentArrow, arrow_angle, arrow_rect);
		arrow_rect.x += 2;
		arrow_point.x += 2;

		if (arrow_angle < 45)
			arrow_angle += 10;
	}
	else {
		SBDL::showTexture(CurrentArrow, arrow_angle, arrow_rect);

		if (gameStatus != pause)
		{
			arrow_rect.x -= 2;
			arrow_point.x -= 2;

			if (arrow_angle > -45)
				arrow_angle -= 10;
		}

	}
}

void updateBlockPos() {
	if (NeedToUpdateBlocks1) {


		for (int i = 0; i < 10; ++i) {

			int index = rand() % 10;
			LB1[i] = left_block[index];
			LB1[i].y = -((i + 1) * 50) + 1;

		}

		for (int i = 0; i < 10; ++i) {

			int index = rand() % 10;
			RB1[i] = right_block[index];
			RB1[i].y = -((i + 1) * 50) + 1;

		}

		NeedToUpdateBlocks1 = false;
	}
	///// Update Block Position Second Part
	if (NeedToUpdateBlocks2) {


		for (int i = 0; i < 10; ++i) {
			int index = rand() % 10;
			LB2[i] = left_block[index];
			LB2[i].y = -((i + 1) * 50) + 1;
		}

		for (int i = 0; i < 10; ++i) {
			int index = rand() % 10;
			RB2[i] = right_block[index];
			RB2[i].y = -((i + 1) * 50) + 1;
		}
		NeedToUpdateBlocks2 = false;
	}
}
void showBlocks() {
	for (int i = 0; i < 10; ++i) {
		SBDL::showTexture(LeftBlockTexture, LB1[i]);
	}

	for (int i = 9; i >= 0; --i) {
		SBDL::showTexture(RightBlockTexture, RB1[i]);
	}

	for (int i = 0; i < 10; ++i) {
		SBDL::showTexture(LeftBlockTexture, LB2[i]);
	}

	for (int i = 9; i >= 0; --i) {
		SBDL::showTexture(RightBlockTexture, RB2[i]);
	}
}
void moveBlocks() {
	for (int i = 0; i < 10; ++i) {
		LB1[i].y += speed;
		passedDistance += speed;
		//cout << passedDistance << endl;
	}

	for (int i = 0; i < 10; ++i) {
		RB1[i].y += speed;
	}

	if (!startStatus) {
		for (int i = 0; i < 10; ++i) {
			LB2[i].y += speed;
		}

		for (int i = 0; i < 10; ++i) {
			RB2[i].y += speed;
		}
	}
}
void speedUp()
{
	if (speed < 7)
	{
		if (passedDistance % stageDistant == 0)
		{
			speed++;
			//cout << "speed++ \n";
		}
	}
}
void restartGame() {

	arrow_point = { 250 + 10, 250 };
	arrow_rect = { 250, 250, 20, 20 };
	speed = startingSpeed;
	arrowSpeed = 2;
	passedDistance = 0;
	score = 0;
	arrow_angle = 0;

	//update block pos 
	for (int i = 0; i < 10; ++i) {

		int index = rand() % 10;
		LB1[i] = left_block[index];
		LB1[i].y = -((i + 1) * 50) + 1;

	}

	for (int i = 0; i < 10; ++i) {

		int index = rand() % 10;
		RB1[i] = right_block[index];
		RB1[i].y = -((i + 1) * 50) + 1;

	}

	NeedToUpdateBlocks1 = false;

	for (int i = 0; i < 10; ++i) {
		int index = rand() % 10;
		LB2[i] = left_block[index];
		LB2[i].y = -((i + 1) * 50) + 1;
	}

	for (int i = 0; i < 10; ++i) {
		int index = rand() % 10;
		RB2[i] = right_block[index];
		RB2[i].y = -((i + 1) * 50) + 1;
	}
	NeedToUpdateBlocks2 = false;

	startStatus = true;
}

bool wallIntersection()
{

	for (int i = 0; i < 10; ++i) {
		if (SBDL::pointInRect(arrow_point.x, arrow_point.y, LB1[i]))
			return true;
		//cout << passedDistance << endl;
	}

	for (int i = 0; i < 10; ++i) {
		if (SBDL::pointInRect(arrow_point.x, arrow_point.y, RB1[i]))
			return true;
	}


	for (int i = 0; i < 10; ++i) {
		if (SBDL::pointInRect(arrow_point.x, arrow_point.y, LB2[i]))
			return true;
	}

	for (int i = 0; i < 10; ++i) {
		if (SBDL::pointInRect(arrow_point.x, arrow_point.y, RB2[i]))
			return true;
	}

	return false;

}

void arrowSetttingAnimation(Texture chosenArrow)
{
	static int i = 1;

	if (settingArrowRect.x < -10)
		settingArrowRect.x += window_width;

	if (settingArrowRect.x > 510)
		settingArrowRect.x -= window_width;



	if (settingArrowAngle == 45 || settingArrowAngle == -45)
		i = -i;

	settingArrowAngle += i;
	settingArrowRect.x += settingArrowAngle / 10;

	SBDL::showTexture(chosenArrow, settingArrowAngle, settingArrowRect);

}