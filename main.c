#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_MARGIN 20
#define GAME_AREA_WIDTH (WINDOW_WIDTH - 2 * WINDOW_MARGIN)
#define GAME_AREA_HEIGHT (WINDOW_HEIGHT - 2 * WINDOW_MARGIN)
#define SNAKE_SIZE 20
#define SNAKE_SPEED 100

typedef struct {
  int r, g, b, a;
} Color;

typedef struct {
  int x, y;
} Point;

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;
typedef enum { N, S, E, W, NONE } NewDirection;

typedef struct {
  Point *body;
  int length;
  int capacity;
  Direction direction;
} Snake;

typedef struct {
  Point position;
} Food;

typedef struct {
  Snake snake;
  Food food;
  NewDirection newDirection;
  bool quit;
} GameState;

void placeFood(Food *food, Snake *snake) {
  bool valid = false;

  while (!valid) {
    valid = true;
    food->position.x = (rand() % (GAME_AREA_WIDTH / SNAKE_SIZE)) * SNAKE_SIZE;
    food->position.y = (rand() % (GAME_AREA_HEIGHT / SNAKE_SIZE)) * SNAKE_SIZE;

    for (int i = 0; i < snake->length; i++) {
      if (food->position.x == snake->body[i].x &&
          food->position.y == snake->body[i].y) {
        valid = false;
        break;
      }
    }
  }
}

Snake initSnake() {
  Snake snake;
  snake.length = 3;
  snake.capacity = 20;
  snake.body = malloc(snake.capacity * sizeof(Point));
  for (int i = 0; i < snake.length; i++) {
    snake.body[i].x = GAME_AREA_WIDTH / 2 - i * SNAKE_SIZE;
    snake.body[i].y = GAME_AREA_HEIGHT / 2;
  }
  snake.direction = RIGHT;
  return snake;
}

Food initFood(Snake *snake) {
  Food food = {0, 0};
  placeFood(&food, snake);
  return food;
}

GameState initGameState() {
  Snake snake = initSnake();
  Food food = initFood(&snake);
  NewDirection newDirection = E;
  bool quit = false;
  return (GameState){snake, food, newDirection, quit};
}

void handle_events(GameState *gameState) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      gameState->quit = true;
      break;
    } else if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
      case SDLK_UP:
        gameState->newDirection = N;
        break;
      case SDLK_DOWN:
        gameState->newDirection = S;
        break;
      case SDLK_LEFT:
        gameState->newDirection = W;
        break;
      case SDLK_RIGHT:
        gameState->newDirection = E;
        break;
      }
    }
  }
}

void update(GameState *gameState) {
  Snake *snake = &gameState->snake;
  Food *food = &gameState->food;
  Point new_head = {snake->body[0].x, snake->body[0].y};

  switch (gameState->newDirection) {
  case N:
    if (snake->direction != DOWN) {
      snake->direction = UP;
      break;
    }
  case S:
    if (snake->direction != UP) {
      snake->direction = DOWN;
      break;
    }
  case W:
    if (snake->direction != RIGHT) {
      snake->direction = LEFT;
      break;
    }
  case E:
    if (snake->direction != LEFT) {
      snake->direction = RIGHT;
      break;
    }
  case NONE:
    break;
  }
  gameState->newDirection = NONE;

  switch (snake->direction) {
  case UP:
    new_head.y -= SNAKE_SIZE;
    break;
  case DOWN:
    new_head.y += SNAKE_SIZE;
    break;
  case LEFT:
    new_head.x -= SNAKE_SIZE;
    break;
  case RIGHT:
    new_head.x += SNAKE_SIZE;
    break;
  }

  // Check if the snake has collided with the food
  if (new_head.x == food->position.x && new_head.y == food->position.y) {
    snake->length++;
    if (snake->length >= snake->capacity) {
      snake->capacity *= 2;
      snake->body = realloc(snake->body, snake->capacity * sizeof(Point));
    }
    for (int i = snake->length - 1; i > 0; i--) {
      snake->body[i] = snake->body[i - 1];
    }
    snake->body[0] = new_head;
    placeFood(food, snake);
  } else {
    for (int i = snake->length - 1; i > 0; i--) {
      snake->body[i] = snake->body[i - 1];
    }
    snake->body[0] = new_head;
  }

  // Check if the snake has collided with the walls or itself
  if (new_head.x < 0 || new_head.x >= GAME_AREA_WIDTH || new_head.y < 0 ||
      new_head.y >= GAME_AREA_HEIGHT) {
    gameState->quit = true;
  }
  for (int i = 1; i < snake->length; i++) {
    if (new_head.x == snake->body[i].x && new_head.y == snake->body[i].y) {
      gameState->quit = true;
    }
  }
}

void render(SDL_Renderer *renderer, GameState gameState) {
  Color black = {0, 0, 0, 255};
  Color grey = {128, 128, 128, 255};
  Color green = {0, 255, 0, 255};
  Color red = {255, 0, 0, 255};

  SDL_SetRenderDrawColor(renderer, black.r, black.g, black.b, black.a);
  SDL_RenderClear(renderer);

  // Draw the game area
  SDL_SetRenderDrawColor(renderer, grey.r, grey.g, grey.b, grey.a);
  int margin = WINDOW_MARGIN;
  SDL_Rect game_area_rect = {margin, margin, GAME_AREA_WIDTH, GAME_AREA_HEIGHT};
  SDL_RenderDrawRect(renderer, &game_area_rect);

  // Draw the snake
  SDL_SetRenderDrawColor(renderer, green.r, green.g, green.b, green.a);
  for (int i = 0; i < gameState.snake.length; i++) {
    SDL_Rect rect = {gameState.snake.body[i].x + WINDOW_MARGIN,
                     gameState.snake.body[i].y + WINDOW_MARGIN, SNAKE_SIZE,
                     SNAKE_SIZE};
    SDL_RenderFillRect(renderer, &rect);
  }

  // Draw the food
  SDL_SetRenderDrawColor(renderer, red.r, red.g, red.b, red.a);
  SDL_Rect rect = {gameState.food.position.x + WINDOW_MARGIN,
                   gameState.food.position.y + WINDOW_MARGIN, SNAKE_SIZE,
                   SNAKE_SIZE};
  SDL_RenderFillRect(renderer, &rect);

  SDL_RenderPresent(renderer);
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  srand(time(NULL));

  GameState gameState = initGameState();

  bool running = true;
  Uint32 lastUpdateTime = SDL_GetTicks();
  while (running) {
    handle_events(&gameState);
    running = !gameState.quit;

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastUpdateTime >= SNAKE_SPEED) {
      update(&gameState);
      lastUpdateTime = currentTime;
    }
    render(renderer, gameState);

    // Cap the frame rate to 60 FPS
    SDL_Delay(1000 / 60);
  }

  // Cleanup
  free(gameState.snake.body);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
