#include <iostream>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#ifndef BASIC_OBJECT_H
#define BASIC_OBJECT_H

class BasicObject{
    protected:
        std::string fontPath;
        Uint32 convertColorToUint32(SDL_Color color){
            return (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
        }
    public:
        int xPos, yPos;
        int width, height;
        int maxLength;
        std::string fontName;
        std::string text;
        SDL_Color textColor;
        SDL_Color backGroundColor;
        TTF_Font* font;
        int fontSize;

        BasicObject(int x, int y, int w, int h,SDL_Surface* screenSurface, SDL_Color textColor = {0,0,0,255}, SDL_Color backGroundColor = {255,255,255,255},std::string font = "Arial", int fontSize = 16){
            xPos = x;
            yPos = y;
            width = w;
            height = h;
            this->textColor = textColor;
            this->backGroundColor = backGroundColor;
            this->fontSize = fontSize;
            std::string path = "assets/";
            path += font;
            path += ".ttf";
            this->fontPath = path;
            this->font = TTF_OpenFont(fontPath.c_str(),fontSize);
            this->fontName = font;
            this->maxLength = (int)(width*1.5/fontSize);
        }
        ~BasicObject(){
        }
        BasicObject(const BasicObject& other){
            xPos = other.xPos;
            yPos = other.yPos;
            width = other.width;
            height = other.height;
            maxLength = other.maxLength;
            text = other.text;
            textColor = other.textColor;
            backGroundColor = other.backGroundColor;
            font = other.font;
            fontName = other.fontName;
            fontPath = other.fontPath;
            fontSize = other.fontSize;
        }
        void setFontSize(int newSize) { 
            this->fontSize = newSize;
            font = TTF_OpenFont(this->fontPath.c_str(),fontSize);
        }
        void setNewFont(std::string& fontName){
            this->fontName = fontName; 
            this->fontPath = "assets/"+fontName+".ttf";
            font = TTF_OpenFont(this->fontPath.c_str(),fontSize);
        }
        void setLength(int length) {this->maxLength = length;}
        inline void SetTextColor(SDL_Color color) {textColor = color;}
        inline void SetBackGroundColor(SDL_Color color) {backGroundColor = color;}
        bool Load(SDL_Surface* screenSurface){
            
            if(screenSurface == nullptr) return false;
            if (this->text.empty()) {
                this->text = " "; // Prazan string da ne bi došlo do greške
            }
            bool out;
            
            
            if (!this->text.empty()) {
                Uint32 bgColor = convertColorToUint32(backGroundColor);
                SDL_Surface* textSurface = TTF_RenderText_Blended(this->font, this->text.c_str(), this->textColor);
                SDL_Rect boxRect = {this->xPos, this->yPos, textSurface->w + 10, this->height};

                SDL_Rect textPosition = {this->xPos+5, this->yPos, this->width, this->height};
                SDL_FillRect(screenSurface, &boxRect, bgColor);
                textPosition.y += (this->height - textSurface->h) / 2; // Vertikalno centriranje

                out = SDL_BlitSurface(textSurface, NULL, screenSurface, &textPosition);
                SDL_FreeSurface(textSurface);
            }
            return out == 0 ? 1 : 0;
        }
};
#endif