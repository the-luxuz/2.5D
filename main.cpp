#include "include/SDL.h"
#include "iostream"

#define Window_Width 900
#define Window_Height 650

#define Wall_size 50
#define FOV 100

#define Max_Render 100000

#define map_x 10
#define map_y 10

#define PI 3.141592653589793238462643383279502884 

class Vector2{
    public:
        float x, y;
};

double check_walls(SDL_Renderer* render, Vector2& player_pos, int (&map)[map_y][map_x], float angle){
    
    double dist = 0.0f;
    double step = 0.5f;

    while(dist < Max_Render){

        float ray_x = player_pos.x + SDL_cosf(angle) * dist;
        float ray_y = player_pos.y + SDL_sinf(angle) * dist;

        int cell_pos_x = (int)(ray_x / Wall_size);
        int cell_pos_y = (int)(ray_y / Wall_size);

        // comment this if to see literaly the garbage memory of your computer :-)
        if(cell_pos_x < 0 || cell_pos_x >= map_x || cell_pos_y < 0 || cell_pos_y >= map_y) {
            return -1;
        }

        if(map[cell_pos_y][cell_pos_x] == 1){
            return dist;
        }
        dist += step;
    }

    return -1;

}

void draw_walls(SDL_Renderer* render, Vector2 player_pos, float player_angle, int (&map)[map_y][map_x]){

    float FOV_angle = FOV * PI / 180.0f;

    int column = 0;

    for(float i = player_angle - FOV_angle / 2; i < player_angle + FOV_angle / 2; i += FOV_angle / Window_Width){

        double distance = check_walls(render, player_pos, map, i);

        if(distance <= 0) {
            column++;
            continue;
        }

        int wall_height = (int)(Window_Height * 50 / distance);

        int start_y = Window_Height/2 - wall_height/2;

        SDL_Rect rect = { column, start_y, 1, wall_height };

        SDL_SetRenderDrawColor(render, 0x00, 0xFF, 0xFF, 0xFF);

        SDL_RenderFillRect(render, &rect);

        column++;
    }
}

int main(int argc, char *argv[]){

    int map[map_y][map_x] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
        1, 0, 0, 1, 0, 0, 0, 1, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 1, 0, 0, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("2.5D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Window_Width, Window_Height, 0);

    SDL_Renderer * render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;

    SDL_Event event;

    Vector2 player_pos {0, 0};
    float player_angle = 0;

    while(!quit){

        while(SDL_PollEvent(&event) != 0){
            if(event.type == SDL_QUIT){
                quit = true;
            }

            if(event.type == SDL_KEYDOWN){

                SDL_Keycode sym = event.key.keysym.sym;

                float speed = 5.0f;

                if (event.key.keysym.sym == SDLK_w) {
                    player_pos.x += SDL_cosf(player_angle) * speed;
                    player_pos.y += SDL_sinf(player_angle) * speed;
                }

                if (event.key.keysym.sym == SDLK_s) {
                    player_pos.x -= SDL_cosf(player_angle) * speed;
                    player_pos.y -= SDL_sinf(player_angle) * speed;
                }

                if (event.key.keysym.sym == SDLK_a) {
                    player_angle += -PI/10;
                }

                if (event.key.keysym.sym == SDLK_d) {
                    player_angle += PI/10;
                }
            }
        }

        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);

        draw_walls(render, player_pos, player_angle, map);

        SDL_RenderPresent(render);

    }

    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}