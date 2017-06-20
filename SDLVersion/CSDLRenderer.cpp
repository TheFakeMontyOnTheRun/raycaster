#include <functional>
#include "Common.h"

#include "CGame.h"
#include "CRenderer.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <iostream>
#include <cmath>

namespace odb {

    float sines[ 360 ];
    float cossines[ 360 ];

    SDL_Surface *video;

    CRenderer::CRenderer( CControlCallback keyPressedCallback, CControlCallback keyReleasedCallback ):
    mOnKeyPressedCallback( keyPressedCallback ), mOnKeyReleasedCallback( keyReleasedCallback )
    {
        SDL_Init( SDL_INIT_EVERYTHING );
        video = SDL_SetVideoMode( 640, 480, 0, 0 );

        for ( int c = 0; c < 360; ++c ) {
            float sin_a = std::sin(((c) * 3.14159f) / 180.0f) / 10.0f;
            float cos_a = std::cos(((c) * 3.14159f) / 180.0f) / 10.0f;
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

        constexpr auto columnsPerDegree = 640.0f / 45.0f;
        Sint16 column = 0;

        for (Sint16 d = -22; d < 22; ++d) {

            float ray = castRay(game, d);
            float distance = 2.0f * ray * 240.0f / 12.0f;

            rect = SDL_Rect{static_cast<Sint16 >(column),
                            static_cast<Sint16 >(240.0f - (distance / 2.0f)),
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

        float rx = game.x;
        float ry = game.y;

        int angle = static_cast<int>( game.angle + offset) % 360;

        while ( angle < 0 ) {
            angle += 360;
        }

        do {
            rx += sines[ angle ];
            ry += cossines[ angle ];
        } while ( ( rx > 0 && rx < 12 && ry > 0 && ry < 12 ) && ( game.map[ ry ][ rx ] == 0 ));


        float dx = rx - game.x;
        float dy = ry - game.y;
        float distance = sqrt(( dx * dx ) + ( dy * dy ));

        return 12.0f / std::max( 0.1f, distance );
    }
}
