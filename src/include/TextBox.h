#include <iostream>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <BasicObject.h>

class TextBox : public BasicObject{
    private:
        std::string savedText;
    public:
        bool focused;
        SDL_Color focusColor;
        inline void SetFocusColor(SDL_Color color) {focusColor = color;}
        TextBox(int x, int y,int w, int h,SDL_Surface* screenSurface,int maxLength = 10, SDL_Color textColor = {0,0,0,255},
                SDL_Color backGroundColor = {255,255,255,255},SDL_Color focusColor = {255,0xD3,0xD3,0xD3},
                std::string font = "Arial", int fontSize = 16) : BasicObject(x,y,w,h,screenSurface,textColor,backGroundColor,font,fontSize){

            focused = false;
            this->focusColor = focusColor;
            SDL_Rect boxRect = {this->xPos, this->yPos, this->width, this->height};
            Uint32 bgColor = this->focused ? convertColorToUint32(focusColor) : convertColorToUint32(backGroundColor);
            SDL_FillRect(screenSurface, &boxRect, bgColor);
            this->text = " ";
            if (!this->text.empty()) {
                SDL_Surface* textSurface = TTF_RenderText_Blended(this->font, this->text.c_str(), this->textColor);
                SDL_Rect textPosition = {this->xPos + 5, this->yPos, this->width, this->height};
                textPosition.y += (this->height - textSurface->h) / 2; // Vertikalno centriranje

                SDL_BlitSurface(textSurface, NULL, screenSurface, &textPosition);
                SDL_FreeSurface(textSurface);
            }
            this->maxLength = maxLength;
        }
        void writeText(){
            this->savedText = this->text;
        }
        std::string getText(){
            return savedText;
        }
        ~TextBox(){

        }
        TextBox(const TextBox& other) : BasicObject(other){
            focused = other.focused;
            focusColor = other.focusColor;
        }

        bool Load(SDL_Surface* screenSurface){
            
            if(screenSurface == nullptr) return false;
            if (this->text.empty()) {
                this->text = " ";
            }
            bool out;
            SDL_Rect boxRect = {this->xPos, this->yPos, this->width, this->height};
            Uint32 bgColor = this->focused ? convertColorToUint32(focusColor) : convertColorToUint32(backGroundColor);
            SDL_FillRect(screenSurface, &boxRect, bgColor);
            
            
            if (!this->text.empty()) {
                SDL_Surface* textSurface = TTF_RenderText_Blended(this->font, this->text.c_str(), this->textColor);
                SDL_Rect textPosition = {this->xPos + 5, this->yPos, this->width, this->height};
                textPosition.y += (this->height - textSurface->h) / 2; // Vertikalno centriranje

                out = SDL_BlitSurface(textSurface, NULL, screenSurface, &textPosition);
                SDL_FreeSurface(textSurface);
            }
            return out == 0 ? 1 : 0;
        }
        bool operator==(const TextBox& other) const {
            return (this->xPos == other.xPos &&
                    this->yPos == other.yPos &&
                    this->width == other.width &&
                    this->height == other.height &&
                    this->focused == other.focused &&
                    this->text == other.text &&
                    this->textColor.r == other.textColor.r &&
                    this->textColor.g == other.textColor.g &&
                    this->textColor.b == other.textColor.b &&
                    this->textColor.a == other.textColor.a &&
                    this->backGroundColor.r == other.backGroundColor.r &&
                    this->backGroundColor.g == other.backGroundColor.g &&
                    this->backGroundColor.b == other.backGroundColor.b &&
                    this->backGroundColor.a == other.backGroundColor.a &&
                    this->focusColor.r == other.focusColor.r &&
                    this->focusColor.g == other.focusColor.g &&
                    this->focusColor.b == other.focusColor.b &&
                    this->focusColor.a == other.focusColor.a); 
        }


};