#include <iostream>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <BasicObject.h>

class Label : public BasicObject{
    public:
        Label(int x, int y,int w, int h,SDL_Surface* screenSurface, SDL_Color textColor = {0,0,0,255},
            SDL_Color backGroundColor = {255,255,255,255},std::string font = "Arial",
            int fontSize = 16, std::string text = "Boban") : BasicObject(x,y,w,h,screenSurface,textColor,backGroundColor,font,fontSize){

                SDL_Rect boxRect = {this->xPos, this->yPos, this->width, this->height};
                Uint32 bgColor = convertColorToUint32(backGroundColor);
                SDL_FillRect(screenSurface, &boxRect, bgColor);
                this->text = text;
                if (!this->text.empty()) {
                    SDL_Surface* textSurface = TTF_RenderText_Blended(this->font, this->text.c_str(), this->textColor);
                    SDL_Rect textPosition = {this->xPos, this->yPos, this->width, this->height};
                    textPosition.y += (this->height - textSurface->h) / 2;

                    SDL_BlitSurface(textSurface, NULL, screenSurface, &textPosition);
                    SDL_FreeSurface(textSurface);
                }
            }
};