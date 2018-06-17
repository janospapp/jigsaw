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
    void loadPuzzle();
    void savePuzzle();
    void createPuzzle(const std::string &filePath_,const std::string &name_, const int pieceNum_);
    void destroy();

    // Get the pieces in bottom-up order for rendering. (We draw the top most for last.)
    std::list<Piece *> getPieceOrderForRender();

    // Get the piece link under the mouse position and add it to the selection. The InputManager
    // will handle the selection, moving etc. We just add the corresponding pieces here.
    // Returns whether any piece was added to the selection or not.
    bool addPiecesToSelection(sf::Vector2f &mousePos_, std::unordered_set<Piece *> &selection_);
    void selectEdgePieces(std::unordered_set<Piece *> &selection_);

    // This will put down the pieces, checking the matchings and clear the selection.
    void putDownPieces(std::unordered_set<Piece *> &selection_);

    U16 getPieceNumber();

    CurvyEdge getBaseCurvyEdge(const bool scale_);
    LineEdge getBaseLineEdge(const bool scale_);

    const sf::FloatRect& getPlayArea();
    void updatePlayArea(const bool reset_ = false);

    extern U16 _pieceSize;
    extern std::vector<sf::Color> _colors;
}

#endif
