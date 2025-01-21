#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include <math.h>

const int width = 800;
const int height = 640;
const Uint32 whiteColor = 0xffffffff;
const Uint32 blackColor = 0x00000000;
const double gForce = 9.81;
const double maxSpeed = 20;
const double elasticity = 0.8;
const double friction = 0.99;
const Uint32 textColor = 0xFFD3D3D3;
const SDL_Color text = {0,0,0,255};

class Circle : SDL_Rect{
    public:
        double x;
        double y;
        double radius;
        Circle(double xCord, double yCord, double rad) : x(xCord), y(yCord), radius(rad){}
        ~Circle(){}
};

class Container{
    public:
        double x,y;
        double height, width;
        std::vector<SDL_Rect> objects;
        Container(double xPosition, double yPosition, double h, double w) : x(xPosition),y(yPosition),height(h),width(w){}
        inline void addObjects(SDL_Rect object) {objects.push_back(object);}
        bool removeObject(SDL_Rect* object){
            bool found = false;
            return found;
        }


};

void FillCircle(SDL_Surface* surface, Circle circle, Uint32 color){
    double radiusSquared = circle.radius*circle.radius;
    for(double i = circle.x - circle.radius; i <= circle.x + circle.radius;i++){
        for(double j = circle.y-circle.radius;j<=circle.y + circle.radius;j++){
            //we are doing squared distance because circles are basically squares
            //so we are saving as much time calculating as possible to make refresh rate better.
            double distance = (circle.x - i)*(circle.x - i) + (circle.y - j)*(circle.y - j);
            if(distance <= radiusSquared){
                SDL_Rect pixel = {(int)i,(int)j,1,1};
                SDL_FillRect(surface,&pixel,color);
            }
        }
    }
}

bool CheckIfInTxt(SDL_Event event, SDL_Rect textBox){
    double mouseX = event.motion.x;
    double mouseY = event.motion.y;
    std::cout << "Checking..." << textBox.x << " " << textBox.y << " " << event.motion.x << " " << event.motion.y <<std::endl;
    return (mouseX >= textBox.x && mouseX <= textBox.w + textBox.x) && (mouseY >= textBox.y && mouseY <= textBox.h + textBox.y);
}

void FocusTextBox(int txtBoxNum, std::vector<bool>* textBoxes){
    if(txtBoxNum < -1) return;
    if(txtBoxNum != -1)
        for(int i = 0;i<textBoxes->size();i++)
            textBoxes->at(i) = i == txtBoxNum ? true : false;
    else
        for(int i = 0;i<textBoxes->size();i++)
            textBoxes->at(i) = false;
}

void CalculatCoordinates(SDL_Event event, Circle* circle, SDL_Surface* surface){
    if (event.motion.x + circle->radius >= surface->w) {
        circle->x = surface->w - circle->radius;
    } else if (event.motion.x - circle->radius < 0) {
        circle->x = circle->radius;
    } else {
        circle->x = event.motion.x;
    }

    if (event.motion.y + circle->radius >= surface->h) {
        circle->y = surface->h - circle->radius;
    } else if (event.motion.y - circle->radius < 0) {
        circle->y = circle->radius;
    } else {
        circle->y = event.motion.y;
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

    //INITS
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_StartTextInput();

    SDL_Window* window = SDL_CreateWindow("Bouncing Ball",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,width,height,0);

    TTF_Font* font = TTF_OpenFont("assets/Arial.ttf",16);
    if(!font){
        std::cout << "Greska pri otvaranju fonta! " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    Circle krug = Circle(200,200,80);

    bool exitCondition = true;
    std::vector<bool> textBoxes(7,0);
    std::vector<std::string> textBoxesText(7,"");
    std::vector<SDL_Rect> textBoxList(7,{640,10,160,30});


    SDL_Event event;

    SDL_Rect clearScreen = {0,0,width,height};
    SDL_Rect txtBox0 = {640,10,160,30};

    while(exitCondition){
        
        // EVENT HANDLING

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                exitCondition = false;
            }
            if(event.type == SDL_MOUSEMOTION && event.motion.state != 0){
                CalculatCoordinates(event,&krug,surface);
                falling = false;
                sliding = false;
                moving = true;
            }
            if(event.type == SDL_MOUSEBUTTONDOWN){
                firstPositionX = event.motion.x;
                firstPositionY = event.motion.y;

                for(int i = 0;i<textBoxes.size();i++){
                    if(CheckIfInTxt(event,textBoxList[i])){
                        FocusTextBox(i,&textBoxes);
                        break;
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
                    if (textBoxes[i]) {
                        if (event.key.keysym.sym == SDLK_BACKSPACE) {
                            if (!textBoxesText[i].empty()) {
                                textBoxesText[i].pop_back();
                                std::cout << "Obrisao: " << textBoxesText[i] << std::endl;
                            }
                        }
                        if(event.key.keysym.sym == SDLK_RETURN){
                            std::cout << "TextBox" << i << ":" << textBoxesText[i] << std::endl;
                        }
                    }
                }
            }
            if (event.type == SDL_TEXTINPUT) {
                for (int i = 0; i < textBoxes.size(); i++) {
                    textBoxesText[i] += event.text.text;
                    std::cout << "Pisem: " << event.text.text << std::endl;
                    break;
                }
            }
        }
        if(falling && !moving){
            if(krug.y+krug.radius < surface->h && krug.y - krug.radius >= 0){
                speedY += acceleration * 0.02;
                krug.y += speedY;
            }
            else{
                if(krug.y+krug.radius >= surface->h)
                    krug.y = surface->h-krug.radius-1;
                if(krug.y-krug.radius <= 0)
                    krug.y = krug.radius;
                speedY *= -elasticity;
            }
            if(abs(speedY) <= 0.5 && krug.y+krug.radius >= surface->h - 2)
                falling = false;
        }else{
            speedY = 0;
            if(!falling && !moving)
                krug.y = surface->h - krug.radius;
        }
        if(sliding && !moving){
            if(krug.x+krug.radius <= surface->w && krug.x-krug.radius >= 0){
                krug.x += speedX;
                speedX *= friction;
            }else{
                if(krug.x - krug.radius <= 0){
                    krug.x = krug.radius;
                }
                if(krug.x + krug.radius > surface->w){
                    krug.x = surface->w - krug.radius;
                }
                speedX *= -elasticity;
            }
            if(abs(speedX) <= 0.2)
                sliding = false;
        }else{
            speedX = 0;
            if(!sliding && !moving){
                if(krug.x+krug.radius >= surface->w - 2) krug.x = surface->w - krug.radius;
                if(krug.x-krug.radius <= 2) krug.x = 0;
            }
        }
        //RENDERING
        SDL_FillRect(surface,&clearScreen,blackColor);
        FillCircle(surface,krug,whiteColor);
        SDL_FillRect(surface,&textBoxList[0],textBoxes[0] ? whiteColor : textColor);
        for(int i = 0;i<textBoxes.size();i++){
            if(!textBoxesText[i].empty()){
                SDL_Surface* textSurface = TTF_RenderText_Blended(font,textBoxesText[i].c_str(),{0,0,0,255});
                SDL_Rect textPosition = textBoxList[i];  
                textPosition.x += 5;
                textPosition.y += (textBoxList[i].h - textSurface->h) / 2;
                SDL_BlitSurface(textSurface,NULL,surface,&textPosition);
                SDL_FreeSurface(textSurface);
            }
        }
        SDL_UpdateWindowSurface(window);
        SDL_Delay(20);
    }
    std::cout<<"Kraj aplikacije."<< std::endl;
}