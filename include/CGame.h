#ifndef CGAME_H
#define CGAME_H

namespace odb {
    class CGame {
    public:

      enum class EGameState{ kTitleScreen, kGame, kVictory, kGameOver };

      EGameState gameState = EGameState::kTitleScreen;
        float angle = 0;
      float x = 0;
      float y = 0;
      explicit CGame();
      long timeEllapsed = 0;

        std::array<std::array< int, 12>, 12 > map;

        void tick( long ms );
        CControlCallback getKeyPressedCallback();
        CControlCallback getKeyReleasedCallback();
    };
}
#endif
