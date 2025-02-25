#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define SNAKE_SIZE 20
#define BORDER_SIZE 20
#define WINDOW_WIDTH (SNAKE_SIZE * 32)
#define WINDOW_HEIGHT (SNAKE_SIZE * 24)
#define GAME_AREA_WIDTH (WINDOW_WIDTH - (BORDER_SIZE * 2))
#define GAME_AREA_HEIGHT (WINDOW_HEIGHT - (BORDER_SIZE * 2))
#define SNAKE_SPEED 100

typedef struct {
  int x, y;
} Point;

typedef struct {
  Point *body;
  int length;
  int capacity;
  int dx, dy;
} Snake;

typedef struct {
  Point position;
} Food;

void init_snake(Snake *snake) {
  snake->length = 20;
  snake->capacity = 20;
  snake->body = malloc(snake->capacity * sizeof(Point));
  snake->body[0].x = GAME_AREA_WIDTH / 2;
  snake->body[0].y = GAME_AREA_HEIGHT / 2;
  snake->dx = SNAKE_SIZE;
  snake->dy = 0;
}

void grow_snake(Snake *snake) {
  if (snake->length >= snake->capacity) {
    snake->capacity *= 2;
    snake->body = realloc(snake->body, snake->capacity * sizeof(Point));
  }
  snake->length++;
}

void move_snake(Snake *snake) {
  for (int i = snake->length - 1; i > 0; i--) {
    snake->body[i] = snake->body[i - 1];
  }
  snake->body[0].x += snake->dx;
  snake->body[0].y += snake->dy;
}

bool check_collision(Snake *snake) {
  for (int i = 1; i < snake->length; i++) {
    if (snake->body[0].x == snake->body[i].x &&
        snake->body[0].y == snake->body[i].y) {
      return true;
    }
  }
  return false;
}

void spawn_food(Food *food, Snake *snake) {
  bool valid_position;
  do {
    valid_position = true;
    // Generate a random x-coordinate for the food within the game area
    // The x-coordinate is a multiple of SNAKE_SIZE to align with the grid
    food->position.x =
        BORDER_SIZE + (rand() % (GAME_AREA_WIDTH / SNAKE_SIZE)) * SNAKE_SIZE;
    food->position.y =
        BORDER_SIZE + (rand() % (GAME_AREA_HEIGHT / SNAKE_SIZE)) * SNAKE_SIZE;
    for (int i = 0; i < snake->length; i++) {
      if (food->position.x == snake->body[i].x &&
          food->position.y == snake->body[i].y) {
        valid_position = false;
        break;
      }
    }
  } while (!valid_position);
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  srand(time(NULL));

  Snake snake;
  init_snake(&snake);

  Food food;
  spawn_food(&food, &snake);

  bool running = true;
  Uint32 last_move_time = SDL_GetTicks();

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_UP:
          if (snake.dy == 0) {
            snake.dx = 0;
            snake.dy = -SNAKE_SIZE;
          }
          break;
        case SDLK_DOWN:
          if (snake.dy == 0) {
            snake.dx = 0;
            snake.dy = SNAKE_SIZE;
          }
          break;
        case SDLK_LEFT:
          if (snake.dx == 0) {
            snake.dx = -SNAKE_SIZE;
            snake.dy = 0;
          }
          break;
        case SDLK_RIGHT:
          if (snake.dx == 0) {
            snake.dx = SNAKE_SIZE;
            snake.dy = 0;
          }
          break;
        }
      }
    }

    if (SDL_GetTicks() - last_move_time > SNAKE_SPEED) {
      move_snake(&snake);
      if (snake.body[0].x == food.position.x &&
          snake.body[0].y == food.position.y) {
        grow_snake(&snake);
        spawn_food(&food, &snake);
      }
      if (snake.body[0].x < BORDER_SIZE ||
          snake.body[0].x >= (GAME_AREA_WIDTH + BORDER_SIZE) ||
          snake.body[0].y < BORDER_SIZE ||
          snake.body[0].y >= (GAME_AREA_HEIGHT + BORDER_SIZE) ||
          check_collision(&snake)) {
        running = false;
      }
      last_move_time = SDL_GetTicks();
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_Rect game_area_rect = {SNAKE_SIZE, SNAKE_SIZE, GAME_AREA_WIDTH,
                               GAME_AREA_HEIGHT};
    SDL_RenderFillRect(renderer, &game_area_rect);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int i = 0; i < snake.length; i++) {
      SDL_Rect rect = {snake.body[i].x, snake.body[i].y, SNAKE_SIZE,
                       SNAKE_SIZE};
      SDL_RenderFillRect(renderer, &rect);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {food.position.x, food.position.y, SNAKE_SIZE, SNAKE_SIZE};
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
  }

  free(snake.body);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
