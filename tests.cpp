#include "raylib.h"
#include <cmath>

int main(void)
{
    // init
    const int screenWidth = 1200;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "miya raylib playground");


    SetTargetFPS(60);
    // triangle things
    float posv1x = screenWidth/2, posv1y = screenHeight/2, posv2x = 100.0, posv2y = screenHeight - 100.0;
    float posv3x = screenWidth - 100.0, posv3y = screenHeight - 100.0;
    unsigned char a = 0,b = 0,c = 0;

    //ship things
    Texture ship = LoadTexture("millenium.png");
    Rectangle sourceShip = { 0, 0, ship.width, ship.height };
    Vector2 shipposition = { screenWidth/2, screenHeight/2 };
    float ship_modulo_speed = 0;
    float shipangle = 0;
    float shipangularvelocity = 0;
    Vector2 shipvelocities = { 0, 0 };

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {   
        // update triangle
        posv1x = screenWidth/2 + 150*cosf(GetTime());
        posv1y = screenHeight/2 + 150*sinf(GetTime());

        posv2y = screenHeight - 200 - 100.0*sinf(GetTime());
        posv3y = screenHeight - 200 - 100.0*cosf(GetTime());

        a = sinf(GetTime()+2)*200;
        b = sinf(GetTime()+3)*200;
        c = sinf(GetTime()+4)*200;

        // update ship
        shipposition.x += shipvelocities.x;
        shipangle += shipangularvelocity;
        if(shipposition.x > screenWidth) shipposition.x = 0;
        if(shipposition.x < 0) shipposition.x = screenWidth;
        shipposition.y += shipvelocities.y;
        if(shipposition.y > screenHeight) shipposition.y = 0;
        if(shipposition.y < 0) shipposition.y = screenHeight;
        shipvelocities = { ship_modulo_speed*cosf(shipangle), ship_modulo_speed*sinf(shipangle) };
        ship_modulo_speed *= 0.98;
        shipangularvelocity *= 0.98;

        if (IsKeyDown(KEY_RIGHT)) shipangularvelocity += 0.01;
        if (IsKeyDown(KEY_LEFT)) shipangularvelocity -= 0.01;
        if (IsKeyDown(KEY_UP)) ship_modulo_speed += 0.3;
        if (IsKeyDown(KEY_DOWN)) ship_modulo_speed -= 0.3;

        BeginDrawing();

            ClearBackground(RAYWHITE);

            //draw the triangle
            DrawTriangleLines({ posv1x , posv1y },
                         { posv2x , posv2y },
                         { posv3x , posv3y }, BLACK);

            DrawTriangle({ posv1x , posv1y },
                         { posv2x , posv2y },
                         { posv3x , posv3y }, Color{a,b,c,200});
            
            DrawLine(screenWidth/2, screenHeight/3,posv1x, posv1y, BLACK);
            DrawLine(screenWidth/2, screenHeight/3,posv2x, posv2y, BLACK);
            DrawLine(screenWidth/2, screenHeight/3,posv3x, posv3y, BLACK);

            //draw the ship
            Rectangle destrec = {shipposition.x, shipposition.y,ship.width/10.0f,ship.height/10.0f};
            Vector2 originrec = {ship.width/20.0f, ship.height/20.0f};
            DrawTexturePro(ship, sourceShip, destrec, originrec, 57.2958*shipangle, WHITE);

            DrawText("Playground", 10, 10, 20, GRAY);
            DrawText(TextFormat("Facing angle: %03f", 57.2958*shipangle), 10, 50, 10, GRAY);
            DrawText(TextFormat("Ship velocity x: %03f", shipvelocities.x), 10, 70, 10, GRAY);
            DrawText(TextFormat("Ship velocity y: %03f", shipvelocities.y), 10, 90, 10, GRAY);

        EndDrawing();

    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}
