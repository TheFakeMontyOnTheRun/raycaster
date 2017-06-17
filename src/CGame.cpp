#include <functional>
#include <cmath>
#include <iostream>
#include "Common.h"
#include "CGame.h"

namespace odb {

    CGame::CGame() {
        gameState = EGameState::kGame;
        x = y = 1;
        timeEllapsed = 0;



        map = std::array<std::array<int, 12>, 12>{
                std::array<int, 12>{0,0,0,0,0,0,0,0,0,0,0,0},
                std::array<int, 12>{0,0,0,0,0,0,0,0,0,0,0,0},
                std::array<int, 12>{0,0,0,0,0,0,0,0,0,0,0,0},
                std::array<int, 12>{0,0,0,0,0,0,0,0,0,0,0,0},
                std::array<int, 12>{0,0,1,0,0,0,1,0,0,0,0,0},
                std::array<int, 12>{0,0,0,0,1,0,0,0,0,0,0,0},
                std::array<int, 12>{0,0,0,1,1,1,0,0,0,0,0,0},
                std::array<int, 12>{0,0,0,0,1,0,0,0,0,0,0,0},
                std::array<int, 12>{0,0,1,0,0,0,1,0,0,0,0,0},
                std::array<int, 12>{0,0,0,0,0,0,0,0,0,0,0,0},
                std::array<int, 12>{0,0,0,0,0,0,0,0,0,0,0,0},
                std::array<int, 12>{0,0,0,0,0,0,0,0,0,0,0,0}
        };

    }


    void CGame::tick(long ms) {

        timeEllapsed += ms;

        if (x >= 12) {
            x = 11;
        }

        if (y >= 12) {
            y = 11;
        }

        if (x < 0) {
            x = 0;
        }

        if (y < 0) {
            y = 0;
        }

//      if ( map[ ( y / 40 ) ][ ( x / 40 ) ] == 1 ) {
//	gameState = EGameState::kGameOver;
//      }
//
//      if ( map[ ( (35 + y) / 40 )  ][ ( x / 40 ) ] == 1 ) {
//	gameState = EGameState::kGameOver;
//      }
//
//      if ( map[ ( y / 40 ) ][ ( (x + 35) / 40 ) ] == 1 ) {
//	gameState = EGameState::kGameOver;
//      }
//
//      if ( map[ ( ( y + 35) / 40 ) ][ ( (x + 35) / 40 ) ] == 1 ) {
//	gameState = EGameState::kGameOver;
//      }
//
//      if ( map[ ( y / 40 ) ][ ( x / 40 ) ] == 2 ) {
//	gameState = EGameState::kVictory;
//      }
//
//      if ( map[ ( (35 + y) / 40 )  ][ ( x / 40 ) ] == 2 ) {
//	gameState = EGameState::kVictory;
//      }

//      if ( map[ ( y / 40 ) ][ ( (x + 35) / 40 ) ] == 2 ) {
//	gameState = EGameState::kVictory;
//      }
//
//      if ( map[ ( ( y + 35) / 40 ) ][ ( (x + 35) / 40 ) ] == 2 ) {
//	gameState = EGameState::kVictory;
//      }
    }

    CControlCallback CGame::getKeyPressedCallback() {
        return [&](ECommand command) {

            std::cout << "angle: " << angle << " x: " << x << ", y: " << y << std::endl;
            if (command == ECommand::kLeft) {
                angle-=5;
            }

            if (command == ECommand::kRight) {
                angle+=5;
            }

            angle = static_cast<int>(angle) % 360;

            while ( angle < 0 ) {
                angle += 360;
            }

            if (command == ECommand::kUp) {
                x += std::sin((angle * 3.14159f) / 180.0f);
                y += std::cos((angle * 3.14159f) / 180.0f);

            } else if (command == ECommand::kDown) {
                x -= std::sin((angle * 3.14159f) / 180.0f);
                y -= std::cos((angle * 3.14159f) / 180.0f);

            }
        };
    }

    CControlCallback CGame::getKeyReleasedCallback() {
        return [&](ECommand command) {


        };
    }
}
