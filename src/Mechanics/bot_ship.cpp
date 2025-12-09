#ifndef BOTSHIP
#define BOTSHIP

#include <raylib.h>
#include <vector>
#include <cmath>
#include "entity.cpp"
#include "ship.cpp"

using namespace std;

class BotShip : public Ship {
    private:

    float fovradius;   // prov para bot_nave
    bool fov_detect;  //prov, remover dps

    public:
    
    // constructor -----------------------------------------------------------------------------
    BotShip(float x, float y, int window_w, int window_h, Texture2D& ship_tex, unsigned int id)
    : Ship(x,y,window_w,window_h,ship_tex,id)
    {
        type                   = 2;
        collisionradius        = 18;

    }
    // setters e getters ------------------------------------------------------------------------
    void set_melhor_nave(){
        eh_o_melhor = true;
    }

    // methods ----------------------------------------------------------------------------------

};

#endif