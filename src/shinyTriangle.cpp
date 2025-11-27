
class Shinytriangle{
    private:

        float size;
        Vector2 v1,v2,v3;
        Vector2 v4,v5,v6;
        Vector2 tip;
        unsigned char a,b,c;
        int wobble;

    public:
        //construtor
        Shinytriangle(float posx, float posy, int radius, int wobbliness){
            size = radius;
            v1.x = posx + size*sqrt(3)/2;
            v1.y = posy - size*sqrt(3)/2;
            v2.x = posx;
            v2.y = posy + size*sqrt(3)/2;
            v3.x = posx - size*sqrt(3)/2;
            v3.y = posy - size*sqrt(3)/2;
            v6 = v3;
            v5 = v2;
            v4 = v1;
            tip = { posx , posy - size };
            wobble = wobbliness;
        }

        void Draw(){
            DrawTriangle(v6,v5,v4,Color{a,b,c,200});
            DrawTriangleLines(v6,v5,v4,BLACK);
            DrawLineV(tip,v6,BLACK);
            DrawLineV(tip,v5,BLACK);
            DrawLineV(tip,v4,BLACK);
        }

        void update(){
            a = sinf(GetTime()+1)*200;
            b = sinf(GetTime()+2)*200;
            c = sinf(GetTime()+3)*200;
            v6 = {v3.x + wobble*cosf(GetTime()+1), v3.y + wobble*sinf(GetTime()+1)};
            v5 = {v2.x + wobble*cosf(GetTime()+0.5), v2.y + wobble*sinf(GetTime()+0.5)};
            v4 = {v1.x + wobble*cosf(GetTime()+1.5), v1.y + wobble*sinf(GetTime()+1.5)};
        }
};