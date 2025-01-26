#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include <math.h>
#include "src/include/TextBox.h"
#include "src/include/Label.h"

const int width = 880;
const int height = 640;
const Uint32 whiteColor = 0xffffffff;
const Uint32 blackColor = 0x00000000;
const double gForce = 9.81;
const double maxSpeed = 20;
double elasticity = 0.8;
double friction = 0.99;
const Uint32 textColor = 0xD3D3D3FF;
const SDL_Color text = {0,0,0,255};
Uint32 ballColor;

class Circle : SDL_Rect{
    public:
        double x;
        double y;
        double radius;
        Circle(double xCord, double yCord, double rad) : x(xCord), y(yCord), radius(rad){}
        ~Circle(){}
        void FillCircle(SDL_Surface* surface, Uint32 color){
        double radiusSquared = this->radius*this->radius;
        for(double i = this->x - this->radius; i <= this->x + this->radius;i++){
            for(double j = this->y-this->radius;j<= this->y + this->radius;j++){
                //we are doing squared distance because circles are basically squares
                //so we are saving as much time calculating as possible to make refresh rate better.
                double distance = (this->x - i)*(this->x - i) + (this->y - j)*(this->y - j);
                if(distance <= radiusSquared){
                    SDL_Rect pixel = {(int)i,(int)j,1,1};
                    SDL_FillRect(surface,&pixel,color);
                }
            }
        }
    }

    void CalculatCoordinates(SDL_Event event, double w, double h){
        if (event.motion.x + this->radius >= w) {
            this->x = w - this->radius;
        } else if (event.motion.x - this->radius < 0) {
            this->x = this->radius;
        } else {
            this->x = event.motion.x;
        }
    
        if (event.motion.y + this->radius >= h) {
            this->y = h - this->radius;
        } else if (event.motion.y - this->radius < 0) {
            this->y = this->radius;
        } else {
            this->y = event.motion.y;
        }
    }    
};



bool CheckIfInTxt(SDL_Event event, TextBox textBox){
    double mouseX = event.motion.x;
    double mouseY = event.motion.y;
    return (mouseX >= textBox.xPos && mouseX <= textBox.width + textBox.xPos) && (mouseY >= textBox.yPos && mouseY <= textBox.height + textBox.yPos);
}

void FocusTextBox(std::vector<TextBox>& textBoxes,TextBox* tb, bool willFocus){
    for(int i = 0;i<textBoxes.size();i++){
        if(textBoxes[i] == *tb){
            tb->focused = willFocus;
        }else{
            textBoxes[i].focused = false;
        }
    }
}

int main(int argc, char *argv[]){
    double acceleration = gForce;
    double initialVelocity;
    double speedY = 0;
    double speedX = 0;
    bool falling = 0;
    bool sliding = 0;
    bool moving = 0;
    double firstPositionX;
    double firstPositionY;
    double lastPositionY;
    double lastPositionX;



    std::cout << "Pokretanje programa...\n";
    //INITS
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "SDL_Init greska: " << SDL_GetError() << std::endl;
        return -1;
    }
    std::cout << "Prosao Init...\n";

    if(TTF_Init() == -1){
        std::cout << "TTF_Init greska: " << SDL_GetError() << std::endl;
        return -1;
    }
    std::cout << "Prosao TTF..\n.";

    SDL_StartTextInput();
    std::cout << "Prosao StartTextInput...\n";
    double borderWidth = 600.0, borderHeight = height;
    SDL_Window* window = SDL_CreateWindow("Bouncing Ball",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,width,height,0);
    if (!window) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return -1;
    }
    std::cout << "Prosao Window...\n";

    TTF_Font* font = TTF_OpenFont("assets/Arial.ttf",16);
    if(!font){
        std::cout << "Greska pri otvaranju fonta! " << SDL_GetError() << std::endl;
        return -1;
    }
    std::cout << "Prosao Font...\n";

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    std::cout << "Prosao Surface...\n";

    Circle circle = Circle(200,200,80);
    std::cout << "Prosao circle\n";

    bool exitCondition = true;

    SDL_Event event;

    std::cout << "Prosao Event\n";
    SDL_Rect clearScreen = {0,0,width,height};
    SDL_Rect buttons = {600,0,width-(int)borderWidth,height};



    //ADDING TEXTBOXES
    std::vector<TextBox> textBoxes;
    std::vector<Label> labels;
    std::cout << "Prosao vectorTextBoxes\n";
    TextBox r = TextBox(620,30,60,30,surface,3); 
    TextBox g = TextBox(700,30,60,30,surface,3);
    TextBox b = TextBox(800,30,60,30,surface,3);
    r.text = "0";
    g.text = "0";
    b.text = "0";
    TextBox frictionTb = TextBox(620,100,60,30,surface,4);
    frictionTb.text = "0.";
    TextBox elasticityTb = TextBox(700,100,60,30,surface,4);
    elasticityTb.text = "0.";

    Label label1 = Label(620,10,40,20,surface,{0,0,0,255},{255,128,128,128},"Arial",16,"Ball RGB");
    Label label2 = Label(620,65,60,30,surface,{0,0,0,255},{255,128,128,128},"Arial",16,"Friction");
    Label label3 = Label(700,65,60,30,surface,{0,0,0,255},{255,128,128,128},"Arial",16,"Elasticity");
    std::cout << "Prosao kreiranje textBox-a\n";
    textBoxes.push_back(r);
    textBoxes.push_back(g);
    textBoxes.push_back(b);
    textBoxes.push_back(frictionTb);
    textBoxes.push_back(elasticityTb);
    for(int i = 0;i<3;i++){
        textBoxes[i].writeText();
    }

    labels.push_back(label1);
    labels.push_back(label2);
    labels.push_back(label3);

    std :: cout << "izbacio tb";
    while(exitCondition){
        
        // EVENT HANDLING

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                exitCondition = false;
            }
            if(event.type == SDL_MOUSEMOTION && event.motion.state != 0){
                circle.CalculatCoordinates(event,borderWidth,borderHeight);
                falling = false;
                sliding = false;
                moving = true;
            }
            if(event.type == SDL_MOUSEBUTTONDOWN){
                firstPositionX = event.motion.x;
                firstPositionY = event.motion.y;

                for(int i = 0;i<textBoxes.size();i++){
                    if(CheckIfInTxt(event,textBoxes[i])){
                        FocusTextBox(textBoxes,&textBoxes[i],1);
                        break;
                    }else{
                        FocusTextBox(textBoxes,&textBoxes[i],0);
                    }
                }
            }
            if(event.type == SDL_MOUSEBUTTONUP){
                lastPositionX = event.motion.x;
                speedX = (lastPositionX-firstPositionX) * 0.2;
                lastPositionY = event.motion.y;
                speedY = (lastPositionY-firstPositionY)*0.2;
                falling = true;
                sliding = true;
                moving = false;
            }
            if (event.type == SDL_KEYDOWN) {
                for (int i = 0; i < textBoxes.size(); i++) {
                    if (textBoxes[i].focused) {
                        if (event.key.keysym.sym == SDLK_BACKSPACE) {
                            if (!textBoxes[i].text.empty()) {
                                if(i == 3 || i == 4){
                                    if(textBoxes[i].text.size() > 2){
                                        textBoxes[i].text.pop_back();
                                    }
                                }else{
                                    textBoxes[i].text.pop_back();
                                }
                                std::cout << "Obrisao: " << textBoxes[i].text << std::endl;
                            }
                        }
                        if(event.key.keysym.sym == SDLK_RETURN){
                            if(i < 3){
                                if(std::stoi(textBoxes[i].text) > 255) textBoxes[i].text = "255";
                            }
                            if(i == 3) friction = std::stod(textBoxes[i].text);
                            if(i == 4) elasticity = std::stod(textBoxes[i].text);
                            std::cout << "Upisao u textbox" << i << "\n";
                            textBoxes[i].writeText();
                            textBoxes[i].focused = 0;
                        }
                    }
                }
            }
            if (event.type == SDL_TEXTINPUT) {
                for (int i = 0; i < textBoxes.size(); i++) {
                    if(textBoxes[i].focused){
                        if(textBoxes[i].text.size() <= textBoxes[i].maxLength){
                            textBoxes[i].text += event.text.text;
                            std::cout << "Pisem: " << event.text.text << std::endl;
                        }
                        break;
                    }
                }
            }
        }
        if(falling && !moving){
            if(circle.y+circle.radius < borderHeight && circle.y - circle.radius >= 0){
                speedY += acceleration * 0.02;
                circle.y += speedY;
            }
            else{
                if(circle.y+circle.radius >= borderHeight)
                    circle.y = borderHeight - circle.radius-1;
                if(circle.y-circle.radius <= 0)
                    circle.y = circle.radius;
                speedY *= -elasticity;
            }
            if(abs(speedY) <= 0.5 && circle.y+circle.radius >= borderHeight- 2)
                falling = false;
        }else{
            speedY = 0;
            if(!falling && !moving)
                circle.y = borderHeight - circle.radius;
        }
        if(sliding && !moving){
            if(circle.x+circle.radius <= borderWidth && circle.x-circle.radius >= 0){
                circle.x += speedX;
                speedX *= friction;
            }else{
                if(circle.x - circle.radius <= 0){
                    circle.x = circle.radius;
                }
                if(circle.x + circle.radius > borderWidth){
                    circle.x = borderWidth - circle.radius;
                }
                speedX *= -elasticity;
            }
            if(abs(speedX) <= 0.2)
                sliding = false;
        }else{
            speedX = 0;
            if(!sliding && !moving){
                if(circle.x+circle.radius >= borderWidth - 2) circle.x = borderWidth - circle.radius;
                if(circle.x-circle.radius <= 2) circle.x = 0;
            }
        }
        ballColor = (std::stoul(textBoxes[0].getText()) << 16) | (std::stoul(textBoxes[1].getText())<<8) | std::stoul(textBoxes[2].getText()) | 255<<24;
        //std::cout << ballColor << std::endl;
        //RENDERING
        SDL_FillRect(surface,&clearScreen,blackColor);
        circle.FillCircle(surface,ballColor);
        SDL_FillRect(surface,&buttons,0xFFA8A8A8);
        for (int i = 0; i < textBoxes.size(); i++) {
            textBoxes[i].Load(surface);
        }
        for(int i = 0; i < labels.size();i++){
            labels[i].Load(surface);
        }
        SDL_UpdateWindowSurface(window);
        SDL_Delay(20);
    }
    std::cout<<"Kraj aplikacije."<< std::endl;
}