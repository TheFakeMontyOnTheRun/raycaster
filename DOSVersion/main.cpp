#include <functional>
#include <memory>
#include <iostream>
#include "Common.h"
#include "CGame.h"
#include "CRenderer.h"

std::shared_ptr<odb::CRenderer> renderer;
odb::CGame game;

void gameLoopTick() {
  renderer->render( game, 33 );
  renderer->handleSystemEvents();
  game.tick( 33 );
}


int main() {
    renderer = std::make_shared<odb::CRenderer>( game.getKeyPressedCallback(), game.getKeyReleasedCallback() );


    while ( true ) {
        gameLoopTick();
        renderer->sleep( 33 );
    }

    return 0;
}
