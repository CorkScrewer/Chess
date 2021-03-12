#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define CELL_SIZE 100

#define BOARD_W 8
#define BOARD_H 8

const int screen_width = CELL_SIZE * BOARD_W;
const int screen_height = CELL_SIZE * BOARD_H;

enum {
    PIECE_EMPTY = 0,
    PIECE_KING,
    PIECE_QUEEN,
    PIECE_BISHOP,
    PIECE_KNIGHT,
    PIECE_ROOK,
    PIECE_PAWN
};

typedef struct {
    int type;
    bool is_white;
} Piece;

Piece pieces[BOARD_W][BOARD_H] = {
    { {PIECE_ROOK, 0}, {PIECE_KNIGHT, 0}, {PIECE_BISHOP, 0}, {PIECE_QUEEN, 0}, {PIECE_KING, 0}, {PIECE_BISHOP, 0}, {PIECE_KNIGHT, 0}, {PIECE_ROOK, 0} },
    { {PIECE_PAWN, 0}, {PIECE_PAWN, 0}, {PIECE_PAWN, 0}, {PIECE_PAWN, 0}, {PIECE_PAWN, 0}, {PIECE_PAWN, 0}, {PIECE_PAWN, 0}, {PIECE_PAWN, 0} },
    { {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0} },
    { {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0} },
    { {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0} },
    { {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0}, {PIECE_EMPTY, 0} },
    { {PIECE_PAWN, 1}, {PIECE_PAWN, 1}, {PIECE_PAWN, 1}, {PIECE_PAWN, 1}, {PIECE_PAWN, 1}, {PIECE_PAWN, 1}, {PIECE_PAWN, 1}, {PIECE_PAWN, 1} },
    { {PIECE_ROOK, 1}, {PIECE_KNIGHT, 1}, {PIECE_BISHOP, 1}, {PIECE_QUEEN, 1}, {PIECE_KING, 1}, {PIECE_BISHOP, 1}, {PIECE_KNIGHT, 1}, {PIECE_ROOK, 1} },
};

int selected_x = -1, selected_y = -1;

bool is_white_turn = true;
int is_game_over = 0;

int sign(int x) {
    if (x < 0) return -1;
    if (x > 0) return 1;
    return 0;
}

SDL_Rect spritesheet_get_piece_rect(Piece p) {
    SDL_Rect r;
    r.x = (p.type - 1) * CELL_SIZE;
    r.y = !p.is_white * CELL_SIZE;
    r.w = CELL_SIZE;
    r.h = CELL_SIZE;

    return r;
}

void move_piece(int from_x, int from_y, int to_x, int to_y) {
    pieces[to_y][to_x] = pieces[from_y][from_x];

    pieces[from_y][from_x].is_white = 0;
    pieces[from_y][from_x].type = 0;
}

bool is_move_valid(int from_x, int from_y, int to_x, int to_y) {
    Piece p;
    bool is_valid = false;
    int temp_from_x;
    int temp_from_y;
    
    if (from_x == to_x && from_y == to_y) return false;
    
    p = pieces[from_y][from_x];

    if (pieces[to_y][to_x].type != PIECE_EMPTY && pieces[to_y][to_x].is_white == p.is_white) return false;

    switch (p.type) {
    case PIECE_KING: /* TODO: Castle */
        is_valid = abs(to_x - from_x) <= 1 && abs(to_y - from_y) <= 1;
        break;
    case PIECE_PAWN: /* TODO: En passant */
        if (p.is_white) {
            if (to_x == from_x && to_y == from_y-1 && pieces[to_y][to_x].type == PIECE_EMPTY) is_valid = true;
            if (from_y == 6 && to_x == from_x && to_y == from_y-2 && pieces[to_y][to_x].type == PIECE_EMPTY && pieces[to_y+1][to_x].type == PIECE_EMPTY) is_valid = true;
            if (to_x == from_x-1 && to_y == from_y-1 && pieces[to_y][to_x].type != PIECE_EMPTY) is_valid = true;
            if (to_x == from_x+1 && to_y == from_y-1 && pieces[to_y][to_x].type != PIECE_EMPTY) is_valid = true;
        } else {
            if (to_x == from_x && to_y == from_y+1 && pieces[to_y][to_x].type == PIECE_EMPTY) is_valid = true;
            if (from_y == 1 && to_x == from_x && to_y == from_y+2 && pieces[to_y][to_x].type == PIECE_EMPTY && pieces[to_y-1][to_x].type == PIECE_EMPTY) is_valid = true;
            if (to_x == from_x-1 && to_y == from_y+1 && pieces[to_y][to_x].type != PIECE_EMPTY) is_valid = true;
            if (to_x == from_x+1 && to_y == from_y+1 && pieces[to_y][to_x].type != PIECE_EMPTY) is_valid = true;
        }
        break;
    case PIECE_ROOK:
        is_valid = (to_x == from_x || to_y == from_y);
        break;
    case PIECE_BISHOP:
        is_valid = abs(to_x - from_x) == abs(to_y - from_y);
        break;
    case PIECE_QUEEN:
        is_valid = (to_x == from_x || to_y == from_y) || abs(to_x - from_x) == abs(to_y - from_y);
        break;
    case PIECE_KNIGHT:
        is_valid = (abs(to_x - from_x) == 1 && abs(to_y - from_y) == 2) || (abs(to_x - from_x) == 2 && abs(to_y - from_y) == 1);
        break;
    }

    /* We're manipulating from_x and from_y instead of using another variable, so we're gonna reset it. */
    temp_from_x = from_x;
    temp_from_y = from_y;
    
    if (p.type == PIECE_ROOK || p.type == PIECE_QUEEN) {
        if (to_x == from_x) {
            int s = sign(to_y - from_y);

            from_y += s;
            
            while (from_y != to_y) {
                if (pieces[from_y][from_x].type != PIECE_EMPTY) is_valid = false;
                from_y += s;
            }
        } else if (to_y == from_y) {
            int s = sign(to_x - from_x);

            from_x += s;
            
            while (from_x != to_x) {
                if (pieces[from_y][from_x].type != PIECE_EMPTY) is_valid = false;
                from_x += s;
            }
        }
    }
    if (p.type == PIECE_BISHOP || p.type == PIECE_QUEEN) {
        int sx = sign(to_x - from_x);
        int sy = sign(to_y - from_y);

        from_x += sx;
        from_y += sy;

        while (from_x != to_x && from_y != to_y) {
            if (pieces[from_y][from_x].type != PIECE_EMPTY) is_valid = false;
            from_x += sx;
            from_y += sy;
        }
    }

    from_x = temp_from_x;
    from_y = temp_from_y;

    return is_valid;
}

int is_in_check(int wkx, int wky, int bkx, int bky) {
    int white_king_x = wkx, white_king_y = wky;
    int black_king_x = bkx, black_king_y = bky;
    int x, y;

    if (wkx == -1) {
        for (y = 0; y < BOARD_H; ++y) {
            for (x = 0; x < BOARD_W; ++x) {
                if (pieces[y][x].type == PIECE_KING) {
                    if (pieces[y][x].is_white) {
                        white_king_x = x;
                        white_king_y = y;
                    } else {
                        black_king_x = x;
                        black_king_y = y;
                    }
                }
            }
        }
    }
    
    for (y = 0; y < BOARD_H; ++y) {
        for (x = 0; x < BOARD_W; ++x) {
            if (is_move_valid(x, y, white_king_x, white_king_y)) {
                return 1;
            }
            if (is_move_valid(x, y, black_king_x, black_king_y)) {
                return 2;
            }
        }
    }

    fflush(stdout);
    return 0;
}

bool is_in_checkmate(int who) {
    int x, y;
    int i, j;

    int white_king_x, white_king_y;
    int black_king_x, black_king_y;

    for (y = 0; y < BOARD_H; ++y) {
        for (x = 0; x < BOARD_W; ++x) {
            if (pieces[y][x].type == PIECE_KING) {
                if (pieces[y][x].is_white) {
                    white_king_x = x;
                    white_king_y = y;
                } else {
                    black_king_x = x;
                    black_king_y = y;
                }
            }
        }
    }
    
    for (y = 0; y < BOARD_H; ++y) {
        for (x = 0; x < BOARD_W; ++x) {
            if (who == 1 && !pieces[y][x].is_white) continue;
            if (who == 2 && pieces[y][x].is_white) continue;
            for (j = 0; j < BOARD_H; ++j) {
                for (i = 0; i < BOARD_W; ++i) {
                    if (is_move_valid(x, y, i, j)) {
                        Piece p[BOARD_H][BOARD_W];
                        memcpy(p, pieces, sizeof(Piece) * BOARD_W * BOARD_H);
                        
                        move_piece(x, y, i, j);
                        
                        if (0==is_in_check(white_king_x, white_king_y, black_king_x, black_king_y)) {
                            memcpy(pieces, p, sizeof(Piece) * BOARD_W * BOARD_H);
                            return false;
                        }

                        memcpy(pieces, p, sizeof(Piece) * BOARD_W * BOARD_H);
                    }
                }
            }
        }
    }

    return true;
}

int main() {
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running = true;

    SDL_Surface *surf, *go1, *go2;
    SDL_Texture *texture, *gow, *gob;
    
	SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    
	window = SDL_CreateWindow("Chess",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              screen_width,
                              screen_height,
                              SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    
    surf = IMG_Load("pieces.png");
    texture = SDL_CreateTextureFromSurface(renderer, surf);

    go1 = IMG_Load("white_win.bmp");
    gow = SDL_CreateTextureFromSurface(renderer, go1);

    go2 = IMG_Load("black_win.bmp");
    gob = SDL_CreateTextureFromSurface(renderer, go2);

    SDL_FreeSurface(surf);
    SDL_FreeSurface(go1);
    SDL_FreeSurface(go2);

	while (running) {
		SDL_Event event;
        SDL_Rect r = {
            0,
            0,
            CELL_SIZE,
            CELL_SIZE
        };

        int x, y, c;
        
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
            
            if (event.type == SDL_MOUSEBUTTONDOWN && !is_game_over) {
                int px = event.button.x / CELL_SIZE;
                int py = event.button.y / CELL_SIZE;

                int was_in_check = is_in_check(-1, -1, -1, -1);
                
                if (selected_x == -1) {
                    if (pieces[py][px].type == PIECE_EMPTY || pieces[py][px].is_white != is_white_turn) break;
                
                    selected_x = px;
                    selected_y = py;
                } else {
                    if (is_move_valid(selected_x, selected_y, px, py)) {
                        Piece tmp[BOARD_W][BOARD_H];
                        memcpy(tmp, pieces, sizeof(Piece) * BOARD_W * BOARD_H);
                        move_piece(selected_x, selected_y, px, py);
                        if ((was_in_check == 1 && is_white_turn) ||
                            (was_in_check == 2 && !is_white_turn)) {
                            if ((was_in_check == 1 && is_in_check(-1,-1,-1,-1) == 1) ||
                                (was_in_check == 2 && is_in_check(-1,-1,-1,-1) == 2)) {
                                /* Illegal move. */
                                memcpy(pieces, tmp, sizeof(Piece) * BOARD_W * BOARD_H);
                            } else {
                                is_white_turn = !is_white_turn;
                            }
                        } else {
                            if ((!was_in_check && ((is_white_turn && 1 == is_in_check(-1,-1,-1,-1)) || (!is_white_turn && 2 == is_in_check(-1,-1,-1,-1))))) {
                                /* Illegal move. */
                                memcpy(pieces, tmp, sizeof(Piece) * BOARD_W * BOARD_H);
                            } else {
                                int c = is_in_check(-1,-1,-1,-1);
                                is_white_turn = !is_white_turn;
                                if ((c == 1 && is_white_turn) || (c == 2 && !is_white_turn)) {
                                    bool is = is_in_checkmate(c);
                                    if (is) {
                                        is_game_over = c==1 ? 2 : 1;
                                    }
                                }
                            }
                        }
                    }
                    selected_x = -1;
                    selected_y = -1;
                }
            }
		}

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

        c = is_in_check(-1,-1,-1,-1);
        
        for (y = 0; y < BOARD_H; ++y) {
            for (x = 0; x < BOARD_W; ++x) {
                r.x = x * CELL_SIZE;
                r.y = y * CELL_SIZE;

                if ((x+y) % 2 == 1) {
                    SDL_SetRenderDrawColor(renderer, 255, 216, 158, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 219, 139, 71, 1);
                }

                if (pieces[y][x].type == PIECE_KING && ((c == 1 && pieces[y][x].is_white) || (c == 2 && !pieces[y][x].is_white))) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                }
                
                if (selected_x == x && selected_y == y) {
                    SDL_SetRenderDrawColor(renderer, 216, 255, 90, 255);
                }

                SDL_RenderFillRect(renderer, &r);

                if (pieces[y][x].type != PIECE_EMPTY) {
                    SDL_Rect src = spritesheet_get_piece_rect(pieces[y][x]);
                    SDL_RenderCopy(renderer, texture, &src, &r);
                }
            }
        }

        if (is_game_over == 1) {
            SDL_RenderCopy(renderer, gow, NULL, NULL);
        } else if (is_game_over == 2) {
            SDL_RenderCopy(renderer, gob, NULL, NULL);
        }
        
		SDL_RenderPresent(renderer);
	}

    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(gow);
    SDL_DestroyTexture(gob);
    
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	
	SDL_Quit();

    return 0;
}