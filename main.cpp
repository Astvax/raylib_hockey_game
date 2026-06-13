#include <raylib.h>
#include <iostream>
using namespace std;

Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

Sound goalSound;
Sound winSound;
Sound loseSound;

int player_score = 0;
int cpu_score = 0;
bool gameOver = false;
string winner = "";

void CheckWinner()
{
    if (player_score >= 10)
    {
        gameOver = true;
        winner = "YOU LOSE!";
        PlaySound(loseSound);
    }
    else if (cpu_score >= 10)
    {
        gameOver = true;
        winner = "YOU WIN";
        PlaySound(winSound);
    }
}

class Ball
{
public:
    float x, y;
    float speed_x, speed_y;
    int radius;

    void Draw()
    {
        DrawCircle(x, y, radius, BLACK);
    }

    void Update()
    {
        x += speed_x;
        y += speed_y;
        if (y + radius >= GetScreenHeight() || y - radius <= 0)
        {
            speed_y *= -1;
        }
        if (x + radius >= GetScreenWidth())
        {
            cpu_score++;
            PlaySound(goalSound);
            CheckWinner();
            if (!gameOver)
            {
                ResetBall();
            }
        }
        if (x - radius <= 0)
        {
            player_score++;
            PlaySound(goalSound);
            CheckWinner();
            if (!gameOver)
            {
                ResetBall();
            }
        }
    }
    void ResetBall()
    {
        x = GetScreenWidth() / 2;
        y = GetScreenWidth() / 2;

        int speed_choices[2] = {-1, 1};
        speed_x *= speed_choices[GetRandomValue(0, 1)];
        speed_y *= speed_choices[GetRandomValue(0, 1)];
        speed_x = 6;
        speed_y = 6;
    }
};

class Paddle
{
protected:
    void LimitMovement()
    {
        if (y <= 0)
        {
            y = 0;
        }
        if (y + height >= GetScreenHeight())
        {
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;
    float width, height;
    int speed;

    void Draw(Color color)
    {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, color);
    }
    void Update()
    {
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        {
            y = y - speed;
        }
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        {
            y = y + speed;
        }

        LimitMovement();
    }
};

class CpuPaddle : public Paddle
{
public:
    void Update(int ball_y)
    {
        if (y + height / 2 > ball_y)
        {
            y = y - speed;
        }
        if (y + height / 2 <= ball_y)
        {
            y = y + speed;
        }
        LimitMovement();
    }
};

Ball ball;
Paddle player;
CpuPaddle cpu;

void ResetGame()
{

    player_score = 0;
    cpu_score = 0;
    gameOver = false;
    winner = "";

    ball.x = GetScreenWidth() / 2;
    ball.y = GetScreenHeight() / 2;
    ball.speed_x = 5;
    ball.speed_y = 5;

    player.y = GetScreenHeight() / 2;
    cpu.y = GetScreenHeight() / 2;
}

int main()
{

    const int screen_width = 1280;
    const int screen_height = 800;
    string title = "ping-pong";
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(screen_width, screen_height, title.c_str());
    SetTargetFPS(120);
    InitAudioDevice();
    goalSound = LoadSound("YOUR_SOUND_HERE.wav"); //Use your sound and paste here
    winSound = LoadSound("YOUR_SOUND_HERE.wav");  //Use your sound and paste here
    loseSound = LoadSound("YOUR_SOUND_HERE.wav"); //Use your sound and paste here

    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 5;
    ball.speed_y = 5;

    player.width = 25;
    player.height = 120;
    player.x = 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 6;

    cpu.height = 120;
    cpu.width = 25;
    cpu.x = screen_width - cpu.width - 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 5;

    while (!WindowShouldClose())
    {

        if (gameOver && IsKeyPressed(KEY_SPACE))
        {
            ResetGame();
        }

        if (!gameOver)
        {

            ball.Update();
            player.Update();
            cpu.Update(ball.y);
            if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height}))
            {

                ball.speed_x *= -1;
                if (ball.speed_x < 0)
                {
                    ball.speed_x -= 0.25;
                }
                else
                {
                    ball.speed_x += 0.25;
                }
            }
            if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height}))
            {

                ball.speed_x *= -1;
                if (ball.speed_x < 0)
                {
                    ball.speed_x -= 0.25;
                }
                else
                {
                    ball.speed_x += 0.25;
                }
            }
        }
        BeginDrawing();

        ClearBackground(DARKBLUE);
        DrawCircle(screen_width / 2, screen_height / 2, 150, BLUE);
        DrawRectangle(screen_width / 2 - 10, 0, 20, 800, BLUE);
        DrawCircle(screen_width / 2, screen_height / 2, 50, WHITE);
        ball.Draw();
        cpu.Draw(RED);
        player.Draw(BLUE);
        DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, BLUE);
        DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, RED);

        if (gameOver)
        {
            DrawRectangle(0, 0, screen_width, screen_height, Color{0, 0, 0, 150});
            int textWidth = MeasureText(winner.c_str(), 80);
            DrawText(winner.c_str(), screen_width / 2 - textWidth / 2, screen_height / 2 - 40, 80, winner=="YOU WIN" ? GREEN:RED);
            string text = "Press SPACE to play again";
            int textWidth2 = MeasureText(text.c_str(), 30);
            DrawText(text.c_str(), screen_width / 2 - textWidth2 / 2, screen_height / 2 + 40, 30, YELLOW);
            string leaveText = "Press ESCAPE to leave";
            int textWidth3 = MeasureText(leaveText.c_str(), 30);
            DrawText(leaveText.c_str(), screen_width / 2 - textWidth3 / 2, screen_height / 2 + 80, 30, RED);
        }

        EndDrawing();
    }
    UnloadSound(winSound);
    UnloadSound(loseSound);
    UnloadSound(goalSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
