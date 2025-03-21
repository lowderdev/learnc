#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CELL_SIZE 10
#define GRID_WIDTH 120
#define GRID_HEIGHT 80
#define WINDOW_MARGIN (CELL_SIZE)
#define GAME_AREA_WIDTH (CELL_SIZE * GRID_WIDTH)
#define GAME_AREA_HEIGHT (CELL_SIZE * GRID_HEIGHT)
#define WINDOW_WIDTH (GAME_AREA_WIDTH + 2 * WINDOW_MARGIN)
#define WINDOW_HEIGHT (GAME_AREA_HEIGHT + 2 * WINDOW_MARGIN)

typedef struct {
  int r, g, b, a;
} Color;

void initGrid(bool grid[GRID_HEIGHT][GRID_WIDTH]) {
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      grid[y][x] = rand() % 100 < 8;
    }
  }
  // print the state of the grid
  // for (int y = 0; y < GRID_WIDTH; y++) {
  //   for (int x = 0; x < GRID_WIDTH; x++) {
  //     printf("%d ", grid[y][x]);
  //   }
  //   printf("\n");
  // }
}

int countAliveNeighbors(bool grid[GRID_HEIGHT][GRID_WIDTH], int x, int y) {
  int count = 0;
  if (x > 0 && grid[y][x - 1])
    count++;
  if (x < GRID_WIDTH - 1 && grid[y][x + 1])
    count++;
  if (y > 0 && grid[y - 1][x])
    count++;
  if (y < GRID_HEIGHT - 1 && grid[y + 1][x])
    count++;
  if (x > 0 && y > 0 && grid[y - 1][x - 1])
    count++;
  if (x < GRID_WIDTH - 1 && y > 0 && grid[y - 1][x + 1])
    count++;
  if (x > 0 && y < GRID_HEIGHT - 1 && grid[y + 1][x - 1])
    count++;
  if (x < GRID_WIDTH - 1 && y < GRID_HEIGHT - 1 && grid[y + 1][x + 1])
    count++;
  return count;
}

void update(bool grid[GRID_HEIGHT][GRID_WIDTH]) {
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      int aliveNeighbors = countAliveNeighbors(grid, x, y);
      if (grid[y][x]) {
        if (aliveNeighbors < 2 || aliveNeighbors > 3) {
          grid[y][x] = false;
        }
      } else {
        if (aliveNeighbors == 3) {
          grid[y][x] = true;
        }
      }
    }
  }
}

void render(SDL_Renderer *renderer, bool grid[GRID_HEIGHT][GRID_WIDTH]) {
  Color black = {0, 0, 0, 255};
  Color white = {255, 255, 255, 255};
  Color darkGrey = {64, 64, 64, 255};
  // Color grey = {128, 128, 128, 255};
  // Color red = {255, 0, 0, 255};

  // Clear the screen
  SDL_SetRenderDrawColor(renderer, darkGrey.r, darkGrey.g, darkGrey.b,
                         darkGrey.a);
  SDL_RenderClear(renderer);

  // Draw the game area
  SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, white.a);
  SDL_Rect game_area_rect = {WINDOW_MARGIN - 1, WINDOW_MARGIN - 1,
                             GAME_AREA_WIDTH + 2, GAME_AREA_HEIGHT + 2};
  SDL_RenderDrawRect(renderer, &game_area_rect);

  // render grid
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      bool alive = grid[y][x];
      SDL_Rect rect = {x * CELL_SIZE + WINDOW_MARGIN,
                       y * CELL_SIZE + WINDOW_MARGIN, CELL_SIZE, CELL_SIZE};
      if (alive) {
        SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, white.a);
      } else {
        SDL_SetRenderDrawColor(renderer, black.r, black.g, black.b, black.a);
      }
      SDL_RenderFillRect(renderer, &rect);
    }
  }

  SDL_RenderPresent(renderer);
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                        WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  srand(time(NULL));

  bool grid[GRID_HEIGHT][GRID_WIDTH];
  initGrid(grid);

  bool running = true;
  bool paused = false;
  bool step = false;
  int speed = 100;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_UP:
          if (paused) {
            paused = false;
          }
          speed = (speed > 20) ? speed - 20 : 10;
          break;
        case SDLK_DOWN:
          if (speed < 1000) {
            speed += 20;
          } else {
            paused = true;
          }
          break;
        case SDLK_RIGHT:
          step = true;
          break;
        case SDLK_SPACE:
          paused = !paused;
          break;
        case SDLK_p:
          paused = !paused;
          break;
        case SDLK_r:
          initGrid(grid);
          break;
        }
      }
    }

    if (!paused || step) {
      update(grid);
      step = false;
    }
    render(renderer, grid);

    SDL_Delay(speed);
  }

  // Cleanup
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
