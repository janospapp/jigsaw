#ifndef PUZZLEMANAGER_HPP
#define PUZZLEMANAGER_HPP

#include "Utils/Config.hpp"
#include "Puzzle/Edge.hpp"
#include "Puzzle/Link.hpp"
#include <string>
#include <map>
#include <memory>
#include <list>


namespace PuzzleManager
{
    //Implement these later
    void loadPuzzle(const std::string &name);
    void savePuzzle();
    void createPuzzle();
    void destroy();

    // Get the pieces in bottom-up order for rendering. (We draw the top most for last.)
    std::list<Piece *> getPieceOrderForRender();

    // Get the piece link under the mouse position and add it to the selection. The InputManager
    // will handle the selection, moving etc. We just add the corresponding pieces here.
    void addPiecesToSelection(sf::Vector2f &mousePos_, std::unordered_set<Piece *> &selection_);

    // This will put down the pieces, checking the matchings and clear the selection.
    void putDownPieces(std::unordered_set<Piece *> &selection_);
}

#endif
