#ifndef SHIP_H
#define SHIP_H

#define drag 0.99
#define accel 0.5
#define angaccel 0.01
#define maxang 0.1
#define angdrag 0.9
#define collrad 20 

using namespace std;

enum class SensorType {
            Velocidade,
            //ObjetoDetectado,
            //ObjetoDx,
            //ObjetoDy,
            // adicionar mais dps
            COUNT
        };
    
class Ship{
private:

    Texture2D texture;
    Rectangle source;
    int screenHeight;
    int screenWidth;

public:

    Vector2 position;              //posiçao x,y
    Vector2 speeds;                //velocidade x,y
    float speed_angle;             //angulo da velocidade (rad)
    float facing_angle;            //angulo da nave (rad)
    float angularvelocity;         //velocidade angular da nave (rad)
    float max_angular_velocity;    //max velocidade angular
    float abs_speed;               //modulo da velocidade
    float acceleration;            //aceleracao da nave
    float angular_acceleration;    //acelaracao angular da nave
    float collisionradius;         //raio de colisao da nave
    float distancemoved;           //distancia percorrida pela nave
    
    //construtor

    Ship(Texture2D ship_texture) : texture(ship_texture) {};

    Ship(float x, float y, int windowWidth, int windowHeight, Texture2D ship_texture) 
        : texture(ship_texture)
    {
        source = { 0, 0, texture.width/1.0f, texture.height/1.0f };
        position = { x , y };
        distancemoved = 0;
        speed_angle = 0;
        angularvelocity = 0;
        facing_angle = 0;
        speeds = { 0 , 0 };
        max_angular_velocity = maxang;
        acceleration = accel;
        abs_speed = 0;
        angular_acceleration = angaccel;
        screenHeight = windowHeight;
        screenWidth = windowWidth;
        collisionradius = collrad;
    }
    
    void update(){
        //update positions
        position.x += speeds.x;
        position.y += speeds.y;

        //update angular velocity and speeds
        facing_angle += angularvelocity;
        angularvelocity *= angdrag;
        speeds = { speeds.x*(float)drag, speeds.y*(float)drag };
        if(facing_angle > 360*DEG2RAD) facing_angle -= DEG2RAD*360.0;
        if(facing_angle < 0) facing_angle += DEG2RAD*360.0;
        abs_speed = sqrt(speeds.x*speeds.x + speeds.y*speeds.y);
        distancemoved += abs_speed;
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

        //cap ship angular velocity
        if(angularvelocity > max_angular_velocity) angularvelocity = max_angular_velocity;
        if(angularvelocity < -max_angular_velocity) angularvelocity = -max_angular_velocity;
    }

    vector<double> scan_inputs(){
        vector<double> output(4);
        output = {0,0,0,0};

        if(IsKeyDown(KEY_DOWN)){
            output[0] = 1;
        }
        if(IsKeyDown(KEY_UP)){
            output[1] = 1;
        }
        if(IsKeyDown(KEY_RIGHT)){
            output[2] = 1;
        }
        if(IsKeyDown(KEY_LEFT)){
            output[3] = 1;
        }

        return(output);
    }

    void movement(vector<double> inputs){
        // Rotação para direita
        if (inputs[2] > 0.25) angularvelocity += angular_acceleration;
        
        // Rotação para esquerda
        if (inputs[3] > 0.25) angularvelocity -= angular_acceleration;
        
        // Acelerar
        if (inputs[1] > 0.25){
            speeds.x += acceleration*cosf(facing_angle);
            speeds.y += acceleration*sinf(facing_angle);
        }
        
        // Freiar
        if (inputs[0] > 0.25){
            speeds.x -= acceleration*cosf(facing_angle);
            speeds.y -= acceleration*sinf(facing_angle);
        }
    }

    void Draw(){
        Rectangle dest = {position.x, position.y,collisionradius*2.5f,collisionradius*2.0f};
        Vector2 origin = {dest.width/2.0f, dest.height/2.0f};
        
        //Lógica de alinhamento da nave
        float visual_angle;
        if(abs_speed > 0.5f){
            visual_angle = speed_angle;
        }
        else{
            visual_angle = facing_angle;
        }

        DrawTexturePro(texture, source, dest, origin, RAD2DEG*visual_angle, WHITE);
    }

    void DrawExtra(){
        Vector2 velocity_endpoint = {position.x + 10*speeds.x, position.y + 10*speeds.y};
        Vector2 arrowhead_1 = {velocity_endpoint.x - abs_speed*cosf(speed_angle+DEG2RAD*30.0), velocity_endpoint.y - abs_speed*sinf(speed_angle+DEG2RAD*30)};
        Vector2 arrowhead_2 = {velocity_endpoint.x - abs_speed*cosf(speed_angle-DEG2RAD*30.0), velocity_endpoint.y - abs_speed*sinf(speed_angle-DEG2RAD*30)};
        Vector2 arrowhead_3 = {velocity_endpoint.x, velocity_endpoint.y};
        DrawLineV(position, velocity_endpoint, RED);
        DrawTriangle(arrowhead_1, arrowhead_2, arrowhead_3, RED);
        
        DrawCircleLines(position.x, position.y, collisionradius, RED);

        //Lógica de alinhamento do sensor
        float sensor_angle;
        if(abs_speed > 0.5f){
            sensor_angle = speed_angle;
        }
        else{
            sensor_angle = facing_angle;
        }

        DrawCircleSector(position, 250, sensor_angle*RAD2DEG+30, sensor_angle*RAD2DEG, 15, Color{10,120,200,100});
        DrawCircleSector(position, 250, sensor_angle*RAD2DEG, sensor_angle*RAD2DEG-30, 15, Color{200,10,120,100});
    }

    vector<double> getSensors() const {
        vector<double> s((size_t)SensorType::COUNT);

        s[(int)SensorType::Velocidade]     = abs_speed;
        //s[(int)SensorType::ObjetoDetectado]= objetoDetectado ? 1.0 : 0.0;
        //s[(int)SensorType::ObjetoDx]       = objetoDx;
        //s[(int)SensorType::ObjetoDy]       = objetoDy;

        return s;
    }



};

#endif