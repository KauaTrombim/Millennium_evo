#include "raylib.h"
#include <cmath>

int main(void)
{
    // init
    const int screenWidth = 1200;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "miya raylib playground");


    SetTargetFPS(60);
    const float pi = 3.1415;
    // triangle things
    float posv1x = screenWidth/2, posv1y = screenHeight/2, posv2x = 100.0, posv2y = screenHeight - 100.0;
    float posv3x = screenWidth - 100.0, posv3y = screenHeight - 100.0;
    unsigned char a = 0,b = 0,c = 0;

    //ship things
    Texture ship = LoadTexture("millenium.png");
    Rectangle sourceShip = { 0, 0, ship.width/1.0f, ship.height/1.0f };
    Vector2 originrec = {ship.width/20.0f, ship.height/20.0f};
    Vector2 shipposition = { screenWidth/2, screenHeight/2 };
    float ship_speed_angle = 0;
    float ship_facing_angle = 0;
    Vector2 shipspeeds = { 0, 0 };
    float shipangularvelocity = 0;
    float max_angular_velocity = 0.1;

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
        shipposition.x += shipspeeds.x;
        shipposition.y += shipspeeds.y;

        ship_facing_angle += shipangularvelocity;
        shipangularvelocity *= 0.9;
        shipspeeds = { shipspeeds.x*0.98f, shipspeeds.y*0.98f };
        ship_facing_angle = fmod(ship_facing_angle, 2*pi);

        //screen wrap
        if(shipposition.x > screenWidth) shipposition.x = 0;
        if(shipposition.x < 0) shipposition.x = screenWidth;
        if(shipposition.y > screenHeight) shipposition.y = 0;
        if(shipposition.y < 0) shipposition.y = screenHeight;

        if (IsKeyDown(KEY_RIGHT)) shipangularvelocity += 0.01;
        if (IsKeyDown(KEY_LEFT)) shipangularvelocity -= 0.01;
        if (IsKeyDown(KEY_UP)){
            shipspeeds.x += 0.5*cosf(ship_facing_angle);
            shipspeeds.y += 0.5*sinf(ship_facing_angle);
        }
        if (IsKeyDown(KEY_DOWN)){
            shipspeeds.x -= 0.5*cosf(ship_facing_angle);
            shipspeeds.y -= 0.5*sinf(ship_facing_angle);
        }

        if(shipangularvelocity > max_angular_velocity) shipangularvelocity = max_angular_velocity;
        if(shipangularvelocity < -max_angular_velocity) shipangularvelocity = -max_angular_velocity;

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
            
            //draw velocity vector and field of view
            float abs_speed = sqrt(shipspeeds.x*shipspeeds.x + shipspeeds.y*shipspeeds.y);
            
            Vector2 velocity_endpoint = {shipposition.x + 10*shipspeeds.x, shipposition.y + 10*shipspeeds.y};
            Vector2 direction = { velocity_endpoint.x - shipposition.x, velocity_endpoint.y - shipposition.y };
            ship_speed_angle = atan2f(direction.y, direction.x);
            DrawLineV(shipposition, velocity_endpoint, RED);
            Vector2 arrowhead_1 = {velocity_endpoint.x - abs_speed*cosf(ship_speed_angle+DEG2RAD*30.0), velocity_endpoint.y - abs_speed*sinf(ship_speed_angle+DEG2RAD*30)};
            Vector2 arrowhead_2 = {velocity_endpoint.x - abs_speed*cosf(ship_speed_angle-DEG2RAD*30.0), velocity_endpoint.y - abs_speed*sinf(ship_speed_angle-DEG2RAD*30)};
            Vector2 arrowhead_3 = {velocity_endpoint.x, velocity_endpoint.y};
            DrawTriangle(arrowhead_1, arrowhead_2, arrowhead_3, RED);
            DrawCircleSector(shipposition,250,ship_facing_angle*RAD2DEG+30,ship_facing_angle*RAD2DEG-30,15,Color{10,120,200,100});

            //draw the ship
            Rectangle destrec = {shipposition.x, shipposition.y,ship.width/10.0f,ship.height/10.0f};
            DrawTexturePro(ship, sourceShip, destrec, originrec, 57.2958*ship_facing_angle, WHITE);
            DrawText(TextFormat("%02i FPS", GetFPS()), 10, 10, 20, GRAY);
            DrawText(TextFormat("Facing angle: %03f", 57.2958*ship_facing_angle), 10, 50, 10, GRAY);
            DrawText(TextFormat("Ship velocity x: %03f", shipspeeds.x), 10, 70, 10, GRAY);
            DrawText(TextFormat("Ship velocity y: %03f", shipspeeds.y), 10, 90, 10, GRAY);

        EndDrawing();

    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}
