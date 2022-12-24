# Othello game

This is a simple GUI-based othello game (also known as reversi) written in C which uses SDL2, SDL_image and SDL_ttf libraries to operate.

if you want to know more about the rules of the game, how it's played and its strategies you can get further information in the following links:

- [Rules](https://www.worldothello.org/about/about-othello/othello-rules/official-rules/englishhttps://www.worldothello.org/about/about-othello/othello-rules/official-rules/english)

- [Play online](https://www.eothello.com)

## Dependencies

For successfuly compiling the game you need SDL2 and SDL_image installed which are respectively for rendering graphics, loading images with various formats and using different fonts and rendering text. Note that there's no need to install SDL_ttf separately since it comes with SDL2.

### ubuntu

```
sudo apt-get install libsdl-2.0
sudo apt-get install libsdl-image1.2-dev
```

install instructions for other platforms will be added soon...

## How the code works

Most of the functionalities are explained within the code as comments but a here is a brief explanation. 

For being able to reuse some commonly needed SDL functions easier several function wrappers have been created: 

- CreateWindow

- CreateRenderer

- IMGLoader

- CreateTex

- ShowScore

Their functionalities are well-described by their names and what they mostly do is creating textures, surfaces etc and also checking for errors in the proccess and if unsuccessfull the program will exit with defined exit codes.

After the board is initialized (according to game standards), SDL is also initialized and checked for errors. Then struct pointers window, renderer and also some surfaces and textures are loaded from 3 pictures in the game directory.

In the main loop after the board texture is loaded into the back buffer using SDL_RenderCopy (you can read about double buffering which is used in graphic rendering [here](https://www.geeksforgeeks.org/double-buffering/https://www.geeksforgeeks.org/double-buffering) the an SDL_event struct is created for handling mouse button events (either quitting the game or selecting a square).

When the user clicks on a square first the row and column are detected using the x,y coordinates and then if the square is available and legal the disk will be placed and some of the opponents disks are flipped and both scores are updated and the text data is copied into back buffer and if the square the user has chosen is not within the available ones simply nothing is done . Note that these steps happen in two 2D arrays and are not yet shown to the user. After the changes happened in the 2D arrays (one for keeping track of available points and other for keeping track of disks) the arrays are scanned and the disk textures are also copied to the back buffer and after all these happened using SDL_RenderPresent a new frame containing new move and the flipped disks along with new available points is shown. This goes on untill either quit is pressed or the game has ended (no more available squares) and if quit button is pressed the window is closed very shortly and renderer and window are destroyed.

## Game play

Bellow you can see a video showing the game being played for a short time:



https://user-images.githubusercontent.com/110523279/209427500-85d34d1f-93a9-4f8d-a05f-7f0df02259e4.mp4



## Known issues

- The functions for flipping disks after new placements and for finding new available places can probably be implemented more efficiently and will change in near future.

- The circles showing the available squares are not perfect and are drawn pixle by pixle, there might be a better way for drawing them.

## Planned future changes

- Add music to the game

- Add start and pause menues

- Add game instructions and "How to play" section

- Add a menu after the game has ended (starting new game etc)

## License

The code itself is under GPLv3 license but the pictures and the font used may have different licensing terms which should be taken into account by users who might want to use this project. I will try to find pictures which can be used freely (or create one myself) to solve this problem.
