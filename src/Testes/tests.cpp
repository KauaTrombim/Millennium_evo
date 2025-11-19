#include "raylib.h"
#include <cmath>
#include <vector>
#include "testship.cpp"
#include "testasteroid.cpp"
#include "shinytriangle.cpp"

int main(void)
{
    // init
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "miya raylib playground");
    SetTargetFPS(60);

    // triangle things
    Shinytriangle test_triangle(screenWidth/2, screenHeight/2, 300, 100);
    // ship things
    Ship test_ship(screenWidth/2, screenHeight/2, screenWidth, screenHeight);
    // asteroid things
    std::vector<Asteroid> asteroids(10);
    for(int i=0; i<5; i++){
        Asteroid temp = Asteroid(1.0f*GetRandomValue(0,screenWidth),1.0f*GetRandomValue(0,screenHeight),GetRandomValue(50,150),screenWidth,screenHeight);
        asteroids[i] = temp;
    }

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {   
        //update triangle
        test_triangle.update();
        
        //update ship
        test_ship.scan_inputs();
        test_ship.update();

        //update asteroids
        for(int i=0; i<5; i++){
            asteroids[i].update();
        }

        BeginDrawing();

            ClearBackground(BLACK);

            //draw the triangle
            test_triangle.Draw();

            //draw the asteroids
            for(auto a : asteroids){
                a.Draw();
                a.DrawExtra();
            }

            //draw the ship
            test_ship.Draw();
            test_ship.DrawExtra();

            //draw info on screen
            DrawFPS(GetScreenWidth() - 95, 10);
            DrawText(TextFormat("Absolute Speed: %03.03f", test_ship.abs_speed), 10, 10, 20, WHITE);
            DrawText(TextFormat("Distance moved: %03.0f", test_ship.distancemoved), 10, 40, 20, WHITE);
            DrawText(TextFormat("Ship velocity x: %03.03f", test_ship.speeds.x), 10, 70, 10, WHITE);
            DrawText(TextFormat("Ship velocity y: %03.03f", test_ship.speeds.y), 10, 90, 10, WHITE);
            DrawText(TextFormat("Facing angle: %03.03f", RAD2DEG*test_ship.facing_angle), 10, 110, 10, WHITE);

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
