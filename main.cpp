#include "include/SDL.h"

int main(int argc, char *argv[]){

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("2.5D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 650, 0);

    SDL_Renderer * render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;

    SDL_Event event;

    SDL_Rect rect = {0, 0, 50, 50};

    while(!quit){

        while(SDL_PollEvent(&event) != 0){
            if(event.type == SDL_QUIT){
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);

        SDL_SetRenderDrawColor( render, 0x00, 0xFF, 0x00, 0xFF );
        SDL_RenderFillRect(render, &rect);

        SDL_RenderPresent(render);

    }

    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}