# Chess Game (C++ / SFML)

A 2-player desktop chess game built in C++ using SFML for graphics and audio.

## Features

- Complete 8x8 chess board with draggable pieces
- Legal move generation for all pieces
- Check and checkmate detection
- Stalemate detection
- Pawn promotion (rook, knight, bishop, queen)
- En passant support
- Score tracking for captures
- Save and load game state
- Sound effects and background music
- Start menu with player names

## Tech Stack

- C++
- SFML 2.6.x
- Visual Studio (Windows)

## Project Structure

- `Chess_Game.cpp` - main game logic (movement, rules, UI, sounds)
- `Chess-Project.sln` - Visual Studio solution
- `Chess-Project.vcxproj` - Visual Studio project config

## Requirements

- Windows
- Visual Studio (with C++ desktop workload)
- SFML installed locally

This project expects SFML in:

- `D:\SFML C++\SFML-2.6.2\include`
- `D:\SFML C++\SFML-2.6.2\lib`
- `D:\SFML C++\SFML-2.6.2\bin`

If your SFML path is different, update project properties in Visual Studio.

## Build and Run (Visual Studio)

1. Open `Chess-Project.sln`
2. Select configuration:
   - `Debug`
   - `x64`
3. Build -> Rebuild Solution
4. Run with `F5`

## Controls

- **Mouse drag/drop**: move pieces
- **S key**: save game
- **L key**: load game

## Chess Rules Implemented

- Normal piece movement rules
- Captures
- Check/checkmate
- Stalemate
- Pawn promotion
- En passant


## Authors

- Muhammad Abdullah Rana
- Danyal Aqeel
