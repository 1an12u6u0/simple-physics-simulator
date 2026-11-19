#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include <raymath.h>
#include <iostream>
using std::cerr;

const int ScreenWidth =1400;
const int ScreenHeight= 900;

const int FloorHeight=ScreenHeight-100;


#include <vector>

//watched https://www.youtube.com/watch?v=7knY1U-ysY4

const float g=9.81f; // gravity  m/s^2 or N/kg

enum class ConvertMode{
    m_to_px,
    px_to_m
};

float ConvertToRadians(float deg){ return deg*(PI/180.0f); };

//convertion scale 1m ---> 20px
const float scale=20.0f;
float Convert(ConvertMode mode,float val){
    if(mode==ConvertMode::m_to_px) return val*scale;
    else return val/scale;
}

class Ball{
    private:
        const float r=20.0f;

        Vector2 StartPos={60.0f,FloorHeight-r}; //the x and y pos as a vector
        Vector2 pos=StartPos;//current ballpos
        int angle=70; //angle at which ball is thrown
        float v=25.0f;   //the velocity,needs to be positive

        bool Launched=false;
        float ETime=0.0f;
    public:
        //formulas used http://hyperphysics.phy-astr.gsu.edu/hbase/traj.html

        void update(){
            //when space is pressed launch ball
            if(IsKeyPressed(KEY_SPACE)){
                Launched=true;
                ETime=0.0f;
                pos=StartPos;
            };

            if(Launched){
                //update the inair time
                ETime+=GetFrameTime();

                //convert to radians
                float rad=ConvertToRadians(angle);
                Vector2 v0={
                    float(v*cos(rad)),
                    float(v*sin(rad))
                };

                float totalT=TotalTime(v0);

                //stop when elapsed time is bigger than total time
                //this meas the ball has hit the ground

                if(ETime>=totalT){
                    ETime=totalT;
                    Launched=false;
                }

                //calculate pos at each time
                Vector2 d={
                    //convert the results to px
                    // v0.x*ETime
                    // v0.y*ETime-(g*ETime*ETime)/2.0f
                    Convert(ConvertMode::m_to_px,v0.x*ETime),
                    Convert(ConvertMode::m_to_px,v0.y*ETime-(g*ETime*ETime)/2.0f)
                };

                //update the current pos
                pos.x=StartPos.x+d.x;
                pos.y=StartPos.y-d.y;


            }

        }


        //calculate the time needed for the ball to collide with the ground;
        float TotalTime(const Vector2 v0){
            return (2*v0.y)/g;
        }

        int N=100;

        void DrawTrajectory(){
            //without drag

            //convert degrees to radians
            float rad=ConvertToRadians(angle);

            //convert the angle and velocity to a v0 Vector
            Vector2 v0={
                float(v*cos(rad)),
                float(v*sin(rad))
            };

            //time step so we know how much to loop
            //for more accuracy we increase the N
            //and for less accuracy we decrease it

            float DeltaT=TotalTime(v0)/N;
            Vector2 TPos=StartPos;//separate trajectory pos so when the program calculates
                                // the balls trajectory it doesnt use the balls actual pos

            for(size_t i=0;i<=N;i++){
                float T=i*DeltaT;
                
                float dx_m=v0.x*T;
                float dy_m=v0.y*T-(g*T*T)/2.0f;

                TPos={
                    StartPos.x+Convert(ConvertMode::m_to_px,dx_m),
                    StartPos.y-Convert(ConvertMode::m_to_px,dy_m)
                };

                DrawCircleV(TPos,2,BLUE);
            }
        }

        void draw(){
            DrawCircleV(pos,r,RED);
            DrawTrajectory();
        }
};


int main(){
    Ball ball;

    InitWindow(
        ScreenWidth,
        ScreenHeight,
        "simple physics simulator"
    );
    SetTargetFPS(60);

    while(!WindowShouldClose()){
        ball.update();

        BeginDrawing();
        
        ClearBackground(WHITE);
        ball.draw();
        DrawLine(0,FloorHeight,ScreenWidth,FloorHeight,RED);
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}