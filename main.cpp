#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include <raymath.h>
#include <iostream>
using std::cerr;

#include <vector>

/*

to do list:
-take input as sliders and numbers
-gravity

*/

//watched https://www.youtube.com/watch?v=7knY1U-ysY4

const double g=9.81f; // gravity  m/s^2 or N/kg

void Trajectory(Vector2 &pos){
    //we are using the trajectory formula

    int x=pos.x;
    int y=pos.y;



}

class Ball{
    private:
        Vector2 pos={0.0f,0.0f}; //the x and y pos as a vector
        float angle=0.0f; //angle at which ball is thrown
        float v=0.0f;   //the velocity,needs to be positive

        Vector2 velocity={0.0f,0.0f};
    public:
        void Update(){

        }

        void CalculateTrajectory(){

        }
};


int main(){
    const int ScreenWidth =1400;
    const int ScreenHeight= 900;

    InitWindow(
        ScreenWidth,
        ScreenHeight,
        "simple physics simulator"
    );

    while(!WindowShouldClose()){
        BeginDrawing();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}