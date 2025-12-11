#ifndef FITGRAPH
#define FITGRAPH
#include <raylib.h>
#include <vector>

class Fitness_graph{
    private:
    std::vector<float> fit_history;
    int x;
    int y;
    int w;
    int h;
    
    public:
    // constructor -----------------------------------
    Fitness_graph(int posx, int posy, int width, int height)
    :x(posx),
    y(posy),
    w(width),
    h(height)
    {

    }

    // methods --------------------------------------

    void add_datapoint(float data){
        fit_history.push_back(data);
    }

    void Draw() {
    // Semi-transparent background
        DrawRectangle(x, y, w, h, Fade(BLACK, 0.6f));
        DrawRectangleLines(x, y, w, h, DARKGREEN);
        
        if (fit_history.empty()) return;

        // Finds the maximum value for scaling
        float max_val = 0;
        for (float v : fit_history) if (v > max_val) max_val = v;
        if (max_val < 10) max_val = 10; 

        // Draws lines connecting the points
        for (size_t i = 0; i < fit_history.size() - 1; i++) {
            float val1 = fit_history[i];
            float val2 = fit_history[i+1];

            // X Coordinates
            int x1 = x + (int)((float)i / (fit_history.size() - 1) * w);
            int x2 = x + (int)((float)(i + 1) / (fit_history.size() - 1) * w);
            
            if(fit_history.size() == 1) { x1 = x; x2 = x + w; }

            // Y Coordinates
            int y1 = y + h - (int)((val1 / max_val) * (h - 20)) - 10;
            int y2 = y + h - (int)((val2 / max_val) * (h - 20)) - 10;

            DrawLine(x1, y1, x2, y2, GREEN);
        }

        DrawText("FITNESS HISTORY", x + 5, y + 5, 10, GREEN);
        DrawText(TextFormat("Max: %.0f", max_val), x + 5, y + 20, 10, LIME);
}

};

#endif