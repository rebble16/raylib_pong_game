#include <iostream>
#include <raylib.h>
#include<string>

#define WIDTH 1280
#define HEIGHT 800
#define GREEN {0, 175, 75, 255 }
#define RED {150, 0, 30, 255 }

using namespace std;

enum menu_flag{EMPTY,BOT,PLAYER,QUIT};
struct ball
{
	int x, y;
	float radius;
	int speed_x;
	int speed_y;
	ball();
	void display()const;
	void Movement(Sound& o);
	bool IsInBoundries()const;
	void Reset();

};
struct paddle
{
	int x, y;
	int p_height, p_width;
	float roundness;
	int speed_y;
	int score;
	paddle(int x=10);
	void display()const;
	void LimitMovement();
	virtual void Movement()=0;
	virtual bool IsScored(ball* p_bl)=0;

};
struct player1 :public paddle
{
public:
	player1():paddle() {}
	void Movement()override;
	bool IsScored(ball* p_bl)override;
};
struct player2 :public paddle
{
public:
	player2() { x=WIDTH - p_width - 10; }
	void Movement()override;
	bool IsScored(ball* p_bl)override;
};
struct paddle_bot :public paddle
{
	int reactionDelay;    
	int framesCounter;     
	int targetY;           
	paddle_bot();
	void Movement(ball* p_bl);
	void Movement()override {};
	bool IsScored(ball* p_bl)override;

};
class Button
{
private:
	Vector2 position;
	const char* name;
	Vector2 size;
	Color color;
public:
	Button(Vector2* vec = nullptr, const char* str = " ", Vector2* s = nullptr, Color* c =nullptr);
	void display()const;
	bool isPressed()const;
};
class Game
{
	ball bl;
	player1 pl1;
	player2 pl2;
	paddle_bot bt;
	
	Vector2 center{ WIDTH / 2, HEIGHT / 2 };
	bool GameRunning;
	bool menu_choice_picked;
	menu_flag choice;
public:
	Game() { GameRunning = true; menu_choice_picked = false; choice = EMPTY; }
	void Menu(Sound& o);
	void Run();
	void CollisionCheck(Sound& o);
	void WinnerCheck(Sound& w, Sound& l);
	void ShowScoreboard();
	void Player_vs_Bot(Sound& wh, Sound& h, Sound& gl,Sound& victory, Sound& lose);
	void Player_vs_Player(Sound& wh, Sound& h, Sound& gl, Sound& victory);


};


//=======================MAIN==================================
int main()
{
	Game game;
	game.Run();
	return 0;
}


ball::ball()
{
	x = WIDTH / 2;
	y = HEIGHT / 2;
	radius = 10.0f;
	speed_x = 7;
	speed_y = 7;
}

paddle::paddle(int x)
{
	this->x = x;
	y = HEIGHT / 2;
	p_height = 125;
	p_width = 25;
	roundness = 7.0f;
	speed_y = 10;
	score = 0;

}

void paddle::display() const
{
	Rectangle rec = { x, y, p_width, p_height };
	DrawRectangleRounded(rec, roundness, 3, YELLOW);
}


void ball::Movement(Sound& o)
{
	y += speed_y;
	x += speed_x;
	if (!IsInBoundries())
	{
		speed_y *= -1;
		PlaySound(o);
	}

}

bool ball::IsInBoundries() const
{
	return (((y + radius) >= GetScreenHeight()) || (y - radius) <= 0) ? false : true;
}

void ball::Reset()
{
	x = WIDTH / 2;
	y = HEIGHT / 2;
	speed_y *= -1;
	speed_x *= -1;
}

void paddle::LimitMovement() 
{
	if (y <= 0)y = 0;
	if (y+p_height>=GetScreenHeight())y = GetScreenHeight() - p_height;
}


void ball::display() const
{
	 DrawCircle(x, y,  radius, WHITE);
}

void Game::Menu(Sound& o)
{
	Vector2 pos1 = { WIDTH/2 -200,HEIGHT/2 - 200};
	Vector2 pos2 = { WIDTH / 2 - 200,HEIGHT / 2 };
	Vector2 pos3 = { WIDTH / 2 - 200,HEIGHT / 2 + 200 };
	Vector2 size = { 400,150 };
	const char* button_name1 = "PLAY WITH BOT";
	const char* button_name2 = "PLAY WITH FRIEND";
	const char* button_name3 = "QUIT";
	Color color = { 0, 0, 255, 255 };
	Button but1 =  { &pos1,button_name1,&size,&color };
	Button but2 = { &pos2,button_name2,&size,&color };
	Button but3 = { &pos3,button_name3,&size,&color };
	but1.display();
	but2.display();
	but3.display();
	if (but1.isPressed())
	{
		PlaySound(o);
		cout << "Button 1 pressed!!!" << endl;
		choice = BOT;
		menu_choice_picked = true;
	}
	if (but2.isPressed())
	{
		PlaySound(o);
		cout << "Button 2 pressed!!!" << endl;
		choice = PLAYER;
		menu_choice_picked = true;
	}
	if (but3.isPressed())
	{
		PlaySound(o);
		cout << "Button 3 pressed!!!" << endl;
		choice = QUIT;
		menu_choice_picked = true;
	}
}

void Game::Run()
{
	InitWindow(WIDTH, HEIGHT, "Pong Game made by rebble");
	InitAudioDevice();//
	Sound hit=LoadSound("sounds/the-sound-of-hitting-the-ball.mp3");
	Sound wall_hit = LoadSound("sounds/throwing-a-ping-pong-ball-against-a-wall.mp3");
	Sound victory = LoadSound("sounds/victory.wav");
	Sound lose = LoadSound("sounds/game_over.mp3");
	Sound goal = LoadSound("sounds/goal.mp3");
	Sound button = LoadSound("sounds/button_pressed.mp3");
	SetTargetFPS(60);
	do
	{
		BeginDrawing();
		DrawText("GAME MENU", 450, 100, 60, WHITE);
		Menu(button);
		EndDrawing();
	} while (!WindowShouldClose()&&!menu_choice_picked);
	while (!WindowShouldClose()&&GameRunning)
	{
		BeginDrawing();
		ClearBackground(GREEN);
		ShowScoreboard();
		DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, WHITE);                                
		DrawCircleLines(center.x,center.y, 150.0f, WHITE);
		if (choice == BOT)
			Player_vs_Bot(wall_hit, hit, goal,victory,lose);
		if (choice == PLAYER)
			Player_vs_Player(wall_hit, hit, goal, victory);
		if (choice == QUIT)
		{
			WaitTime(0.5);
			break;
		}

		EndDrawing();
	}
	CloseAudioDevice();
}

void Game::CollisionCheck(Sound& o)
{
	Vector2 bl_vec = { bl.x,bl.y };
	Rectangle pl1_rec = { pl1.x, pl1.y, pl1.p_width, pl1.p_height };
	if (CheckCollisionCircleRec(bl_vec, bl.radius, pl1_rec) && bl.speed_x < 0)
	{
		bl.speed_x *= -1;
		PlaySound(o);
	}
	if (choice == BOT)
	{
		Rectangle bt_rec = { bt.x, bt.y, bt.p_width, bt.p_height };
		if (CheckCollisionCircleRec(bl_vec, bl.radius, bt_rec) && bl.speed_x > 0)
		{
			bl.speed_x *= -1;
			PlaySound(o);
		}
	}
	if (choice == PLAYER)
	{
		Rectangle pl2_rec = { pl2.x, pl2.y, pl1.p_width, pl2.p_height };
		if (CheckCollisionCircleRec(bl_vec, bl.radius, pl2_rec) && bl.speed_x > 0)
		{
			bl.speed_x *= -1;
			PlaySound(o);
		}
	}
}

void Game::WinnerCheck(Sound& w,Sound& l)
{
	int fontSize = 100;
	if (choice == BOT)
	{
		if (pl1.score == 11 || bt.score == 11)
		{
			if (pl1.score == 11)PlaySound(w);
			else PlaySound(l);
			const char* winner = (pl1.score == 11) ? "Player has won!" : "Bot has won!";
			ClearBackground(BLACK);
			int textWidth = MeasureText(winner, fontSize);
			int textHeight = fontSize;

			float textX = (WIDTH-textWidth) / 2.0f;
			float textY = (HEIGHT - textHeight) / 2.0f;
			DrawText(winner, textX, textY, fontSize, WHITE);
			EndDrawing();
			WaitTime(5);
			GameRunning = false;
		}

	}
	if (choice == PLAYER)
	{
		if (pl1.score == 11 || pl2.score == 11)
		{
			PlaySound(w);
			const char* winner = (pl1.score == 11) ? "Player 1 has won!" : "Player 2 has won!";
			ClearBackground(BLACK);
			int textWidth = MeasureText(winner, fontSize);
			int textHeight = fontSize;

			float textX = (WIDTH- textWidth) / 2.0f;
			float textY = (HEIGHT - textHeight) / 2.0f;
			DrawText(winner, textX, textY, fontSize, WHITE);
			EndDrawing();
			WaitTime(5);
			GameRunning = false;
		}
	}
	

}

void Game::ShowScoreboard()
{
	string pl1_score = to_string(pl1.score);
	DrawText(pl1_score.c_str(), center.x - 125, 30, 50, RED);
	if (choice == BOT)
	{
		string bt_score = to_string(bt.score);
		DrawText(bt_score.c_str(), center.x + 100, 30, 50, RED);
	}
	if (choice == PLAYER)
	{
		string pl2_score = to_string(pl2.score);
		DrawText(pl2_score.c_str(), center.x + 100, 30, 50, RED);
	}

}

void Game::Player_vs_Bot(Sound& wh,Sound& h, Sound& gl,Sound& victory,Sound& lose)
{
	WinnerCheck(victory,lose);
	pl1.Movement();
	bl.Movement(wh);
	bt.Movement(&bl);
	CollisionCheck(h);
	if (pl1.IsScored(&bl))
	{
		PlaySound(gl);
		pl1.score++;
		bl.Reset();
	}
	if (bt.IsScored(&bl))
	{
		PlaySound(gl);
		bt.score++;
		bl.Reset();
	}
	pl1.display();
	bl.display();
	bt.display();
}

void Game::Player_vs_Player(Sound& wh, Sound& h, Sound& gl, Sound& victory)
{
	WinnerCheck(victory,victory);
	pl1.Movement();
	pl2.Movement();
	bl.Movement(wh);
	CollisionCheck(h);
	if (pl1.IsScored(&bl))
	{
		PlaySound(gl);
		pl1.score++;
		bl.Reset();
	}
	if (pl2.IsScored(&bl))
	{
		PlaySound(gl);
		pl2.score++;
		bl.Reset();
	}
	pl1.display();
	bl.display();
	pl2.display();
}


paddle_bot::paddle_bot()
{
	x = WIDTH - p_width - 10;
	framesCounter = 0;
	targetY = HEIGHT / 2;
	reactionDelay = 15 + rand() % 10; 
	speed_y = 7;                       
}

void paddle_bot::Movement(ball* p_bl)
{
	if (p_bl == nullptr) return;

	framesCounter++;

	if (framesCounter >= reactionDelay)
	{
		framesCounter = 0;
		int errorRange=80;
		int error = (rand() % (errorRange * 2)) - errorRange;		
		int predictedY = p_bl->y + p_bl->speed_y * 15;		
		targetY = predictedY + error;	
	}

	int centerY = y + p_height / 2;
	int distance = abs(centerY - targetY);

	if (distance > 10)
	{
		int speed = speed_y;

		if (distance < 30)
			speed = speed_y / 2;

		if (centerY < targetY)
			y += min(speed, distance);
		else
			y -= min(speed, distance);
	}
	LimitMovement();
}

bool paddle_bot::IsScored(ball* p_bl)
{
	return (p_bl->x == 10) ? true : false;
}

Button::Button(Vector2* vec, const char* str, Vector2* s, Color* c)
{
	position = *vec;
	name = str;
	size = *s;
	color = *c;
	
}
void Button::display()const
{
	DrawRectangleV(position, size, color);
	
	int fontSize = 30;
	int textWidth = MeasureText(name, fontSize);
	int textHeight = fontSize;

	float textX = position.x + (size.x - textWidth) / 2.0f;
	float textY = position.y + (size.y - textHeight) / 2.0f;

	DrawText(name, textX,textY, fontSize, WHITE);
}

bool Button::isPressed() const
{
	Vector2 mouse=GetMousePosition(); // Get mouse position XY
	Rectangle rec = { position.x,position.y,size.x,size.y };
	return CheckCollisionPointRec(mouse, rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)? true : false;
}

void player1::Movement()
{
	if (IsKeyDown(KEY_UP))
		y -= speed_y;
	if (IsKeyDown(KEY_DOWN))
		y += speed_y;
	LimitMovement();
}

bool player1::IsScored(ball* p_bl)
{
	return (p_bl->x >= WIDTH - 10) ? true : false;
}

void player2::Movement()
{
	if (IsKeyDown(KEY_W))
		y -= speed_y;
	if (IsKeyDown(KEY_S))
		y += speed_y;
	LimitMovement();
}
bool player2::IsScored(ball* p_bl)
{
	return (p_bl->x <= 10) ? true : false;
}
