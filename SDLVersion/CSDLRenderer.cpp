#include <functional>
#include "Common.h"

#include "CGame.h"
#include "CRenderer.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <memory>
#include <iostream>
#include <cmath>
#include <vector>
#include "NativeBitmap.h"
#include "IFileLoaderDelegate.h"
#include "CPlainFileLoader.h"
#include "LoadPNG.h"
namespace odb {
    const int xRes = 800;
    const int yRes = 600;

    auto texture = loadPNG("tile0.png", std::make_shared<Knights::CPlainFileLoader>(""));

    float Q_rsqrt( float number )
    {
        long i;
        float x2, y;
        const float threehalfs = 1.5F;

        x2 = number * 0.5F;
        y  = number;
        i  = * ( long * ) &y;                       // evil floating point bit level hacking
        i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
        y  = * ( float * ) &i;
        y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

        return y;
    }

    int sines[ 360 ];
    int cossines[ 360 ];

    SDL_Surface *video;

    CRenderer::CRenderer( CControlCallback keyPressedCallback, CControlCallback keyReleasedCallback ):
    mOnKeyPressedCallback( keyPressedCallback ), mOnKeyReleasedCallback( keyReleasedCallback )
    {
        SDL_Init( SDL_INIT_EVERYTHING );
        video = SDL_SetVideoMode( xRes, yRes, 0, 0 );

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

    inline void Set(int x, int y, SDL_Surface* surface, Uint32 color) {
        auto rect = SDL_Rect{
                        static_cast<Sint16 >(x),
                        static_cast<Sint16 >(y),
                        static_cast<Uint16 >(1),
                        static_cast<Uint16 >(1)};

        SDL_FillRect(video, &rect, color );
    }

    inline Uint32 Get(int x, int y, std::shared_ptr<NativeBitmap> texture) {
        Uint32 *ptr = (Uint32 *) texture->getPixelData();
        ptr += texture->getWidth() * y;
        ptr += x;
        return *ptr;
    }

    void MapToSphere(SDL_Surface *dst, int x0, int y0, int x1, int y1, std::shared_ptr<NativeBitmap> texture, int step ) {
        float _cos;
        float _sin;
        int u;
        int v;

        float rad = 3.14159257f / 180.0f;

        for (int d = 90; d < 270; d++ ) {
            for (int c = 0; c < 180; c++) {

                _sin = sin( c * rad) * sin( d * rad) * 180;
                _cos = cos( c * rad ) * 180;

                u = (step + d) % texture->getWidth();
                v = (180 - c) % texture->getHeight();

                auto texel = Get(u, v, texture);
                Set(320 + _sin, 200 + _cos, dst, texel );
            }
        }
    }


    void CRenderer::render( const CGame& game, long ms ) {

    SDL_Rect rect;
    rect = { 0, 0, xRes, yRes };
    const int px = game.x;
    const int py = game.y;
    
    SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 0, 0, 0 ) );
    
    switch ( game.gameState ) {
    case CGame::EGameState::kTitleScreen:
      rect = { xRes / 2, yRes / 2, xRes, yRes };
      SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 0, 255, 0 ) );
      break;
      
    case CGame::EGameState::kGame: {


        constexpr auto columnsPerDegree = xRes / 90;
        Sint16 column = 0;
        std::cout << "BEGIN" << std::endl;
        for (Sint16 d = -45; d < 45; ++d) {

            float ray = castRay(game, d);
            int distance = 2 * (40 * Q_rsqrt(ray)) * yRes / 2 / 40;
            ray = sqrt( ray );
            float sin_a = sin( wrap360( game.angle + d) * ( 3.14159f / 180.0f) );
            float cos_a = cos( wrap360( game.angle + d) * ( 3.14159f / 180.0f) );
            float hueX = game.x + ( ray * ( sin_a ) );
            float hueZ = game.y + ( ray * ( cos_a ) );

            int cellX = hueX;
            int cellZ = hueZ;

            int dx = ( hueX - cellX ) * 32;
            int dz = ( hueZ - cellZ ) * 32;

            int columnHeight = distance;

            MapToSphere(video, 0, 0, 100, 400, texture, 1 );

            for ( int y = 0; y < columnHeight; ++y ) {

                int v = ( texture->getHeight() * y) / columnHeight;
                int ux = (texture->getWidth() * dx) / 40;
                int uz = (texture->getWidth() * dz) / 40;
                int pixel = texture->getPixelData()[ ( texture->getWidth() * v ) + ((ux + uz ) % texture->getWidth()) ];

                rect = SDL_Rect{static_cast<Sint16 >(column),
                                static_cast<Sint16 >(yRes / 2 - (distance / 2) + y ),
                                static_cast<Uint16 >(columnsPerDegree),
                                static_cast<Uint16 >(1)};

                SDL_FillRect(video, &rect, SDL_MapRGB(video->format, pixel & 0xFF000000, pixel & 0x00FF0000, pixel & 0x0000FF ) );
            }



            column += columnsPerDegree;
        }
        std::cout << "END" << std::endl;
    }
      break;

    case CGame::EGameState::kVictory:
      rect = SDL_Rect{ 0,0,0,0 };
      rect.x = game.x;
      rect.y = game.y;
      rect.w = xRes - game.x;
      rect.h = game.y;
      SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 255, 255, 0 ) );
      break;
      
    case CGame::EGameState::kGameOver:
      rect = SDL_Rect{ 0,0, xRes, yRes };
      SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 255, 0, 0 ) );
      break;
      
      
    }
    
    
    SDL_Flip(video);
  }


    float CRenderer::castRay( const CGame &game, short offset) {

        const auto limit = 40 *0xFFFF;

        float rx0 = game.x * 0xFFFF;
        float ry0 = game.y * 0xFFFF;

        float rx = rx0;
        float ry = ry0;

        int angle = wrap360( game.angle + offset);

        do {
            rx += sines[ angle ];
            ry += cossines[ angle ];
        } while ( ( rx > 0 && rx < limit && ry > 0 && ry < limit ) && ( game.map[ ry /0xFFFF][ rx/0xFFFF ] == 0 ));


        float dx = rx - rx0;
        float dy = ry - ry0;

        dx = dx / 0xFFFF;
        dy = dy / 0xFFFF;

        return (( dx * dx ) + ( dy * dy ));
    }

    int CRenderer::wrap360(int i) {
        int angle = static_cast<int>( i ) % 360;

        while ( angle < 0 ) {
            angle += 360;
        }

        return angle;
    }
}
