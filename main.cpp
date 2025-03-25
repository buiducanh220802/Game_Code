<<<<<<< HEAD
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

using namespace std;

const int TILE_SIZE = 50;
const int WIDTH = 10;
const int HEIGHT = 10;
const int SCREEN_WIDTH = WIDTH * TILE_SIZE;
const int SCREEN_HEIGHT = HEIGHT * TILE_SIZE;

struct Bomberman
{
    int x, y;
    Bomberman(int startX, int startY) : x(startX), y(startY) {}
};

struct Bomb
{
    int x, y;
    int timer;
    Bomb(int bx, int by) : x(bx), y(by), timer(3) {}
};

Bomberman player(1, 1);
vector<Bomb> bombs;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

bool initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }
    window = SDL_CreateWindow("Bomberman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }
    return true;
}

void closeSDL()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void drawGame()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect playerRect = {player.x * TILE_SIZE, player.y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
    SDL_RenderFillRect(renderer, &playerRect);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (const auto &bomb : bombs)
    {
        SDL_Rect bombRect = {bomb.x * TILE_SIZE, bomb.y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        SDL_RenderFillRect(renderer, &bombRect);
    }

    SDL_RenderPresent(renderer);
}

void updateGame(SDL_Event &e)
{
    if (e.type == SDL_KEYDOWN)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_LEFT:
            if (player.x > 0)
                player.x--;
            break;
        case SDLK_RIGHT:
            if (player.x < WIDTH - 1)
                player.x++;
            break;
        case SDLK_UP:
            if (player.y > 0)
                player.y--;
            break;
        case SDLK_DOWN:
            if (player.y < HEIGHT - 1)
                player.y++;
            break;
        case SDLK_SPACE:
            bombs.push_back(Bomb(player.x, player.y));
            break;
        }
    }
}

int main()
{
    if (!initSDL())
        return -1;

    bool running = true;
    SDL_Event e;

    while (running)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
            else
            {
                updateGame(e);
            }
        }
        drawGame();
        SDL_Delay(100);
    }

    closeSDL();
    return 0;
}
=======
#include <iostream>
using namespace std;
int main()
{
    cout << "Wellcome to my project" << endl;
    return 0;
}
>>>>>>> 623922e85b40936548df394eb62ef84409d89dd6
