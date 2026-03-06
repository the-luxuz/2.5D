#include "include/SDL.h"
#include "iostream"
#include "cmath"

#define Window_Width 900
#define Window_Height 650

#define Wall_size 50
#define FOV 70

#define Max_Render 500

#define map_x 10
#define map_y 10

#define Player_R 5

#define PI 3.141592653589793238462643383279502884

bool not_w = false;
float ray_x;
float ray_y;

class Vector2{
    public:
        float x, y;
};

bool is_wall(float x, float y, int (&map)[map_y][map_x]) {

    int cell_x = (int)(x / Wall_size);
    int cell_y = (int)(y / Wall_size);

    if(cell_x < 0 || cell_x >= map_x || cell_y < 0 || cell_y >= map_y)
        return true;

    return map[cell_y][cell_x] == 1;
}

double check_walls(SDL_Renderer* render, Vector2& player_pos, int (&map)[map_y][map_x], float angle){
    
    double dist = 0.0f;
    double step = 0.7f;

    while(dist < Max_Render){

        ray_x = player_pos.x + SDL_cosf(angle) * dist;
        ray_y = player_pos.y + SDL_sinf(angle) * dist;

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

void draw_walls(SDL_Renderer* render, SDL_Texture* texture, Vector2 player_pos, float player_angle, int (&map)[map_y][map_x]){

    float FOV_angle = FOV * PI / 180.0f;

    int column = 0;

    for(float i = player_angle - FOV_angle / 2; i <= player_angle + FOV_angle / 2; i += FOV_angle / Window_Width){

        double distance = check_walls(render, player_pos, map, i);

        if(distance <= 0) {
            column++;
            continue;
        }

        int wall_height = (int)(Window_Height * Wall_size / distance);

        int start_y = Window_Height/2 - wall_height/2;

        int shade = (int)(std::max(0.0, 255.0 - (distance)));

        SDL_Rect rect = { column, start_y, 1, wall_height };

        SDL_SetRenderDrawColor(render, shade, shade, shade, 255);

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

    SDL_Texture * texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Window_Width, Window_Height);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    bool quit = false;

    SDL_Event event;

    Vector2 player_pos {0, 0};
    float player_angle = 0;
    Uint32 lastTime = SDL_GetTicks();

    int frame_count = 0;
    Uint32 fps_last_time = SDL_GetTicks();
    float fps = 0.0f;

    while(!quit){

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        while(SDL_PollEvent(&event)){

            if(event.type == SDL_QUIT){
                quit = true;
            }

            if(event.type == SDL_KEYDOWN){

                if(event.key.keysym.sym == SDLK_q)
                    quit = true;

            }

            if(event.type == SDL_MOUSEMOTION){
                player_angle += event.motion.xrel * 0.003f;
            }
            
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);

        float speed = 150.0f * deltaTime;

        if (keystate[SDL_SCANCODE_W]){

            float new_x = player_pos.x + SDL_cosf(player_angle) * speed;
            float new_y = player_pos.y + SDL_sinf(player_angle) * speed;

            if(!is_wall(new_x + Player_R, player_pos.y, map) && !is_wall(new_x - Player_R, player_pos.y, map)){
                player_pos.x = new_x;
            }

            if(!is_wall(player_pos.x, new_y + Player_R, map) && !is_wall(player_pos.x, new_y - Player_R, map)){
                player_pos.y = new_y;
            }
        }

        if (keystate[SDL_SCANCODE_S]) {

            float new_x = player_pos.x - SDL_cosf(player_angle) * speed;
            float new_y = player_pos.y - SDL_sinf(player_angle) * speed;

            if(!is_wall(new_x + Player_R, player_pos.y, map) && !is_wall(new_x - Player_R, player_pos.y, map)){
                player_pos.x = new_x;
            }

            if(!is_wall(player_pos.x, new_y + Player_R, map) && !is_wall(player_pos.x, new_y - Player_R, map)){
                player_pos.y = new_y;
            }
        }

        if (keystate[SDL_SCANCODE_A]) {

            float new_x = player_pos.x + SDL_cosf(player_angle - PI/2) * speed;
            float new_y = player_pos.y + SDL_sinf(player_angle - PI/2) * speed;

            if(!is_wall(new_x + Player_R, player_pos.y, map) && !is_wall(new_x - Player_R, player_pos.y, map)){
                player_pos.x = new_x;
            }

            if(!is_wall(player_pos.x, new_y + Player_R, map) && !is_wall(player_pos.x, new_y - Player_R, map)){
                player_pos.y = new_y;
            }
        }

        if (keystate[SDL_SCANCODE_D]) {

            float new_x = player_pos.x + SDL_cosf(player_angle + PI/2) * speed;
            float new_y = player_pos.y + SDL_sinf(player_angle + PI/2) * speed;

            if(!is_wall(new_x + Player_R, player_pos.y, map) && !is_wall(new_x - Player_R, player_pos.y, map)){
                player_pos.x = new_x;
            }

            if(!is_wall(player_pos.x, new_y + Player_R, map) && !is_wall(player_pos.x, new_y - Player_R, map)){
                player_pos.y = new_y;
            }
        }

        SDL_SetRenderTarget(render, texture);
        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);
        

        draw_walls(render, texture, player_pos, player_angle, map);

        SDL_SetRenderTarget(render, NULL);
        SDL_RenderCopy(render, texture, NULL, NULL);
        SDL_RenderPresent(render);

        frame_count++;
        Uint32 fps_current_time = SDL_GetTicks();
        if (fps_current_time - fps_last_time >= 1000) {
            fps = frame_count * 1000.0f / (fps_current_time - fps_last_time);
            frame_count = 0;
            fps_last_time = fps_current_time;

            std::string title = "2.5D - FPS: " + std::to_string((int)fps);
            SDL_SetWindowTitle(window, title.c_str());
        }

    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}