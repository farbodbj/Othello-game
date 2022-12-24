/************
By: Farbod Bijary
GitHub: https://github.com/farbodbj 
*************/

//Standard needed libraries:
#include<stdio.h>
#include<stdint-gcc.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>

//Graphics libraries:
#include<SDL2/SDL.h>
#include<SDL2/SDL_timer.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

//Error codes:
#define SDL_START_ERROR 0
#define SDL_CREATE_WINDOW_ERROR 1
#define SDL_RENDERER_ERROR 2
#define SDL_SURFACE_ERROR 3
#define SDL_TEXTURE_ERROR 4
#define DRAW_DISK_ERROR 5
#define SDL_FONT_LOAD_ERROR 6
#define SDL_TTF_INIT_ERROR 7

//Success codes:
#define DRAW_DISK_SUCCESS 100

//End game status
#define CONT 1
#define DRAW 2
#define BLACK_WINS 3
#define WHITE_WINS 4

//Global Variables:
char TITLE[] = "Othello game",BOARD[8][8];
int DISK_RADIUS = 22 , SCREEN_HEIGHT = 496, SCREEN_WIDTH = 416, BORDER = 30, LENGTH = 45, AVAIL[8][8] ,scores[2];

//Needed struct (typedef is used for ease in use):
typedef struct{
    int x; 
    int y;
}point;

//Rectangle struct for player scores
SDL_Rect WhitePlayer, BlackPlayer;


//Function prototypes:

//Creates an empty window 
SDL_Window* CreateWindow(const char *,unsigned int,unsigned int,int,int, Uint32);

//Creates a renderer which shows pictures, animations etc.
SDL_Renderer* CreateRenderer(SDL_Window * ,int);

//A wrapper function to load images from a certain path
SDL_Surface* IMGLoader(const char *,SDL_Renderer * , SDL_Window*);

//A wrapper function to create a texture from scratch (does not function as SDL_CreateTextureFromSurface)
SDL_Texture* CreateTex(SDL_Window *, SDL_Renderer *, SDL_Surface *);

//Just scans the board and puts the scores to (int scores[])
SDL_Texture *ShowScore(SDL_Renderer* ,char );

//Receives (x,y) position of a point (from mouse click) and returns the center of the board rectangle it belongs to (both input and output are of type point struct)
point FindPlace(point);

//Loads a picture of a disk with certain color given in the input
//Picture available in resources folder
int DrawCircle(SDL_Surface* , SDL_Renderer* , SDL_Surface* , SDL_Texture* , int , int , char );

//Checks for availability of all points and stores results in int AVAIL[8][8]
void IsAvailable(char );

//When a new disk is placed outflanks (flips) some disks according to rules
void UpdateBoard(int, int, char);

//Draws a hollow circle pixle by pixle using midpoint algorithm the function is copied from an stackoverflow (link provided in function definition) post :D
void DrawAvail(SDL_Renderer *, int, int, int);

//Initializes the Board with 2 white and 2 black disks
void InitBoard(void);

//Scans int BOARD[8][8] and counts disks of each color
void ScoreCounter();

//detects draw or win
int DetectEnd();

int main(int argc, char *argv[]){

    //MoveNum: Keeps track of move number to decide wether white or black should move (even nums: black, odd nums: white)
    //row, column: Just used for tracking user choices for moving
    //Detect: Used for detecting when the games can't be continued anymore 
    int MoveNum=0, row, col, Detect = 1;

    //boolean variable to know if the user presses exit button
    bool QUIT = false; 

    //Used for storing the coordinates of center of the rectangle user click have happened in
    point Cent;

    //Board initialization
    InitBoard();

    //Checks SDL initializations
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
        printf("error initializing window: %s", SDL_GetError());
        exit(SDL_START_ERROR);
    }
    //A window of type pointer to struct (SDL_Window *)
    SDL_Window* window = CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    //A renderer of type pointer to struct (SDL_Renderer *)
    SDL_Renderer* renderer = CreateRenderer(window, -1);

    //Board surface (loades image from certain path)
    SDL_Surface* Board = IMGLoader("resources/Board_TEXT.png", renderer, window);

    //Converts Board surface into texture
    SDL_Texture* tex = CreateTex(window, renderer, Board);

    //Creates the surfaces for white and black disk
    SDL_Surface *black_disk_surface = IMGLoader("resources/black_disk.png", renderer, window);
    SDL_Surface *white_disk_surface = IMGLoader("resources/white_disk.png", renderer, window);

    //Converts respective surfaces to texture to be rendered
    SDL_Texture *white_disk = SDL_CreateTextureFromSurface(renderer, white_disk_surface);
    SDL_Texture *black_disk = SDL_CreateTextureFromSurface(renderer, black_disk_surface);

    //Creates a texture for showing player scores using SDL_ttf
    SDL_Texture* WhitePlayerMessage;
    SDL_Texture* BlackPlayerMessage; 
    
    //(x,y) coordinates for placing the player scores
    WhitePlayer.x = 30;
    WhitePlayer.y = 40;

    BlackPlayer.x = 230; 
    BlackPlayer.y = 40;


    while(!QUIT && Detect==1){
        //Event struct for detecting mouse events
        SDL_Event event; 
        SDL_PollEvent(&event);
        //Detecting Quit event
        if(event.type == SDL_QUIT){
            QUIT=true;
            break;
        }
        //Board picture gets loaded to graphic memory
        SDL_RenderCopy(renderer, tex, NULL, NULL);
        //Stores availability data in int AVAIL[8][8] with respect to a certain color
        IsAvailable((MoveNum % 2)?('w'):('b'));
        Detect = DetectEnd();

        //Scans the Board and draws a hollow circle in available rectangle
        for(int r=0; r<8; r++){
            for(int c=0; c<8; c++){
                if(AVAIL[r][c]==1){
                    DrawAvail(renderer,BORDER + c * LENGTH + DISK_RADIUS, BORDER + LENGTH * r + DISK_RADIUS + 80, DISK_RADIUS);
                }
            }
        }        
        //If user clicks:
        if(event.type == SDL_MOUSEBUTTONDOWN){
            //(x,y) coordinates, center of rectangle, color, row and column of the rectangle in which the click is placed are detected
            point click = {event.button.x, event.button.y};
            Cent = FindPlace(click);
            row = (Cent.y  - BORDER - 80)/LENGTH + 1; 
            col = (Cent.x  - BORDER)/LENGTH + 1;
            char color = (MoveNum % 2)?('w'):('b'); 
            IsAvailable(color);
            
            //If the clicked rectangel is available the Board is updated and the MoveNum is incremented
            if(AVAIL[row-1][col-1]==1){
                UpdateBoard(row-1, col-1, color);
                MoveNum++;
            }
            
            /*
            JUST FOR DEBUGGING PURPOSE (PRINTS THE BOARD)
            for(int i=0; i<8; printf("%d,", AVAIL[row-1][i]),i++);
            printf("\n");
            for(int i=0; i<8; i++){
                for(int j=0; j<8; j++){
                    printf("%3c", BOARD[i][j]);
                }
                printf("\n");
            }
            */
            /*
            JUST FOR DEBUGGING PURPOSE (PRINTS AVAILABLE RECTANGLES)
            printf("\n");
            for(int i=0; i<8; i++){
                for(int j=0; j<8; j++){
                    printf("%3d", AVAIL[i][j]);
                }
                printf("\n");
            }
            */
            //FOR DEBUGGING PURPOSE
            //printf("mouse:\nx=%d, y=%d\ncenter:\nx=%d, y=%d\nrow=%d, col=%d, color=%c, isAvail=%d\nmove number=%d\n",event.button.x,event.button.y, Cent.x, Cent.y, row, col, color, AVAIL[row-1][col-1], MoveNum);
            //Scores are updated
            ScoreCounter();
            //Updated scores are shown
            WhitePlayerMessage = ShowScore(renderer, 'w');
            BlackPlayerMessage = ShowScore(renderer, 'b');

            //printf("score black = %d, score white = %d\n", scores[1], scores[0]);
            //resets scores (scores are counted from 0 every time)
            scores[0]=0;scores[1]=0;
        }
        //Puts disk pictures on defined places and colors on the board 
        for(int r=0; r<8; r++){
            for(int c=0; c<8; c++){
                if(BOARD[r][c]=='b')
                    DrawCircle(Board, renderer, black_disk_surface, black_disk,BORDER + c * LENGTH + DISK_RADIUS ,BORDER + LENGTH * r + DISK_RADIUS + 80,'b');
                if(BOARD[r][c]=='w')
                    DrawCircle(Board, renderer, white_disk_surface, white_disk,BORDER + c * LENGTH  + DISK_RADIUS ,BORDER + LENGTH * r + DISK_RADIUS + 80, 'w');
            }
        }
        //Disk textures are loaded into graphic memory
        SDL_RenderCopy(renderer, WhitePlayerMessage, NULL, &WhitePlayer);
        SDL_RenderCopy(renderer, BlackPlayerMessage, NULL, &BlackPlayer);
        //The back buffer is flipped (new frame is shown)
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/60);    
    }
    //If the game is ended with a draw or win the program waits for the players to watch the last status of the board 
    if(!QUIT)
        SDL_Delay(10000);
    //Window is freed and closed    
    SDL_DestroyWindow(window);
    //SDL_DestroyRenderer(renderer);
    SDL_Quit();

}

//A simple wrapper for SDL standard function to create window
SDL_Window* CreateWindow(const char *Title,unsigned int pos_x, unsigned int pos_y, int width, int height, Uint32 flags){
    SDL_Window* window = SDL_CreateWindow(Title, pos_x, pos_y,width, height, flags);
    //Error detector
    if(!window){
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();      
        exit(SDL_CREATE_WINDOW_ERROR);
    }
    return window;
}

//A wrapper to make creating a renderer easier
SDL_Renderer* CreateRenderer(SDL_Window * window,int index){
    Uint32 render_flags = SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, index, render_flags);
    //Error detector
    if(!renderer)
    {
      printf("error creating renderer: %s\n", SDL_GetError());
      SDL_DestroyWindow(window);
      SDL_Quit();
      exit(SDL_RENDERER_ERROR);
    } 
    return renderer;
}

SDL_Surface* IMGLoader(const char* path, SDL_Renderer * renderer, SDL_Window* window){
    SDL_Surface* surface = IMG_Load(path);
    //Error detector
    if (!surface)
    {
        printf("error creating surface: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(SDL_SURFACE_ERROR);
    }  
    return surface;
}

SDL_Texture* CreateTex(SDL_Window * window,SDL_Renderer * renderer, SDL_Surface * surface){
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    //Error detector
    if (!tex)
    {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(SDL_TEXTURE_ERROR);
    }
    return tex;
}

//A function to find the enclosing rectangle in which a click event is places
//uses constant and image-specific cordinates
point FindPlace(point click){
    int horizontal[]={110 ,155, 200, 245, 290, 335, 380, 425, 470};
    int vertical[]= {75, 120, 165, 210, 255, 300, 345, 390, 435};
    int i=0;
    point circle_rect;
    while(horizontal[i]< click.y){
        i++;
    }
    circle_rect.y = (horizontal[i]+horizontal[i-1])/2;
    //printf("horizontal borders: next one:%d previous one:%d\n", horizontal[i],horizontal[i+1]);
    i=0;
    while(vertical[i]< click.x){
        i++;
    }
    circle_rect.x = (vertical[i]+vertical[i-1])/2;
    //printf("vertical borders: next one:%d previous one:%d\n", vertical[i],vertical[i+1]);
    return circle_rect;
}
//Loads disk images loaded from files to graphic memory
int DrawCircle(SDL_Surface* dst, SDL_Renderer* renderer, SDL_Surface* disk_surface, SDL_Texture* disk_texture, int x_pos, int y_pos, char color){
    SDL_Rect circle_rect;
    int err_chk = 0; 
    circle_rect.h = circle_rect. w = 2 * DISK_RADIUS;
    circle_rect.x = (x_pos - DISK_RADIUS); circle_rect.y = (y_pos - DISK_RADIUS);

    if(color == 'b'){
        SDL_RenderCopy(renderer, disk_texture, NULL, &circle_rect);   
        err_chk = SDL_BlitSurface(disk_surface, NULL, dst, &circle_rect);
    }
            
    if(color == 'w'){
        SDL_RenderCopy(renderer, disk_texture, NULL, &circle_rect);  
        err_chk = SDL_BlitSurface(disk_surface, NULL, dst, &circle_rect);      
    }
    //Error detector
    if(err_chk){
        printf("Error while putting disk on board: %s", SDL_GetError());
        exit(DRAW_DISK_ERROR);
    }
}
//Uses SDL_ttf to show player scores with a Font in "resources" folder
SDL_Texture *ShowScore(SDL_Renderer* renderer,char color){
    TTF_Init();
    SDL_Color black = {0,0,0,0};
    TTF_Font* Font = TTF_OpenFont("resources/Digital.ttf", 26);
    if(!Font){
        printf("Error Opening font: %s", SDL_GetError());
        exit(SDL_FONT_LOAD_ERROR);
    }
    char MessP1[18] , MessP2[15];
    if(color == 'b'){    
        strcpy(MessP1,"Black ");
        sprintf(MessP2,"%d",scores[1]);
    }
    if(color == 'w'){
        strcpy(MessP1 ,"White ");
        sprintf(MessP2,"%d",scores[0]);
    }
    SDL_Surface* Message = TTF_RenderText_Solid(Font,strcat(strcat(MessP1, "player: "), MessP2), black);
    SDL_Texture* ScoreText = SDL_CreateTextureFromSurface(renderer,Message);
    if(color == 'w')
        SDL_QueryTexture(ScoreText, NULL, NULL, &(WhitePlayer.w), &(WhitePlayer.h));
    if(color == 'b')
        SDL_QueryTexture(ScoreText, NULL, NULL, &(BlackPlayer.w), &(BlackPlayer.h));

    return ScoreText;

}
//If no more moves are possible detects the winner or else announces a draw
int DetectEnd(){
    for(int r = 0; r<8; r++){
        for(int c = 0; c<8; c++){
            if(AVAIL[r][c]==1)
                return CONT; 
        }
    }
    if(scores[0]>scores[1])
        return WHITE_WINS;
    else if (scores[1] > scores[0])
        return BLACK_WINS;
    else    
        return DRAW;
}
//Initializes int BOARD[8][8] according to game rules
void InitBoard(){
    for(int i = 0; i < 8; i++ ){
        for(int j =0 ; j < 8; j++){
            BOARD[i][j] = 'e';
        }
    }
    BOARD[4][4] =  BOARD[3][3] = 'w';
    BOARD[4][3] = BOARD[3][4] = 'b';
}

//The int BOARD[8][8] array is iterated completely four times in this function 
//using six nested for(for()) or for(while) loops. iteration types provided in
//comments inside the functions. Probably this part can be furthure improved 
//in aspects of accuracy/efficiency 
void IsAvailable(char color){
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            AVAIL[i][j]=0;
        }
    }
    
    //Row by row iteration
    for(int i = 0; i < 8; i++){    
        for(int j = 0; j < 7; j++){
            if(BOARD[i][j] == color){
                int tmp_2 = j;
                if(BOARD[i][j+1]!='e' && BOARD[i][j+1]!=color){
                    while(BOARD[i][j+1]!='e' && BOARD[i][j+1]!=color && j<7){j++;}
                }
                if(BOARD[i][j+1]=='e' && tmp_2!=j && j<7)
                    AVAIL[i][j+1]=1;
            }
            if(BOARD[i][j] != color && BOARD[i][j] != 'e'){
                int tmp = j;
                while(BOARD[i][j+1]!='e' && BOARD[i][j+1]!=color && j<7){j++;}
                if(BOARD[i][j+1] == color && BOARD[i][tmp-1]=='e' && tmp>=1)
                    AVAIL[i][tmp-1]=1;
            }
        }
    }
    
    //Column by column iteration
    for(int i = 0; i < 8; i++){    
        for(int j = 0; j < 8 ; j++){
            if(BOARD[j][i] == color){
                int tmp = j;
                if(BOARD[j+1][i]!='e' && BOARD[j+1][i]!=color && j<7){
                    while(BOARD[j+1][i]!='e' && BOARD[j+1][i]!=color && j<7){j++;}
                }
                if(BOARD[j+1][i]=='e' &&  j>tmp && j<7)
                    AVAIL[j+1][i] = 1;
            }
            if(BOARD[j][i] != color && BOARD[j][i] != 'e'){
                int tmp_2 = j; 
                while(BOARD[j+1][i] != 'e' && BOARD[j+1][i]!=color && j<7){j++;}
                if(BOARD[j+1][i] == color && BOARD[tmp_2 - 1][i] == 'e' && tmp_2>=1)
                    AVAIL[tmp_2 - 1][i]=1;
            }
        }
    }

   //Down-left to up-right diagonal iteration
    int i, j; 
    for( int k = 2; k < 7; k++){
        i = k; 
        j = 0;
        while(i >= 0){
            if(BOARD[i][j] == color){
                int tmp = i;
                if(BOARD[i-1][j+1] != 'e' && BOARD[i-1][j+1] != color && j<7){
                    while(BOARD[i-1][j+1] != 'e' && BOARD[i-1][j+1] != color && j < 7){i--; j++;}
                }
                if(BOARD[i-1][j+1] == 'e' && i != tmp &&  j < 7)
                    AVAIL[i-1][j+1] = 1;
            }
            if(BOARD[i][j] != color && BOARD[i][j] != 'e'){
                int tmp_x = i, tmp_y = j;
                while(BOARD[i-1][j+1] != 'e' && BOARD[i-1][j+1] != color && j < 7){i--; j++;}
                if(BOARD[i-1][j+1] == color && BOARD[tmp_x + 1][tmp_y - 1] == 'e' && i >= 1)
                    AVAIL[tmp_x +1][tmp_y - 1] = 1;
            }
            i--; 
            j++;
        }
    }
    for( int k = 0; k < 6; k++){
        i = 7; 
        j = k; 
        while(j < 8){
            if(BOARD[i][j] == color){
                int tmp = i;
                if(BOARD[i-1][j+1] != 'e' && BOARD[i-1][j+1] != color && j<7){
                    while(BOARD[i-1][j+1]!='e' && BOARD[i-1][j+1] != color && j < 7){i--; j++;}
                }
                if(BOARD[i-1][j+1] == 'e' && i != tmp &&  j < 7)
                    AVAIL[i-1][j+1] = 1;
            }
            if(BOARD[i][j] != color && BOARD[i][j] != 'e'){
                int tmp_x = i, tmp_y = j;
                while(BOARD[i-1][j+1] != 'e' && BOARD[i-1][j+1] != color && j < 7){i--; j++;}
                if(BOARD[i-1][j+1] == color && BOARD[tmp_x + 1][tmp_y - 1] == 'e' && j >= 1)
                    AVAIL[tmp_x + 1][tmp_y - 1] = 1;
            }
        i--; 
        j++;
        }
    }

    //Up-left to down-right iteration
    for( int k = 0; k < 6; k++){
        i = k; 
        j = 0;
        while(i < 8){
            if(BOARD[i][j] == color){
                int tmp = i;
                if(BOARD[i+1][j+1] != 'e' && BOARD[i+1][j+1] != color && j<7){
                    while(BOARD[i+1][j+1]!='e' && BOARD[i+1][j+1] != color && j < 7){i++; j++;}
                }
                if(BOARD[i+1][j+1] == 'e' && i != tmp &&  j < 7)
                    AVAIL[i+1][j+1] = 1;
            }
            if(BOARD[i][j] != color && BOARD[i][j] != 'e'){
                int tmp_x = i, tmp_y = j;
                while(BOARD[i+1][j+1] != 'e' && BOARD[i+1][j+1] != color && j < 7 && i<7){i++;j++;}
                if(BOARD[i+1][j+1] == color && BOARD[tmp_x - 1][tmp_y - 1] == 'e' && tmp_x >= 1 && tmp_y>=1)
                    AVAIL[tmp_x - 1][tmp_y - 1] = 1;
            }
            i++; 
            j++;
        }
    }
    for( int k = 1; k < 6; k++){
        i = 0; 
        j = k; 
        while(j < 8){
            if(BOARD[i][j] == color){
                int tmp = i;
                if(BOARD[i+1][j+1] != 'e' && BOARD[i+1][j+1] != color && j<7){
                    while(BOARD[i+1][j+1]!='e' && BOARD[i+1][j+1] != color && j < 7){i++; j++;}
                }
                if(BOARD[i+1][j+1] == 'e' && i != tmp &&  j < 7)
                    AVAIL[i+1][j+1] = 1;
            }
            if(BOARD[i][j] != color && BOARD[i][j] != 'e'){
                int tmp_x = i, tmp_y = j;
                while(BOARD[i+1][j+1] != 'e' && BOARD[i+1][j+1] != color && j < 7){i++; j++;}
                if(BOARD[i+1][j+1] == color && BOARD[tmp_x - 1][tmp_y - 1] == 'e' && tmp_x >= 1)
                    AVAIL[tmp_x - 1][tmp_y - 1] = 1;
            }
        i++; 
        j++;
        }

    }

}
//The possibility of the move is checked before this function is called
//the respective row, column and diagonals of the move are scanned and needed
//replacements are done according to games rules
//six nested while loops are used, this part may also be further improved
//for better accuracy and efficiency
void UpdateBoard(int row, int col, char color){
    BOARD[row][col]=color;
    
    //Row iteration  (to right)
    int j = col; 
    while(BOARD[row][j+1] != color && BOARD[row][j+1] != 'e' && j<7)
        j++;
    if(BOARD[row][j+1]==color){
        j=col;
        while(BOARD[row][j+1] != color && BOARD[row][j+1] != 'e' && j<7){
            j++;
            BOARD[row][j]=color;
            }
    }
    //Row iteration (to left)
    j=col;
    while(BOARD[row][j-1] != color && BOARD[row][j-1] != 'e' && j>=1)
        j--;
    if(BOARD[row][j-1]==color){
        j=col;
        while(BOARD[row][j-1] != color && BOARD[row][j-1] != 'e' && j>=1){
            j--;
            BOARD[row][j]=color;
            }
    }
    
    //Column iteration (downwards)
    j = row;     
    while(BOARD[j+1][col] != color && BOARD[j+1][col] != 'e' && j<7)
        j++;
    if(BOARD[j+1][col]==color){
        j=row;  //??  j=col changed to j=row 
        while(BOARD[j+1][col] != color && BOARD[j+1][col] != 'e' && j<7){
            j++;
            BOARD[j][col]=color;
            }
    }
    //Column iteration (upwards)
    j = row;
    while(BOARD[j-1][col] != color && BOARD[j-1][col] != 'e' && j>=1)
        j--;
    if(BOARD[j-1][col]==color){
        j=row;
        while(BOARD[j-1][col] != color && BOARD[j-1][col] != 'e' && j>=1){
            j--;
            BOARD[j][col]=color;
            }
    }
    

   //Down-left to up-right diagonal iteration
   int i = row;
   j = col;
   while(BOARD[i-1][j+1] != color && BOARD[i-1][j+1] != 'e' && i >= 1){
    i--; j++;
   }
   if(BOARD[i-1][j+1]==color){
        i = row; j = col;
        while(BOARD[i-1][j+1] != color && BOARD[i-1][j+1] != 'e' && j < 7 && i > 0){
            i--; 
            j++; 
            BOARD[i][j]=color;
        }
   }
   
   
    //Up-right to down-left iteration
    i = row, j = col;
   while(BOARD[i+1][j-1] != color && BOARD[i+1][j-1] != 'e' && j >= 1){
    i++; j--;
   }
   if(BOARD[i+1][j-1]==color){
        i = row; j = col;
        while(BOARD[i+1][j-1] != color && BOARD[i+1][j-1] != 'e' && i<7){
            i++; 
            j--; 
            BOARD[i][j]=color;
        }
   }
   
    //Down-right to up-left iteration 
    i = row, j = col;
   while(BOARD[i-1][j-1] != color && BOARD[i-1][j-1] != 'e' && j >= 1 && i >= 1){
    i--; j--;
   }
   if(BOARD[i-1][j-1] == color){
        i = row; j = col;
        while(BOARD[i-1][j-1] != color && BOARD[i-1][j-1] != 'e' && j>=1 && i>=1){
            i--; 
            j--; 
            BOARD[i][j]=color;
        }
   }

   //Up-left to down-right iteration
   i = row, j = col;
   while(BOARD[i+1][j+1] != color && BOARD[i+1][j+1] != 'e' && j < 7 && i<7){
    i++; j++;
   }
   if(BOARD[i+1][j+1]==color){
        i = row; j = col;
        while(BOARD[i+1][j+1] != color && BOARD[i+1][j+1] != 'e' && j<7 && i<7){
            i++; 
            j++; 
            BOARD[i][j]=color;
        }
   }
}
//Draws a circle pixle by pixle using midpoint algorithm. Function and algorithm used:
//midpoint algorithm: https://en.wikipedia.org/w/index.php?title=Midpoint_circle_algorithm&oldid=889172082#C_example
//https://stackoverflow.com/questions/38334081/how-to-draw-circles-arcs-and-vector-graphics-in-sdl
void DrawAvail(SDL_Renderer * renderer, int centreX, int centreY, int radius){
   const int diameter = (radius * 2);

   int x = (radius - 1);
   int y = 0;
   int tx = 1;
   int ty = 1;
   int error = (tx - diameter);

   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}
//Just counts scores by scanning the board
void ScoreCounter(){
    for(int row = 0; row<8; row++){
        for(int col = 0; col<8; col++){
            if(BOARD[row][col]=='w')
                scores[0]++;
            else if(BOARD[row][col]=='b')
                scores[1]++;
        }
    }
}
