#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

//#include "Puzzle/CacheZone.hpp"
//#include "SFML/Graphics.hpp"
//#include <deque>
#include "Puzzle/Piece.hpp"
#include <unordered_set>
#include <functional>
#include <list>

namespace InputManager
{
    void initialize();
    void destroy();

    void gameConstantInput(const U32 deltaMilli);

    // When left mouse button is pressed or released.
    void gameLeftClick();
    void gameLeftReleased();

    void gameMouseWheel(int delta);

    std::unordered_set<Piece *> getSelectedPieces();
}

#endif
