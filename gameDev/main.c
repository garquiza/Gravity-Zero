#include "raylib.h"

#include <math.h>
// Some Defines
#define PLAYER_BASE_SIZE  0
#define SHIP_SPEED        6.0f
#define ASTER_SPEED       3
#define MAX_MEDIUM_ASTER  5
#define MAX_SMALL_ASTER   8

typedef struct Ship
{
    Vector2 position;
    Vector2 speed;
    float acceleration;
    float rotation;
    Vector3 collider;
} Ship;

typedef struct Aster
{
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
} Aster;

// Global Variables Definition
static const int screenWidth = 950;
static const int screenHeight = 600;

static int Back_Drop = 0;
static int timeScore = 0;
static int hiScore = 0;
static bool gameOver = false;
static bool pause =  false;

static float shipHeight = 0.0f;

static Ship ship = { 0 };
static Aster mAster[MAX_MEDIUM_ASTER] = { 0 };
static Aster sAster[MAX_SMALL_ASTER] = { 0 };

Texture2D shipSprite;
Texture2D mAsterSprite;
Texture2D sAsterSprite;
Sound game;


// Module Functions Declaration
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)

int main(void)
{
    //Initialization
    InitWindow(screenWidth, screenHeight, "GRAVITY ZERO"); // Initialize window and OpenGL context
    InitAudioDevice(); // Initialize audio device and context
    shipSprite = LoadTexture("pics/ship.png"); //get the sprite of the ship (player) from the file
    mAsterSprite = LoadTexture("pics/mAster.png"); //get the sprite of the medium asteroid from the file
    sAsterSprite = LoadTexture("pics/sAster.png"); //get the sprite of the small asteroid from the file
    game =  LoadSound("pics/game.wav"); //get the sound of the game from the file

    InitGame();
    SetTargetFPS(60);  //60 frames-per-second
    SetSoundVolume(game, 1.0f);  // Set volume for the game

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        UpdateDrawFrame();
    }

    UnloadGame();
    UnloadTexture(shipSprite);
    UnloadTexture(mAsterSprite);
    UnloadTexture(sAsterSprite);
    UnloadSound(game);
    CloseAudioDevice(); // Close the audio device and context
    CloseWindow(); // Close window and OpenGL context
    return 0;
}

void InitGame(void)
{
    int posx, posy;
    int velx, vely;
    bool correctRange = false;

    pause = false;

    PlaySound(game);

    timeScore = 0;

    shipHeight = (PLAYER_BASE_SIZE/2)/tanf(20*DEG2RAD);

    // Initialize ship (player)
    ship.position = (Vector2){screenWidth/2, screenHeight/2 - shipHeight/2};
    ship.speed = (Vector2){0, 0};
    ship.acceleration = 0;
    ship.rotation = 0;
    ship.collider = (Vector3){ship.position.x + sin(ship.rotation*DEG2RAD)*(shipHeight/2.5f), ship.position.y - cos(ship.rotation*DEG2RAD)*(shipHeight/2.5f), 12};


    //Initialize Medium Asteroids
    for (int i = 0; i < MAX_MEDIUM_ASTER; i++)
    {
        posx = GetRandomValue(0, screenWidth);

        while (!correctRange)
        {
            if (posx > screenWidth/2 - 50 && posx < screenWidth/2 + 50) posx = GetRandomValue(0, screenWidth);
            else correctRange = true;
        }

        correctRange = false;

        posy = GetRandomValue(0, screenHeight);

        while (!correctRange)
        {
            if (posy > screenHeight/2 - 50 && posy < screenHeight/2 + 50)  posy = GetRandomValue(0, screenHeight);
            else correctRange = true;
        }

        correctRange = false;
        velx = GetRandomValue(-ASTER_SPEED, ASTER_SPEED);
        vely = GetRandomValue(-ASTER_SPEED, ASTER_SPEED);

        while (!correctRange)
        {
            if (velx == 0 && vely == 0)
            {
                velx = GetRandomValue(-ASTER_SPEED, ASTER_SPEED);
                vely = GetRandomValue(-ASTER_SPEED, ASTER_SPEED);
            }
            else correctRange = true;
        }
        mAster[i].position = (Vector2){posx, posy};
        mAster[i].speed = (Vector2){velx, vely};
        mAster[i].radius = 20;
        mAster[i].active = true;

    }
        //Initialize Small Asteroids
        for (int i = 0; i < MAX_SMALL_ASTER; i++)
    {
        posx = GetRandomValue(0, screenWidth);

        while (!correctRange)
        {
            if (posx > screenWidth/2 - 150 && posx < screenWidth/2 + 150) posx = GetRandomValue(0, screenWidth);
            else correctRange = true;
        }

        correctRange = false;

        posy = GetRandomValue(0, screenHeight);

        while (!correctRange)
        {
            if (posy > screenHeight/2 - 150 && posy < screenHeight/2 + 150)  posy = GetRandomValue(0, screenHeight);
            else correctRange = true;
        }

        correctRange = false;
        velx = GetRandomValue(-ASTER_SPEED, ASTER_SPEED);
        vely = GetRandomValue(-ASTER_SPEED, ASTER_SPEED);

        while (!correctRange)
        {
            if (velx == 0 && vely == 0)
            {
                velx = GetRandomValue(-ASTER_SPEED, ASTER_SPEED);
                vely = GetRandomValue(-ASTER_SPEED, ASTER_SPEED);
            }
            else correctRange = true;
        }
        sAster[i].position = (Vector2){posx, posy};
        sAster[i].speed = (Vector2){velx, vely};
        sAster[i].radius = 10;
        sAster[i].active = true;
    }
}

void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed(KEY_SPACE)) pause = !pause;

        if (!pause)
        {
            timeScore++;

            // Ship (player) logic

            // Rotation
            if (IsKeyDown(KEY_A)) ship.rotation -= 5;
            if (IsKeyDown(KEY_D)) ship.rotation += 5;

            // Speed
            ship.speed.x = sin(ship.rotation*DEG2RAD)*SHIP_SPEED;
            ship.speed.y = cos(ship.rotation*DEG2RAD)*SHIP_SPEED;

            // Controller
            if (IsKeyDown(KEY_W))
            {
                if (ship.acceleration < 1) ship.acceleration += 0.04f;
            }
            else
            {
                if (ship.acceleration > 0) ship.acceleration -= 0.02f;
                else if (ship.acceleration < 0) ship.acceleration = 0;
            }


            // Movement
            ship.position.x += (ship.speed.x*ship.acceleration);
            ship.position.y -= (ship.speed.y*ship.acceleration);

            // Wall behaviour for ship (player)
            if (ship.position.x > screenWidth + shipHeight) ship.position.x = -(shipHeight);
            else if (ship.position.x < -(shipHeight)) ship.position.x = screenWidth + shipHeight;
            if (ship.position.y > (screenHeight + shipHeight)) ship.position.y = -(shipHeight);
            else if (ship.position.y < -(shipHeight)) ship.position.y = screenHeight + shipHeight;

            // Collision Ship to Asteroids
            ship.collider = (Vector3){ship.position.x + sin(ship.rotation*DEG2RAD)*(shipHeight/2.5f), ship.position.y - cos(ship.rotation*DEG2RAD)*(shipHeight/2.5f), 12};

            //Medium Asteroids
            for (int a = 0; a < MAX_MEDIUM_ASTER; a++)
            {
                if (CheckCollisionCircles((Vector2){ship.collider.x, ship.collider.y}, ship.collider.z, mAster[a].position, mAster[a].radius) && mAster[a].active) gameOver = true;
            }
            //Small Asteroids
            for (int a = 0; a < MAX_SMALL_ASTER; a++)
            {
                if (CheckCollisionCircles((Vector2){ship.collider.x, ship.collider.y}, ship.collider.z, sAster[a].position, sAster[a].radius) && sAster[a].active) gameOver = true;
            }

            // Medium Asteroids logic
            for (int i = 0; i < MAX_MEDIUM_ASTER; i++)
            {
                if (mAster[i].active)
                {
                    // movement
                    mAster[i].position.x += mAster[i].speed.x;
                    mAster[i].position.y += mAster[i].speed.y;

                    // wall behaviour
                    if  (mAster[i].position.x > screenWidth + mAster[i].radius) mAster[i].position.x = -(mAster[i].radius);
                    else if (mAster[i].position.x < 0 - mAster[i].radius) mAster[i].position.x = screenWidth + mAster[i].radius;
                    if (mAster[i].position.y > screenHeight + mAster[i].radius) mAster[i].position.y = -(mAster[i].radius);
                    else if (mAster[i].position.y < 0 - mAster[i].radius) mAster[i].position.y = screenHeight + mAster[i].radius;

                }
            }

            //Small Asteroids logic
            for (int i = 0; i < MAX_SMALL_ASTER; i++)
                {
                    if (sAster[i].active)
                    {
                        // movement
                        sAster[i].position.x += sAster[i].speed.x;
                        sAster[i].position.y += sAster[i].speed.y;

                        // wall behaviour
                        if  (sAster[i].position.x > screenWidth + sAster[i].radius) sAster[i].position.x = -(sAster[i].radius);
                        else if (sAster[i].position.x < 0 - sAster[i].radius) sAster[i].position.x = screenWidth + sAster[i].radius;
                        if (sAster[i].position.y > screenHeight + sAster[i].radius) sAster[i].position.y = -(sAster[i].radius);
                        else if (sAster[i].position.y < 0 - sAster[i].radius) sAster[i].position.y = screenHeight + sAster[i].radius;

                    }
                }
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame(void)
{
    BeginDrawing();

    //Background
    Back_Drop +=1;
        if (Back_Drop > 200) {

            Back_Drop = 1;
        }
     if (Back_Drop < 100) {
            ClearBackground((Color) {  95, 77, 138  });
        }
        else {
            ClearBackground((Color){   70, 57, 102  });
        }


        if (!gameOver)

        //ship and asteroids
        DrawTexture(shipSprite, ship.position.x, ship.position.y, WHITE);

        for (int i = 0;i < MAX_MEDIUM_ASTER; i++)
        {
            if (mAster[i].active) DrawTexture(mAsterSprite, mAster[i].position.x, mAster[i].position.y, WHITE);
        }

        for (int i = 0;i < MAX_SMALL_ASTER; i++)
        {
            if (sAster[i].active) DrawTexture(sAsterSprite, sAster[i].position.x, sAster[i].position.y, WHITE);
        }

        // time and high score
        DrawText(" PRESS W TO MOVE FORWARD     PRESS A TO MOVE LEFT     PRESS D TO MOVE RIGHT", 10, 567, 20, BLACK);
        DrawText(TextFormat("TIME SCORE: %.02f", (float)timeScore/60), 10, 35, 25, BLACK);
            if(hiScore < timeScore) {
                hiScore = timeScore;
                DrawText(TextFormat("HI SCORE: %.02f", (float)hiScore/60), 10, 10, 25, BLACK); }
            else
            {
            DrawText(TextFormat("HI SCORE: %.02f", (float)hiScore/60), 10, 10, 25, BLACK);
            }

            if (pause) {
            DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, BLACK); }

            if (gameOver)
            {
            DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 30)/2, GetScreenHeight()/2 - 50, 30, BLACK);
            }

    EndDrawing();
}

void UnloadGame(void) {}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}