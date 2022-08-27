#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int INITIAL_SIZE_X = 640;
const int INITIAL_SIZE_Y = 640;
int WIDTH;
int HEIGHT;

int BOARD_SIZE;
int SQUARE_SIZE;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* queen;
SDL_Texture* light;
SDL_Texture* dark;
SDL_Rect rect;
SDL_Event event;
Uint64 start, end, fpscheck, elapsed, period;
float delta;
bool time_check;


void renderBoard(std::vector<std::vector<int>> board){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	for(int i = 0; i < BOARD_SIZE; i++){
		rect.y  = i * SQUARE_SIZE;
		for(int j = 0; j < BOARD_SIZE; j++){
			rect.x = j * SQUARE_SIZE;
			if((i+j)%2 == 0) SDL_RenderCopy(renderer, light, NULL, &rect);
			else SDL_RenderCopy(renderer, dark, NULL, &rect);
			if(board[i][j] == 1){
				rect.x += SQUARE_SIZE / 10;
				rect.y += SQUARE_SIZE / 10;
				rect.w -= SQUARE_SIZE / 5;
				rect.h -= SQUARE_SIZE / 5;
				SDL_RenderCopy(renderer, queen, NULL, &rect); 
				rect.y = i * SQUARE_SIZE;
				rect.w = SQUARE_SIZE;
				rect.h = SQUARE_SIZE;
			}
		}
	}
	SDL_RenderPresent(renderer);
}

bool safeCheck(int x, int y, std::vector<std::vector<int>>& board){
	if(!(x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[y][x] == 0)) return false;
	
	// check the column on the upper side
	for(int row = 0; row < y; row++){
		if(board[row][x] == 1) return false;
	}
	
	// check upper left diagonal
	for(int row = y, col = x; col >= 0 && row >= 0; row--, col--){
		if(board[row][col] == 1) return false;
	}
	
	// check upper right diagonal
	for(int row = y, col = x; col < BOARD_SIZE && row >= 0; col++, row--){
		if(board[row][col] == 1) return false;
	}
	
	return true;
}

bool solve(int row, std::vector<std::vector<int>>& board){
	start = SDL_GetPerformanceCounter();
	time_check = true;
	while(time_check){
		renderBoard(board);
		fpscheck = SDL_GetPerformanceCounter();
		if(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT){
				SDL_Quit();
				exit(1);
			}
		}
		end = SDL_GetPerformanceCounter();
		elapsed = ((end - start) / (float)SDL_GetPerformanceFrequency()) * 1000;
		if(elapsed >= period) time_check = false;
		else{
			end = SDL_GetPerformanceCounter();
			elapsed = (end-fpscheck) / (float)SDL_GetPerformanceFrequency();
			if(period < 35) delta = period - 0.1f - elapsed * 1000;
			else delta = 33.3f - elapsed * 1000; 
			if(delta > 0 && delta < period - (end - start) / (float)SDL_GetPerformanceFrequency()) SDL_Delay(delta);
		}
	}
	
	// break statement
	if(row == BOARD_SIZE) return true;
	
	for(int col = 0; col < BOARD_SIZE; col++){
		if(!safeCheck(col, row, board)) continue;
		board[row][col] = 1;
		
		if(solve(row + 1, board)) return true;
		board[row][col] = 0; // backtracking
	}
	
	return false;
}

int main(int argc, char** argv){
	std::cout << "board size: ";
	std::cin >> BOARD_SIZE;
	
	std::cout << "update period (in ms): ";
	std::cin >> period;
	
	SQUARE_SIZE = INITIAL_SIZE_X / BOARD_SIZE;
	WIDTH = SQUARE_SIZE * BOARD_SIZE;
	HEIGHT = SQUARE_SIZE * BOARD_SIZE;
	
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	window = SDL_CreateWindow("Chess Board", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	queen = IMG_LoadTexture(renderer, "assets/queen.png");
	dark = IMG_LoadTexture(renderer, "assets/dark.png");
	light= IMG_LoadTexture(renderer, "assets/light.png");
	
	rect.w = SQUARE_SIZE;
	rect.h = SQUARE_SIZE;
	
	std::vector<std::vector<int>> board;
	board.resize(BOARD_SIZE, std::vector<int>(BOARD_SIZE));

	solve(0, board);
	
	while(true){
		start = SDL_GetPerformanceCounter();
		renderBoard(board);
		if(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT){
				SDL_Quit();
				break;
			}
		}
		end = SDL_GetPerformanceCounter();
		elapsed = ((end-start))/SDL_GetPerformanceFrequency();
		delta = 11.1f - elapsed*100;
		if(delta > 0) SDL_Delay(delta); // 90fps
	}
	return 0;
}
