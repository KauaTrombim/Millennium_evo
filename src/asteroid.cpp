#ifndef ASTEROID_H
#define ASTEROID_H

#define maxspeed 50

class Asteroid{
    private: 
    Texture2D& texture;
    Rectangle source;
    int screenHeight;
    int screenWidth;

    public:
    
    Vector2 position;
    Vector2 speeds;
    float angular_velocity;
    float facing_angle;
    float speed_angle;
    float abs_speed;
    float collisionradius;

    Asteroid(float x, float y, int radius, int windowWidth, int windowHeight, Texture2D& asteroid_texture)
        : texture(asteroid_texture)
    {
        source = { 0.0f, 0.0f, 1.0f*texture.width, 1.0f*texture.height };
        position = { x , y };
        speeds = { 0.01f*GetRandomValue(-maxspeed,maxspeed) , 0.01f*GetRandomValue(-maxspeed,maxspeed) };
        facing_angle = (float)GetRandomValue(0,360);
        speed_angle = atan2f(speeds.y, speeds.x);
        texture = asteroid_texture;
        collisionradius = radius;
        screenHeight = windowHeight;
        screenWidth = windowWidth;
    }
    
    void update(){
        position.x += speeds.x;
        position.y += speeds.y;
        if(facing_angle > 360*DEG2RAD) facing_angle -= DEG2RAD*360.0;
        if(facing_angle < 0) facing_angle += DEG2RAD*360.0;
        abs_speed = sqrt(speeds.x*speeds.x + speeds.y*speeds.y);
        speed_angle = atan2f(speeds.y, speeds.x);

        //screen wrap
        /*if(position.x > screenWidth) position.x = 0;
        if(position.x < 0) position.x = screenWidth;
        if(position.y > screenHeight) position.y = 0;
        if(position.y < 0) position.y = screenHeight;*/
        
        //screen edge bounce
        if(position.x + speeds.x >= screenWidth || position.x + speeds.x <= 0){
                position.x -= speeds.x;
                speeds.x *= -1;
        }
        if(position.y + speeds.y >= screenHeight || position.y + speeds.y <= 0){
                position.y -= speeds.y;
                speeds.y *= -1;
        }
    }

    void Draw(){
        Rectangle dest = {position.x, position.y, collisionradius*2, collisionradius*2};
        Vector2 origin = {dest.width/2.0f, dest.height/2.0f};
        DrawTexturePro(texture, source, dest, origin, RAD2DEG*facing_angle, WHITE);
    }

    void DrawExtra(){
        DrawCircleLines(position.x, position.y, collisionradius, RED);
    }
};

#endif