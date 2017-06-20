#include <functional>
#include "Common.h"

#include "CGame.h"
#include "CRenderer.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <iostream>
#include <cmath>

namespace odb {

    int sines[ 360 ];
    int cossines[ 360 ];

    SDL_Surface *video;

    CRenderer::CRenderer( CControlCallback keyPressedCallback, CControlCallback keyReleasedCallback ):
    mOnKeyPressedCallback( keyPressedCallback ), mOnKeyReleasedCallback( keyReleasedCallback )
    {
        SDL_Init( SDL_INIT_EVERYTHING );
        video = SDL_SetVideoMode( 640, 480, 0, 0 );

        for ( int c = 0; c < 360; ++c ) {
            auto sin_a = (std::sin(((c) * 3.14159f) / 180.0f) * static_cast<float>(0xFFFF) / 128.0f);
            auto cos_a = (std::cos(((c) * 3.14159f) / 180.0f) * static_cast<float>(0xFFFF) / 128.0f);
            sines[ c ] = sin_a;
            cossines[ c ] = cos_a;
        }
    }

    void CRenderer::sleep( long ms ) {
        SDL_Delay(33);
    }

    void CRenderer::handleSystemEvents() {
        SDL_Event event;

        while ( SDL_PollEvent( &event ) ) {

            if( event.type == SDL_QUIT ) {
#ifndef __EMSCRIPTEN__
                exit(0);
#endif
            }

            if ( event.type == SDL_KEYUP ) {
                switch ( event.key.keysym.sym ) {
                    case SDLK_q:
#ifndef __EMSCRIPTEN__
                        exit(0);
#endif
                    case SDLK_LEFT:
                        mOnKeyReleasedCallback( ECommand::kLeft );
                        break;

                    case SDLK_RIGHT:
                        mOnKeyReleasedCallback( ECommand::kRight );
                        break;

                    case SDLK_UP:
                        mOnKeyReleasedCallback( ECommand::kUp );
                        break;

                    case SDLK_DOWN:
                        mOnKeyReleasedCallback( ECommand::kDown );
                        break;
		default:
		  break;
		}
            }

            if ( event.type == SDL_KEYDOWN ) {
                switch ( event.key.keysym.sym ) {
                    case SDLK_LEFT:
                        mOnKeyPressedCallback( ECommand::kLeft );
                        break;
                    case SDLK_RIGHT:
                        mOnKeyPressedCallback( ECommand::kRight );
                        break;

                    case SDLK_UP:
                        mOnKeyPressedCallback( ECommand::kUp );
                        break;

                    case SDLK_DOWN:
                        mOnKeyPressedCallback( ECommand::kDown );
                        break;
		default:
		  break;
                }
            }

        }
}

  void CRenderer::render( const CGame& game, long ms ) {

    SDL_Rect rect;
    rect = { 0, 0, 640, 480 };
    const int px = game.x;
    const int py = game.y;
    
    SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 0, 0, 0 ) );
    
    switch ( game.gameState ) {
    case CGame::EGameState::kTitleScreen:
      rect = { 320, 240, 640, 480 };
      SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 0, 255, 0 ) );
      break;
      
    case CGame::EGameState::kGame: {

        constexpr auto columnsPerDegree = 640 / 45;
        Sint16 column = 0;

        for (Sint16 d = -22; d < 22; ++d) {

            float ray = castRay(game, d);
            int distance = 2 * ray * 240 / 12;

            rect = SDL_Rect{static_cast<Sint16 >(column),
                            static_cast<Sint16 >(240 - (distance / 2)),
                            static_cast<Uint16 >(columnsPerDegree),
                            static_cast<Uint16 >(distance)};

            SDL_FillRect(video, &rect, SDL_MapRGB(video->format, std::min<int>( 255, 16 + distance), 0, 0));

            column += columnsPerDegree;
        }
    }
      break;

    case CGame::EGameState::kVictory:
      rect = SDL_Rect{ 0,0,0,0 };
      rect.x = game.x;
      rect.y = game.y;
      rect.w = 640 - game.x;
      rect.h = game.y;
      SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 255, 255, 0 ) );
      break;
      
    case CGame::EGameState::kGameOver:
      rect = SDL_Rect{ 0,0, 640, 480 };
      SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 255, 0, 0 ) );
      break;
      
      
    }
    
    
    SDL_Flip(video);
  }

    float CRenderer::castRay( const CGame &game, short offset) {

        const auto limit = 12 *0xFFFF;

        float rx0 = game.x * 0xFFFF;
        float ry0 = game.y * 0xFFFF;

        float rx = rx0;
        float ry = ry0;

        int angle = static_cast<int>( game.angle + offset) % 360;

        while ( angle < 0 ) {
            angle += 360;
        }

        do {
            rx += sines[ angle ];
            ry += cossines[ angle ];
        } while ( ( rx > 0 && rx < limit && ry > 0 && ry < limit ) && ( game.map[ ry /0xFFFF][ rx/0xFFFF ] == 0 ));


        float dx = rx - rx0;
        float dy = ry - ry0;

        dx = dx / 0xFFFF;
        dy = dy / 0xFFFF;

        float distance = (( dx * dx ) + ( dy * dy ));

        return 144.0f / std::max( 1.0f, distance );
    }
}
