#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>

const int width = 800;
const int height = 640;
const Uint32 whiteColor = 0xffffffff;
const Uint32 blackColor = 0x00000000;
const double gForce = 9.81;
const double maxSpeed = 20;
const double elasticity = 0.8;
const double friction = 1;

class Circle{
    public:
        double x;
        double y;
        double radius;
        Circle(double xCord, double yCord, double rad) : x(xCord), y(yCord), radius(rad){}
        ~Circle(){}
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
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Bouncing Ball",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,width,height,0);
    
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    Circle krug = Circle(200,200,80);

    bool exitCondition = true;
    SDL_Event event;
    SDL_Rect clearScreen = {0,0,width,height};
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
    while(exitCondition){
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
                std::cout<<"First Position of X: " << firstPositionX << std::endl;
                std::cout<<"First Position of Y: " << firstPositionY << std::endl;
            }
            if(event.type == SDL_MOUSEBUTTONUP){
                lastPositionX = event.motion.x;
                speedX = (lastPositionX-firstPositionX) * 0.2;
                lastPositionY = event.motion.y;
                speedY = (lastPositionY-firstPositionY)*0.2;
                falling = true;
                sliding = true;
                moving = false;
                std::cout<<"Second Position: " << lastPositionX << std::endl;
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
        SDL_FillRect(surface,&clearScreen,blackColor);
        FillCircle(surface,krug,whiteColor);
        SDL_UpdateWindowSurface(window);
        SDL_Delay(20);
    }
    std::cout<<"Kraj aplikacije."<< std::endl;
}
