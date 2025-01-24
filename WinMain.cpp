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
const Uint32 textColor = 0xD3D3D3FF;
const SDL_Color text = {0,0,0,255};

class Circle : SDL_Rect{
    public:
        double x;
        double y;
        double radius;
        Circle(double xCord, double yCord, double rad) : x(xCord), y(yCord), radius(rad){}
        ~Circle(){}
};
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
        void setNewFont(std::string fontName){
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
            SDL_Rect boxRect = {this->xPos, this->yPos, (int)((int)this->text.size()*this->fontSize*0.7), this->height};
            Uint32 bgColor = convertColorToUint32(backGroundColor);
            SDL_FillRect(screenSurface, &boxRect, bgColor);
            
            
            if (!this->text.empty()) {
                SDL_Surface* textSurface = TTF_RenderText_Blended(this->font, this->text.c_str(), this->textColor);
                SDL_Rect textPosition = {this->xPos, this->yPos, (int)((int)this->text.size()*this->fontSize), this->height};
                textPosition.x += (textSurface->w - this->width)/2;
                textPosition.y += (this->height - textSurface->h) / 2; // Vertikalno centriranje

                out = SDL_BlitSurface(textSurface, NULL, screenSurface, &textPosition);
                SDL_FreeSurface(textSurface);
            }
            return out == 0 ? 1 : 0;
        }
};
class Label : public BasicObject{
    public:
        Label(int x, int y,int w, int h,SDL_Surface* screenSurface, SDL_Color textColor = {0,0,0,255},
            SDL_Color backGroundColor = {255,255,255,255},std::string font = "Arial",
            int fontSize = 16, std::string text = " ") : BasicObject(x,y,w,h,screenSurface,textColor,backGroundColor,font,fontSize){

                SDL_Rect boxRect = {this->xPos, this->yPos, this->width, this->height};
                Uint32 bgColor = convertColorToUint32(backGroundColor);
                SDL_FillRect(screenSurface, &boxRect, bgColor);
                this->text = text;
                if (!this->text.empty()) {
                    SDL_Surface* textSurface = TTF_RenderText_Blended(this->font, this->text.c_str(), this->textColor);
                    SDL_Rect textPosition = {this->xPos + 5, this->yPos, this->width, this->height};
                    textPosition.y += (this->height - textSurface->h) / 2;

                    SDL_BlitSurface(textSurface, NULL, screenSurface, &textPosition);
                    SDL_FreeSurface(textSurface);
                }
            }
};
class TextBox : public BasicObject{
    public:
        bool focused;
        SDL_Color focusColor;
        inline void SetFocusColor(SDL_Color color) {focusColor = color;}
        TextBox(int x, int y,int w, int h,SDL_Surface* screenSurface, SDL_Color textColor = {0,0,0,255},
                SDL_Color backGroundColor = {255,255,255,255},SDL_Color focusColor = {0xD3,0xD3,0xD3,255},
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
                this->text = " "; // Prazan string da ne bi došlo do greške
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
                    this->focusColor.a == other.focusColor.a); // ili koristi odgovarajuću funkciju za poređenje fontova
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

bool CheckIfInTxt(SDL_Event event, TextBox textBox){
    double mouseX = event.motion.x;
    double mouseY = event.motion.y;
    std::cout << "Checking..." << textBox.xPos << " " << textBox.yPos << " " << event.motion.x << " " << event.motion.y <<std::endl;
    return (mouseX >= textBox.xPos && mouseX <= textBox.width + textBox.xPos) && (mouseY >= textBox.yPos && mouseY <= textBox.height + textBox.yPos);
}

void FocusTextBox(std::vector<TextBox> textBoxes,TextBox* tb, bool willFocus){
    for(int i = 0;i<textBoxes.size();i++){
        if(textBoxes[i] == *tb){
            tb->focused = willFocus;
        }else{
            textBoxes[i].focused = false;
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

    Circle krug = Circle(200,200,80);
    std::cout << "Prosao circle\n";

    bool exitCondition = true;

    SDL_Event event;

    std::cout << "Prosao Event\n";
    SDL_Rect clearScreen = {0,0,width,height};
    SDL_Rect txtBox0 = {640,10,160,30};
    std::cout << "Prosao txtBox0\n";

    //ADDING TEXTBOXES
    std::vector<TextBox> textBoxes;
    std::cout << "Prosao vectorTextBoxes\n";
    TextBox tb = TextBox(600,10,160,30,surface); 
    TextBox tb1 = TextBox(600,50,160,30,surface);

    Label label1 = Label(600,100,80,20,surface,{255,0,0,255},{255,128,128,255},"Arial",16, "OVO JE LABELA");
    std::cout << "Prosao kreiranje textBox-a\n";
    textBoxes.push_back(tb);
    textBoxes.push_back(tb1);

    std :: cout << "izbacio tb";
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
                    if(CheckIfInTxt(event,textBoxes[i])){
                        //NASTAVI DA RADIS I PROMENI SAD DA
                        //SVE STO JE SA LISTAMA TEXTNBOX DA BUDE
                        //KLASA TEXTBOX.
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
                                textBoxes[i].text.pop_back();
                                std::cout << "Obrisao: " << textBoxes[i].text << std::endl;
                            }
                        }
                        if(event.key.keysym.sym == SDLK_RETURN){
                            std::cout << "TextBox" << i << ":" << textBoxes[i].text << std::endl;
                            textBoxes[i].focused = 0;
                        }
                    }
                }
            }
            if (event.type == SDL_TEXTINPUT) {
                for (int i = 0; i < textBoxes.size(); i++) {
                    if(textBoxes[i].focused){
                        if(textBoxes[i].text.size() < textBoxes[i].maxLength){
                            textBoxes[i].text += event.text.text;
                            std::cout << "Pisem: " << event.text.text << std::endl;
                        }
                        break;
                    }
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
        for (int i = 0; i < textBoxes.size(); i++) {
            textBoxes[i].Load(surface);
        }
        label1.Load(surface);
        SDL_UpdateWindowSurface(window);
        SDL_Delay(20);
    }
    std::cout<<"Kraj aplikacije."<< std::endl;
}