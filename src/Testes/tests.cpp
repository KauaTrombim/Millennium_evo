#include "raylib.h"
#include <cmath>
#include "testship.cpp"

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
    Ship test_ship(screenWidth/2, screenHeight/2, screenWidth, screenHeight);

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
        
        test_ship.scan_inputs();
        test_ship.update();

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
            
            test_ship.Draw();
            test_ship.DrawExtra();

            //draw info on screen
            DrawFPS(GetScreenWidth() - 95, 10);
            DrawText(TextFormat("Facing angle: %03f", RAD2DEG*test_ship.ship_facing_angle), 10, 50, 10, GRAY);
            DrawText(TextFormat("Ship velocity x: %03f", test_ship.shipspeeds.x), 10, 70, 10, GRAY);
            DrawText(TextFormat("Ship velocity y: %03f", test_ship.shipspeeds.y), 10, 90, 10, GRAY);

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
