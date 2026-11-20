#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

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

        //we need to lock the angle and velocity values
        //so the sliders dont change the outcome we expect
        int launchAngle=45;
        float launchV=25.0f;

        bool Launched=false;
        float ETime=0.0f;
    public:
        //formulas used http://hyperphysics.phy-astr.gsu.edu/hbase/traj.html

        void update(){
            //when space is pressed launch ball

            //only if ball isnt already launched
            if(!Launched&&IsKeyPressed(KEY_SPACE)){
                Launched=true;
                ETime=0.0f;
                pos=StartPos;

                //update the launch vars with the input
                launchV=v;
                launchAngle=angle;
            };

            if(Launched){
                //update the inair time
                ETime+=GetFrameTime();

                //convert to radians
                float rad=ConvertToRadians(launchAngle);
                Vector2 v0={
                    float(launchV*cos(rad)),
                    float(launchV*sin(rad))
                };

                float totalT=TotalTime(v0);

                //stop when elapsed time is bigger than total time
                //this means the ball has hit the ground

                if(ETime>=totalT){
                    ETime=totalT;
                    Launched=false;
                    pos=StartPos;
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
            else pos=StartPos;

        }


        //calculate the time needed for the ball to collide with the ground;
        float TotalTime(const Vector2 v0){
            return (2*v0.y)/g;
        }

        int N=100;

        void DrawTrajectory(){
            //now that we set up locked vals
            //we need to change the preview method

            //if the ball is launched we stick to the launch vals
            //else we use the users input to preview the trajectory
            int activeAngle=Launched?launchAngle:angle;
            float activeV=Launched?launchV:v;

            //without drag

            //convert degrees to radians
            float rad=ConvertToRadians(activeAngle);

            //convert the angle and velocity to a v0 Vector
            Vector2 v0={
                float(activeV*cos(rad)),
                float(activeV*sin(rad))
            };

            //time step so we know how much to loop
            //for more accuracy we increase the N
            //and for less accuracy we decrease it

            //we need to add an offset so the trajectory can update
            //if our ball is moving

            float totalT=TotalTime(v0);

            //if the ball is launched we start from ETime
            //else the ball hasnt started moving and we start from 0
            float startT=Launched?ETime:0.0f;
            float remainingT=totalT-startT;

            //return(stop) if the ball already stopped
            if(remainingT<=0.0f) return;

            float DeltaT=remainingT/N;

            Vector2 TPos=StartPos;//separate trajectory pos so when the program calculates
                                // the balls trajectory it doesnt use the balls actual pos

            for(size_t i=0;i<=N;i++){
                //aplly offset
                float T=startT+i*DeltaT;
                
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
            DrawTrajectory();
            DrawCircleV(pos,r,RED);
        }

        //functions that allow the update of the angle and velocity
        void updateAngleVal(int val){ angle=val; };
        void updatev(float nv){v=nv;};
};

class Input{
    private:
        Rectangle AngleRec={10,10,200,100};
        float AngleVal=0;
        Rectangle vRec={400,10,200,100};
        float v=0.0f;
    public:
        void draw(){
            //angle slider
            GuiSliderBar(
                AngleRec,
                "0",
                "90",
                &AngleVal,
                0,
                90
            );
            //velocity slider
            GuiSliderBar(
                vRec,
                "1",
                "30",
                &v,
                1.0f,
                30.0f
            );
        }
        int GetAngleVal(){ return int(AngleVal); };
        float Getv(){ return v; };
};

int main(){
    Ball ball;
    Input input;

    InitWindow(
        ScreenWidth,
        ScreenHeight,
        "simple physics simulator"
    );
    SetTargetFPS(60);

    while(!WindowShouldClose()){    
        //inputs ,then update, then draw

        ball.updateAngleVal(input.GetAngleVal());
        ball.updatev(input.Getv());


        ball.update();

        BeginDrawing();
        
        ClearBackground(WHITE);
        input.draw();
        //draw gorund line
        DrawLine(0,FloorHeight,ScreenWidth,FloorHeight,RED);
        ball.draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}