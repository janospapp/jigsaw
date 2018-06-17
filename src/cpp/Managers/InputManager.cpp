#include "GameCore/GameApp.hpp"
#include "GameCore/GameState.hpp"
#include "Managers/InputManager.hpp"
#include "Managers/PuzzleManager.hpp"
#include "Managers/WindowManager.hpp"
#include "Utils/Helper.hpp"

namespace InputManager
{
    static std::unordered_set<Piece *> _selectedPieces;
    static sf::Vector2f _lastMousePos;
    static sf::Vector2f _lastLeftClick;
    static bool _movingPiece;
    static bool _movingCamera;

    void scroll(const U32 deltaMilli);

    void initialize()
    {
        _lastMousePos = WindowManager::getAbsMousePos();
        _movingPiece = false;
        _movingCamera = false;
    }


    void destroy()
    {
        _selectedPieces.clear();
    }


    void gameConstantInput(const U32 deltaMilli)
    {
        sf::Vector2f mousePos(WindowManager::getAbsMousePos());
        if(_movingPiece)
        {
            sf::Vector2f offset=mousePos-_lastMousePos;
            for(auto &pPiece: _selectedPieces)
            {
                pPiece->move(offset);
            }
        }
        else if(_movingCamera)
        {
            sf::Vector2f offset = _lastMousePos - mousePos;
            WindowManager::moveCamera(offset);
            // Get the new position of the cursor after camera moved.
            mousePos = WindowManager::getAbsMousePos();
        }

        _lastMousePos = mousePos;

        scroll(deltaMilli);
    }


    void gameLeftClick()
    {
        sf::Vector2f mousePos(WindowManager::getAbsMousePos());
        _lastLeftClick = _lastMousePos = mousePos;
        if(_movingPiece)
        {   // The mouse is over a selected piece for sure. Put down the pieces.
            PuzzleManager::putDownPieces(_selectedPieces);
            ((GamePlayState*)GameApp::GetState())->_changedSinceLastSave = true;
            _movingPiece = false;
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

                if(!PuzzleManager::addPiecesToSelection(mousePos,_selectedPieces))
                {
                    // No pieces added to selection. The player clicked on the background.
                    _movingCamera = true;
                }
            }

            if(!shiftPressed && !_selectedPieces.empty())
            {
                _movingPiece = true;
            }
        }
    }


    void gameLeftReleased()
    {
        if(_movingPiece)
        {
            sf::Vector2f mousePos(WindowManager::getAbsMousePos());
            if(gridDist(mousePos,_lastLeftClick) > 20)
            {   // If the click and release happened far from each other then the user
                // dragged the pieces.
                PuzzleManager::putDownPieces(_selectedPieces);
                ((GamePlayState*)GameApp::GetState())->_changedSinceLastSave = true;
                _movingPiece = false;
            }
        }
        else if(_movingCamera)
        {
            _movingCamera = false;
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
        float movePixel = GameApp::Options.fScrollSpeed * deltaMilli / WindowManager::getZoomFactor();
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

    void gameRightClick()
    {
        sf::Vector2f mouse = WindowManager::getAbsMousePos();
        for(auto &p: _selectedPieces)
        {
            p->rotateRight(mouse);
        }
    }

    void selectEdgePieces()
    {
        _selectedPieces.clear();
        PuzzleManager::selectEdgePieces(_selectedPieces);
    }

}
