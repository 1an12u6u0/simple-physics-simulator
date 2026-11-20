#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

const int ScreenWidth =1400;
const int ScreenHeight= 900;

const int FloorHeight=ScreenHeight-100;

#include <string>
using std::string;

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

        Color BallColor=GetColor(GuiGetStyle(DEFAULT,LINE_COLOR));
        Color TrajectoryColor=RAYWHITE;
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

        }




        //made by AI
        void DrawReadouts(){
            int   a = Launched ? launchAngle : angle;
            float s = Launched ? launchV     : v;
            float rad = ConvertToRadians(a);

            float vx  = s*cos(rad);          // constant the whole flight
            float vy0 = s*sin(rad);

            // predicted values (they only depend on the launch)
            float T = 2*vy0/g;               // flight time
            float H = vy0*vy0/(2*g);         // max height
            float R = vx*T;                  // range

            // live values (they depend on elapsed time)
            float t     = Launched ? ETime : 0.0f;
            float vy    = vy0 - g*t;
            float speed = sqrt(vx*vx + vy*vy);
            float x     = vx*t;
            float y     = vy0*t - (g*t*t)/2.0f;

            std::string lines[] = {
                TextFormat("Flight time: %.2f s", T),
                TextFormat("Max height:  %.2f m", H),
                TextFormat("Range:       %.2f m", R),
                TextFormat("Time:        %.2f s", t),
                TextFormat("Position:    (%.1f, %.1f) m", x, y),
                TextFormat("Speed:       %.2f m/s", speed),
            };
            for(int i=0;i<6;i++)
                DrawText(lines[i].c_str(), 20, 20+i*24, 20, GetColor(GuiGetStyle(DEFAULT,TEXT)));
        }
        //made by AI






        //calculate the time needed for the ball to collide with the ground;
        float TotalTime(const Vector2 v0){
            return (2*v0.y)/g;
        }


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

            //make this dynamic bc in the start there are too little dots
            //and in the end too many
            int N=std::max(10,int(remainingT/0.05f));

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

                DrawCircleV(TPos,2,GRAY);
            }
        }

        void draw(){
            DrawTrajectory();
            DrawCircleV(pos,r,BallColor);
            DrawReadouts();
        }

        //functions that allow the update of the angle and velocity
        void updateAngleVal(int val){ angle=val; };
        void updatev(float nv){v=nv;};

        //get the startpos of ball
        Vector2 GetStartPos(){ return StartPos; };
};

class Input{
    private:
        //sliders
        Rectangle AngleRec={20,ScreenHeight-50,200,30};
        float AngleVal=50;
        Rectangle vRec={300,ScreenHeight-50,200,30};
        float v=19.0f;

    public:
        void draw(){
            //text
            string AngleTxt=TextFormat("Angle: %02i",int(AngleVal));
            DrawText(
                AngleTxt.c_str(),
                AngleRec.x+(AngleRec.width-MeasureText(AngleTxt.c_str(),20))/2,
                AngleRec.y-30,
                20,
                GetColor(GuiGetStyle(DEFAULT,TEXT))
            );

            string vTxt=TextFormat("Velocity: %02.02f",v);
            DrawText(
                vTxt.c_str(),
                vRec.x+(vRec.width-MeasureText(vTxt.c_str(),20))/2,
                vRec.y-30,
                20,
                GetColor(GuiGetStyle(DEFAULT,TEXT))
            );

            //angle slider
            GuiSlider(
                AngleRec,
                "1",
                "90",
                &AngleVal,
                1,
                90
            );
            //velocity slider
            GuiSlider(
                vRec,
                "1",
                "25",
                &v,
                1.0f,
                25.0f
            );
        }
        int GetAngleVal(){ return int(AngleVal); };
        float Getv(){ return v; };
};

//need to add origin so the grid isnt offseted
void DrawGrid(Vector2 origin){
    //draw a simple grid that helps viusalizing the px to m scale
    Color GridColor=Fade(LIGHTGRAY,0.3f);
    float StepPx=scale;

    int i=0;
    for(float x=origin.x;x<ScreenWidth;x+=StepPx,i++){
        DrawLine(x,0,x,float(FloorHeight),GridColor);
        DrawText(TextFormat("%i",i),x+2,FloorHeight+4,10,GridColor);
    }

    i=0;
    for(float y=origin.y;y>0;y-=StepPx,i++){
        DrawLine(0,y,ScreenWidth,y,GridColor);
        DrawText(TextFormat("%i",i),4,y-12,10,GridColor);
    }
}

//dont know how to do the web app thing :(

//written by AI
Ball* ball = nullptr;
Input* input = nullptr;

void UpdateDrawFrame(){
    ball->updateAngleVal(input->GetAngleVal());
    ball->updatev(input->Getv());
    ball->update();

    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT,BACKGROUND_COLOR)));
    DrawGrid(ball->GetStartPos());
    input->draw();
    DrawLine(0,FloorHeight,ScreenWidth,FloorHeight,GetColor(GuiGetStyle(DEFAULT,LINE_COLOR)));
    ball->draw();
    EndDrawing();
}

int main(){
    InitWindow(ScreenWidth, ScreenHeight, "simple physics simulator");
    GuiLoadStyle("resources/style_amber.rgs");

    ball = new Ball;
    input = new Input;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    while(!WindowShouldClose()) UpdateDrawFrame();
#endif

    delete ball;
    delete input;
    CloseWindow();
    return 0;
}
//written by AI