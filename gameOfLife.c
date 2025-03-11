#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CELL_SIZE 20
#define GRID_WIDTH 32
#define WINDOW_MARGIN (CELL_SIZE)
#define GAME_AREA_WIDTH (CELL_SIZE * GRID_WIDTH)
#define GAME_AREA_HEIGHT (CELL_SIZE * GRID_WIDTH)
#define WINDOW_WIDTH (GAME_AREA_WIDTH + 2 * WINDOW_MARGIN)
#define WINDOW_HEIGHT (GAME_AREA_HEIGHT + 2 * WINDOW_MARGIN)

typedef struct {
  int r, g, b, a;
} Color;

void initGrid(bool grid[GRID_WIDTH][GRID_WIDTH]) {
  for (int y = 0; y < GRID_WIDTH; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      grid[y][x] = rand() % 10 < 3; // 30% chance
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

// int countAliveNeighbors(Grid *grid, int x, int y) {

// }

// void update(Grid *grid) {
//   for (int y = 0; y < GRID_WIDTH; y++) {
//     for (int x = 0; x < GRID_WIDTH; x++) {
//       int i = y * GRID_WIDTH + x;
//       Cell cell = grid->cells[i];
//       int aliveNeighbors = countAliveNeighbors(grid, cell.x, cell.y);
//       if (cell.alive) {
//         if (aliveNeighbors < 2 || aliveNeighbors > 3) {
//           cell.alive = false;
//         }
//       } else {
//         if (aliveNeighbors == 3) {
//           cell.alive = true;
//         }
//       }
//       grid->cells[i] = cell;
//     }
//   }
// }

void render(SDL_Renderer *renderer, bool grid[GRID_WIDTH][GRID_WIDTH]) {
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
  SDL_Rect game_area_rect = {19, 19, GAME_AREA_WIDTH + 2, GAME_AREA_HEIGHT + 2};
  SDL_RenderDrawRect(renderer, &game_area_rect);

  // render grid
  for (int y = 0; y < GRID_WIDTH; y++) {
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

  bool grid[GRID_WIDTH][GRID_WIDTH];
  initGrid(grid);

  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }

    // update(&grid);
    render(renderer, grid);

    // Cap the frame rate to 60 FPS
    SDL_Delay(1000 / 60);
  }

  // Cleanup
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
