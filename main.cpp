#include "include/SDL.h"
#include "iostream"

#define Window_Width 900
#define Window_Height 650

#define Wall_size 50
#define FOV 70

#define Max_Render 500

#define map_x 10
#define map_y 10

#define Player_R 5

#define PI 3.141592653589793238462643383279502884

#define Wall_top Wall_size
#define Wall_bot 0

#define jump_max_height Wall_size/2


class Vector2{
    public:
        float x, y;
};

bool is_wall(float x, float y, int (&map)[map_y][map_x]) {

    int cell_x = (int)(x / Wall_size);
    int cell_y = (int)(y / Wall_size);

    if(cell_x < 0 || cell_x >= map_x || cell_y < 0 || cell_y >= map_y)
        return true;

    return map[cell_y][cell_x] == 1 || map[cell_y][cell_x] == 2;
}

int color;

double check_walls(SDL_Renderer* render, Vector2& player_pos, int (&map)[map_y][map_x], float angle){
    
    double dist = 0.0f;
    double step = 0.7f;

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
            color = 1;
            return dist;
        }else if(map[cell_pos_y][cell_pos_x] == 2){
            color = 2;
            return dist;
        }

        dist += step;
    }

    return -1;

}

Uint8 r = 0, g = 0, b = 0;

void draw_walls(SDL_Renderer* render, Vector2 player_pos, float player_angle, int (&map)[map_y][map_x], float camz){

    float FOV_angle = FOV * PI / 180.0f;
    float half_height = Window_Height / 2.0f;
    float proj_dist = half_height / SDL_tan(FOV_angle / 2.0f);

    for(int x = 0; x < Window_Width; x++){

        float ray_angle = player_angle - FOV_angle/2 + (x / (float)Window_Width) * FOV_angle;

        double distance = check_walls(nullptr, player_pos, map, ray_angle);

        if(distance <= 0)
            continue;

        float wall_perp = distance * SDL_cos(ray_angle - player_angle);

        int wall_top = half_height - ((Wall_top - camz) * proj_dist / wall_perp);
        int wall_bot = half_height - ((Wall_bot - camz) * proj_dist / wall_perp);
        
        wall_top = SDL_max(0, wall_top);
        wall_bot = SDL_min(Window_Height-1, wall_bot);

        float shade = SDL_max(0.2f, 1.0f - wall_perp/Max_Render);

        if(color == 1){
            r = 200 * shade;
            g = 50 * shade;
            b = 50 * shade;
        }
        else if(color == 2){
            r = 50 * shade;
            g = 200 * shade;
            b = 50 * shade;
        }

        // walls
        SDL_SetRenderDrawColor(render, r, g, b, 255);
        SDL_RenderDrawLine(render, x, wall_top, x, wall_bot);

        // top walls
        SDL_SetRenderDrawColor(render, 80, 80, 200, 255);
        SDL_RenderDrawLine(render, x, 0, x, wall_top-1);

        // bottom walls
        SDL_SetRenderDrawColor(render, 150, 100, 50, 255);
        SDL_RenderDrawLine(render, x, wall_bot+1, x, Window_Height-1);
    }
}

int main(int argc, char *argv[]){

    int map[map_y][map_x] = {
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 1, 0, 0, 1, 0, 0, 1, 2,
        2, 0, 0, 1, 0, 0, 0, 1, 0, 2,
        2, 1, 1, 0, 0, 0, 0, 1, 0, 2,
        2, 0, 0, 0, 1, 1, 0, 0, 0, 2,
        2, 0, 1, 0, 0, 1, 0, 0, 1, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 1, 0, 1, 0, 0, 1, 1, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2
    };

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("2.5D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Window_Width, Window_Height, 0);

    SDL_Renderer * render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture * texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Window_Width, Window_Height);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    bool quit = false;

    float camz = Wall_size / 2.0f;
    
    float jump_speed = 100.0f;
    float gravity = 104.8f;
    float camz_base = Wall_size / 2.0f;
    
    SDL_Event event;
    
    Vector2 player_pos { 1.5f * Wall_size, 1.5f * Wall_size };
    float player_angle = 0;
    Uint32 lastTime = SDL_GetTicks();

    int frame_count = 0;
    Uint32 fps_last_time = SDL_GetTicks();
    float fps = 0.0f;

    float jump_height = 0.0f;

    bool can_jump = true;

    while(!quit){

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        while(SDL_PollEvent(&event)){

            if(event.type == SDL_QUIT){
                quit = true;
            }

            if(event.type == SDL_KEYDOWN){

                if(event.key.keysym.sym == SDLK_ESCAPE)
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


        if (keystate[SDL_SCANCODE_SPACE] && can_jump) {
            jump_height += jump_speed * deltaTime;
        }else
            jump_height -= gravity * deltaTime;


        if (jump_height < 0.0f) {
            jump_height = 0.0f;
            can_jump = true;
        }

       if (jump_height > jump_max_height) {
            can_jump = false;
        }

        camz = camz_base + jump_height;

        SDL_SetRenderTarget(render, texture);
        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);
        
        camz = camz + jump_height;
        draw_walls(render, player_pos, player_angle, map, camz);

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