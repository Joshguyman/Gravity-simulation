#include <raylib.h>
#include <ctime>
#include <iostream>
#include <raymath.h>
#include <vector>
#include <deque>
#include <string>
using namespace std;
const double UniverseGravConst = 6.674e-11;


class Circle {
   double radius;
   Vector2 position;
   Color color;
   Vector2 vel;
   double mass;
   deque<Vector2> points;
   Vector2 force;
   bool selected;

   public:
   void DrawMyCircle() {
       DrawCircle(position.x, position.y, radius, color);
   }
   void SetVel(Vector2 nums){
       vel = nums;
   }
   Vector2 GetVel(){
       return vel;
   }
   Vector2 GetPosition(){
       return position;
   }
   double GetRadius(){
       return radius;
   }
   void SetSelect(bool tf){
       selected = tf;
   }
   bool GetSelect() {
       return selected;
   }
   void SetForce(Vector2 f) {
       force = f;
   }
   void UpdatePosition(double DT) {
       position.x += vel.x * DT;
       position.y += vel.y * DT;
       points.push_back(position);
       if(points.size() == 2000){
           points.pop_front();
       }
   }
   void DrawVel(){
       DrawLine(position.x, position.y, position.x + vel.x, position.y, GREEN);
       DrawLine(position.x, position.y, position.x, position.y + vel.y, YELLOW);
       DrawLine(position.x, position.y, position.x + vel.x, position.y + vel.y, ORANGE);
   }
   void DrawAccel() {
       Vector2 acceleration = Vector2Scale(force, 1.0/mass);
       DrawLine(position.x, position.y, position.x + acceleration.x, position.y + acceleration.y, RAYWHITE);
   }
   void DrawSelected() {
       DrawCircleLines(position.x, position.y, radius * 1.25, PINK);
   }
   void CalculateForces(Circle circle){
       double distance = Vector2Distance(position, circle.position);
       double f = UniverseGravConst * (mass * circle.mass) / (distance * distance);
       Vector2 gravForce = Vector2Scale(Vector2Normalize(Vector2Subtract(circle.position, position)), f);
   
       force = Vector2Add(force, gravForce);
       cout << force.x << endl;
   }
   void UpdateVel(double DT) {
       Vector2 acceleration = Vector2Scale(force, 1.0/mass);
       vel.x += (acceleration.x * DT);
       vel.y += (acceleration.y * DT);
   } 
   void DrawPoints() {
       for(int i = 0; i < points.size(); i++) {
           DrawPixelV(points[i], color);
       }
   }
   void SnapCameraTo(Camera2D *c) {
       c->target = position; 
   }
   Circle(Vector2 p, double r, double m, Vector2 v, Color c){
       position = p;
       vel = v;
       radius = r;
       color = c;
       mass = m;
       points = {};
       force = {0, 0};
       selected = false;
       
   }
};
class Button {
    Vector2 position;
    Vector2 size;
    Color color;
    Color currentColor;
    string text;
    string updateText;
    string currentText;
    bool buttonState;
    bool uiSelect;
    int fontSize;
    Font font;
    Vector2 textSize;
    
    public:
    void DrawButton(){
        DrawRectangleV(position, size, currentColor);
        DrawTextEx(font, currentText.c_str(), {position.x + (size.x - textSize.x)/2 , position.y + (size.y - textSize.y)/2}, fontSize, 1.0, LIGHTGRAY);
    }
    bool UpdateButton() {
        if(CheckCollisionPointRec(GetMousePosition(), ButtonToRect())){
            SetColor({ 100, 120, 80, 255 });
            uiSelect = true;
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !buttonState){
                buttonState = true;
                SetText(updateText);
            }
            else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && buttonState) {
                buttonState = false;
                SetText(text);
            }
        }else {
            SetColor(color);
            uiSelect = false;
        }
        return uiSelect;
    }
    bool GetButtonState(){
        return buttonState;
    }
    Vector2 GetButtonSize(){
        return size;
    }
    void SetButtonState(bool state) {
        buttonState = state;
    }
    void SetText(string t) {
        currentText = t;
        textSize = MeasureTextEx(font, t.c_str(), 40, 1);
    }
    void SetColor(Color c){
        currentColor = c;
    }
    Rectangle ButtonToRect() {
        float positionx = position.x;
        float positiony = position.y;
        float sizex = size.x;
        float sizey = size.y;
        Rectangle button = { positionx, positiony, sizex, sizey };
        return button;
    }
    Button(Vector2 p, Vector2 s, string t, int fS, Color c, Font f, string nT){
        position = p;
        color = c;
        currentColor = c;
        text = t;
        updateText = nT;
        buttonState = false;
        uiSelect = false;
        fontSize = fS;
        font = f;
        textSize = MeasureTextEx(f, text.c_str(), 40, 1);
        size = s;
        currentText = t; 
    }
    
};

class Slider {
    Vector2 bodyPosition;
    Vector2 linePosition;
    Vector2 bodySize;
    Vector2 lineStart;
    Vector2 lineEnd;
    Vector2 circlePosition;
    float circleRadius;
    float lineThickness;
    Color bodyColor;
    Color lineColor;
    Color circleColor;
    bool sliderPressed;
    bool uiSelect;
    
    public:
    void DrawSlider() {
        DrawRectangleV(bodyPosition, bodySize, bodyColor);
        DrawLineEx(lineStart, lineEnd, lineThickness, lineColor);
        DrawCircleV(circlePosition, circleRadius, circleColor); 
    }
    bool UpdateCircle(Vector2 newPosition) {
        if(CheckCollisionPointCircle(GetMousePosition(), circlePosition, circleRadius) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            sliderPressed = true;
            uiSelect = true;
        }else if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && sliderPressed) {
            sliderPressed = false;
            uiSelect = false;
        }
        
        if(sliderPressed) {
            circlePosition.x = newPosition.x;
            if(circlePosition.x >= lineEnd.x) circlePosition.x = lineEnd.x;
            else if(circlePosition.x <= lineStart.x) circlePosition.x =lineStart.x;
        }
        return uiSelect;
    }
    Vector2 GetCirclePosition(){
        return circlePosition;
    }
    float GetCircleRadius() {
        return circleRadius;
    }
    Slider(Vector2 bP, Vector2 bS, Color bC, Color lC, Color cC){
        bodyPosition = bP;
        linePosition = {bodyPosition.x/2, bodyPosition.y/2};
        bodySize = bS;
        lineStart = {bodyPosition.x + 10, bodyPosition.y + bodySize.y/2};
        lineEnd = {bodyPosition.x + bodySize.x - 10, bodyPosition.y + bodySize.y/2};
        circlePosition = {bodyPosition.x + bodySize.x/2, bodyPosition.y + bodySize.y/2};
        circleRadius = 15;
        lineThickness = 3;
        bodyColor = bC;
        lineColor = lC;
        circleColor = cC;   
        sliderPressed = false;
        uiSelect = false;
    }
};
int main(void)
{
    const int screenWidth = 1600;
    const int screenHeight = 900;
    bool isPlanetSelected = false;
    bool isUiSelected = false;
    vector<Circle> Planets = { {{(screenWidth/2), (screenHeight/2)}, 20, 1.11e16, {0, 0}, SKYBLUE},
                               {{(screenWidth/2 - 240), screenHeight/2}, 10, 1e14, {0, 60}, RED},
                               {{(screenWidth/2 - 260), screenHeight/2}, 3, 1e5, {0, 75}, PURPLE},
                               {{(screenWidth/2 - 350), screenHeight/2}, 5, 1e2, {0, 50}, YELLOW}}; 

    InitWindow(screenWidth, screenHeight, "Grabity");
    
    Font defaultFont = GetFontDefault();
    Button PausePlay({screenWidth - 85, 5}, {50, 50}, ">", 40, DARKGRAY, defaultFont, "| |");
    Button AddPlanet({screenWidth - 85,  60}, {50, 50}, "+", 40, DARKGRAY, defaultFont, "-");
    Slider S1({screenWidth/4, 5}, {200, 35}, LIGHTGRAY, DARKGRAY, RED);
    
    Camera2D camera = { 0 };
    camera.target = Planets[0].GetPosition();
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 0.5f;

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        DrawFPS(0, 0); 
        float deltaTime = GetFrameTime(); 
        isUiSelected = false; 
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        PausePlay.DrawButton();
        AddPlanet.DrawButton();
        S1.DrawSlider();
        
        isUiSelected |= PausePlay.UpdateButton();
        isUiSelected |= AddPlanet.UpdateButton();
        isUiSelected |= S1.UpdateCircle(GetMousePosition());
        
        if(PausePlay.GetButtonState()){
            for(int i = 0; i < Planets.size(); i++){
                Planets[i].SetForce({0, 0});
                for(int j = 0; j < Planets.size(); j++){
                    if(i != j){
                        Planets[i].CalculateForces(Planets[j]);
                    }
                }
            }
        }
       camera.zoom += GetMouseWheelMove()/20;
       if(camera.zoom <= 0.01) camera.zoom = 0.01;
        
        BeginMode2D(camera);
        for(int i = 0; i < Planets.size(); i++){
            
            Planets[i].DrawPoints();
            if(PausePlay.GetButtonState()){
                Planets[i].UpdateVel(deltaTime);
                Planets[i].UpdatePosition(deltaTime);
            }
            Planets[i].DrawMyCircle();
            
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointCircle(GetScreenToWorld2D(GetMousePosition(), camera), Planets[i].GetPosition(), Planets[i].GetRadius())){
                for(int j = 0; j < Planets.size(); j++){
                    if(i != j) {
                        Planets[j].SetSelect(false);
                    }
                }
                Planets[i].SetSelect(true);
            }else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
                Planets[i].SetSelect(false);
            } 
            if(Planets[i].GetSelect()) {
                camera.target = Planets[i].GetPosition();
                Planets[i].DrawVel();
                Planets[i].DrawAccel();
                Planets[i].DrawSelected();
          
            }
        }
        for(int i = 0; i < Planets.size(); i++){
                if(Planets[i].GetSelect()) isPlanetSelected = true;
                else isPlanetSelected = false;
        }
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !isPlanetSelected && !isUiSelected){
            
            camera.target = Vector2Subtract(camera.target, Vector2Scale(GetMouseDelta(), 1/camera.zoom));
        }
    
        EndMode2D();
        EndDrawing();
    }
    CloseWindow();

    return 0;
}