#include <array>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <conio.h>
#include <dpmi.h>
#include <go32.h>
#include <sys/movedata.h>
#include <pc.h>
#include <sys/farptr.h>
#include <bios.h>
#include <sys/nearptr.h>

#include <cstdlib>
#include <cstdio>
#include <functional>
#include <cmath>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstring>
#include <memory>
#include <iostream>
#include <map>
#include <array>
#include <iostream>
#include <string>
#include <vector>

#include "NativeBitmap.h"
#include "Common.h"
#include "CGame.h"
#include "CRenderer.h"

namespace odb {
  std::array<int, 320 * 200> mBuffer;
  std::array<int, 1000 * 1000 * 1000 > waste;

  int offset = 0;
  std::array< unsigned char, 320 * 200 > buffer;
  int origin = 0;
  int lastOrigin = -1;
  unsigned char shade;
  long frame = 0;
  
  std::vector<char> readToBuffer(FILE *fileDescriptor) {
    const unsigned N = 1024;


    waste[ 39289 ] = 34;
    waste[ 23283928 ] = 32;
    
    fseek(fileDescriptor, 0, SEEK_END);
    auto endPos = ftell( fileDescriptor );
    rewind(fileDescriptor);
    std::vector<char> total(endPos);
    auto writeHead = std::begin( total );
    
    for ( int c = 0; c < endPos; ++c ) {

      size_t read = fread((void *) &waste[0], 1, N, fileDescriptor);
      if (read) {
	for (int c = 0; c < read; ++c) {
	  *writeHead = (waste[c]);
	  writeHead = std::next(writeHead);
	}
      }
      if (read < N) {
	break;
      }
    }

    return total;
  }
  
  
  std::vector<char> loadBinaryFileFromPath(const std::string &path) {
    FILE *fd;

    fd = fopen(path.c_str(), "rb");

    if (fd == nullptr) {
      exit(0);
    }

    std::vector<char> toReturn = readToBuffer(fd);
    fclose(fd);

    return toReturn;
  }
  
  void fill( int x1, int y1, int w, int h, std::array<int,4> colour ) {
      int _x0 = x1 / 2;
      int _x1 = (x1 + w) / 2;
      int _y0 = (y1 * 200) / 480;
      int _y1 = ((y1 + h) * 200 ) / 480;

      int pixel = colour[ 3 ];
      pixel += colour[ 2 ] << 8;
      pixel += colour[ 1 ] << 8;
      
      for ( int y = _y0; y < _y1; ++y ) {
	for ( int x = _x0; x < _x1; ++x ) {

	  if ( x < 0 || x >= 320 || y < 0 || y >= 200 ) {
	    continue;
	  }
	  
	  mBuffer[ (320 * y ) + x ] = pixel;
	}
      }
    };
  
  void drawSquare( int x1, int y1, int x2, int y2, std::array<int,4> colour ) {
      int _x0 = x1 / 2;
      int _x1 = x2 / 2;
      int _y0 = (y1 * 200) / 480;
      int _y1 = (y2 * 200 ) / 480;

      int pixel = colour[ 3 ];
      pixel += colour[ 2 ] << 8;
      pixel += colour[ 1 ] << 8;
      
      for ( int y = _y0; y < _y1; ++y ) {
	for ( int x = _x0; x < _x1; ++x ) {

	  if ( x < 0 || x >= 320 || y < 0 || y >= 200 ) {
	    continue;
	  }
	  
	  mBuffer[ (320 * y ) + x ] = pixel;
	}
      }
    };
    
  unsigned char getPaletteEntry( int origin ) {
    unsigned char shade = 0;
    shade += (((((origin & 0x0000FF)      ) * 4  ) / 255 ) ) << 4;
    shade += (((((origin & 0x00FF00) >> 8 ) * 4  ) / 255 ) ) << 2;
    shade += (((((origin & 0xFF0000) >> 16) * 4  ) / 255 ) ) << 0;
    return shade;
  }
  
  CRenderer::CRenderer( CControlCallback keyPressedCallback, CControlCallback keyReleasedCallback ):
    mOnKeyPressedCallback( keyPressedCallback ), mOnKeyReleasedCallback( keyReleasedCallback ) {

      __dpmi_regs reg;
      
      reg.x.ax = 0x13;
      __dpmi_int(0x10, &reg);
      
      outp(0x03c8, 0);
      
      for ( int r = 0; r < 4; ++r ) {
	for ( int g = 0; g < 4; ++g ) {
	  for ( int b = 0; b < 4; ++b ) {
	    outp(0x03c9, (r * (85) ) );
	    outp(0x03c9, (g * (85) ) );
	    outp(0x03c9, (b * (85) ) );
	  }
	}
      }
    }

  void CRenderer::sleep( long ms ) {
  }
  
  void CRenderer::handleSystemEvents() {
    mOnKeyReleasedCallback( ECommand::kLeft );
    mOnKeyReleasedCallback( ECommand::kRight );
    mOnKeyReleasedCallback( ECommand::kUp );
    mOnKeyReleasedCallback( ECommand::kDown );
	
    while ( kbhit() ) {
      switch ( getch() ) {
      case 'a':
	mOnKeyPressedCallback( ECommand::kLeft );
	break;
      case 'd':
	mOnKeyPressedCallback( ECommand::kRight );
	break;
      case 'w':
	mOnKeyPressedCallback( ECommand::kUp );
	break;
      case 's':
	mOnKeyPressedCallback( ECommand::kDown );
	break;
      case 'z':
	break;
      default:
	break;
      }
    }
  }
  
  void CRenderer::render( const CGame& game, long ms ) {
    
    fill( 0, 0, 640, 480, { 0,0,0,0} );

    switch ( game.gameState ) {
    case CGame::EGameState::kTitleScreen:
      fill( 320, 240, 640, 480, { 0, 0, 255, 0 } );
      break;
      
    case CGame::EGameState::kGame:
      
      fill( 480, game.timeEllapsed / ( 60000 / 480 ), 40, 480 - 40, { 0, 128 , 0, 0 } );
      
      int recth = 40;

      for ( int y = 0; y < 12; ++y ) {
	for ( int x = 0; x < 12; ++x ) {
	  
	  int cx = x;
	  int cy = y;
	  
	  if ( cx > 0 && cx < 12 && cy > 0 && cy < 12 && game.map[ cy ][ cx ] > 0 ) {
	    int rectx = cx * 40;
	    int recty = cy * 40;

	    if ( game.map[ cy ][ cx ] == 1 ) {
	      fill( rectx, recty, 40, recth, { 0, 255, 255, 255 } );

	    } else {
	      fill( rectx, recty, 40, recth, { 0, 255, 255, 0 } );		      
	    }
	  }
	}
      }
      fill( game.x, game.y, 40, 40, { 0, 255, 0, 0 } );		      
      break;
      
    case CGame::EGameState::kVictory:
      fill( game.x, game.y, 640 - game.x, game.y, { 0, 255, 255, 0 } );
      break;
      
    case CGame::EGameState::kGameOver:
      fill( 0, 0, 640, 480, { 0, 255, 0, 0 } ) ;
      break;      
    }
    

    for( int offset = 0; offset < 320 * 200; ++offset ) {
	origin = mBuffer[offset];

	if ( origin != lastOrigin ) {
	  shade = getPaletteEntry( origin );
	}

	lastOrigin = origin;

	buffer[ offset ] = shade;
    }

    dosmemput(&buffer[0], 64000, 0xa0000);
  }
}
