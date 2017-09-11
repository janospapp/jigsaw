#include "Managers/InputManager.hpp"
#include "Managers/PuzzleManager.hpp"
#include "Managers/WindowManager.hpp"
#include "Utils/Helper.hpp"

namespace InputManager
{
    static std::unordered_set<Piece *> _selectedPieces;
    static sf::Vector2f _lastMousePos;
    static sf::Vector2f _lastLeftClick;
    static bool _moving;

    void scroll(const U32 deltaMilli);

    void initialize()
    {
        _lastMousePos=WindowManager::getAbsMousePos();
        _moving=false;
    }


    void destroy()
    {
        _selectedPieces.clear();
    }


    void gameConstantInput(const U32 deltaMilli)
    {
        sf::Vector2f mousePos(WindowManager::getAbsMousePos());
        if(_moving)
        {
            sf::Vector2f offset=mousePos-_lastMousePos;
            for(auto &pPiece: _selectedPieces)
            {
                pPiece->move(offset);
            }
        }

        _lastMousePos = mousePos;

        scroll(deltaMilli);
    }


    void gameLeftClick()
    {
        sf::Vector2f mousePos(WindowManager::getAbsMousePos());
        _lastLeftClick = _lastMousePos = mousePos;
        if(_moving)
        {   // Put down the pieces
            PuzzleManager::putDownPieces(_selectedPieces);
            _moving = false;
        }
        else
        {

            /* Possibilities:
             *
             * Shift pressed:
             *   -> Clicked on selected: nothing (later: deselect that piece)
             *   -> Not clicked any selected: add the clicked one to selection
             * Shift not pressed:
             *   -> Clicked on selected: start moving
             *   -> Not clicked any selected: select only the clicked one and start moving
             *
             */

            bool clickedOnSelected = false;
            for(auto p: _selectedPieces)
            {
                if(p->isPointInside(mousePos))
                {
                    clickedOnSelected = true;
                    break;
                }
            }

            bool shiftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
                                 || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift));

            if(!clickedOnSelected)
            {
                if(!shiftPressed)
                {
                    // If I just cleared the selection I would lose those pieces. They
                    // need to get back to PuzzleManager, that's why I call putDownPieces.
                    PuzzleManager::putDownPieces(_selectedPieces);
                }

                PuzzleManager::addPiecesToSelection(mousePos,_selectedPieces);
            }

            if(!shiftPressed && !_selectedPieces.empty())
            {
                _moving = true;
            }
        }
    }


    void gameLeftReleased()
    {
        if(_moving)
        {
            sf::Vector2f mousePos(WindowManager::getAbsMousePos());
            if(gridDist(mousePos,_lastLeftClick) > 20)
            {   // If the click and release happened far from each other then the user
                // dragged the pieces.
                PuzzleManager::putDownPieces(_selectedPieces);
                _moving = false;
            }
        }
    }


    void gameMouseWheel(int delta)
    {
        if(delta < 0)
        {
            WindowManager::zoomOut(-delta);
        }
        else
        {
            WindowManager::zoomIn(delta);
        }
    }


    void scroll(const U32 deltaMilli)
    {
        float movePixel=200.f*deltaMilli/1000;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            WindowManager::scroll(0,movePixel);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            WindowManager::scroll(1,movePixel);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            WindowManager::scroll(2,movePixel);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            WindowManager::scroll(3,movePixel);
        }
    }


    std::unordered_set<Piece *> getSelectedPieces()
    {
        return _selectedPieces;
    }
}
