#include "GameCore/GameApp.hpp"
#include "Managers/PuzzleManager.hpp"
#include "Managers/FileManager.hpp"
#include "Managers/ImageManager.hpp"
#include "Managers/InputManager.hpp"
#include "Managers/WindowManager.hpp"
#include "Puzzle/Link.hpp"
#include "Utils/Debug.hpp"
#include "Utils/Helper.hpp"
#include "Utils/Random.hpp"
#include <fstream>
#include <tuple>
#include <iostream>


namespace PuzzleManager
{
    void mergeLinks(const U16 l1id_, const U16 l2id_, const sf::Vector2f offset_);
    Link* getLink(const U16 id_);
    // Calls getSmallerLink with the pieces' link id
    Piece* getSmallerLinkPiece(Piece *p1_, Piece *p2_);
    void loadCurveData(std::vector<sf::Vector2f> &pts_, const float &scaleFactor_);
    void createBorders(std::vector<std::vector<PieceBorder>> &borders_, const bool random_);
    void createPieceSprites(sf::Image *origImg_, sf::Image *destImg_, std::vector<std::vector<PieceBorder>> &borders_);
    void drawCurveBorderTexture(sf::Texture *texture_, const U16 destImageSize_);

    //Select the smaller link. If they have the same size, it returns the one with smaller id.
    Link* getSmallerLink(Link *l1, Link *l2);

    U16 _pieceSize; // it's extern. Don't declare it as static.
    std::vector<sf::Color> _colors{sf::Color::Red,sf::Color::Black};
    static sf::FloatRect _playArea;
    static sf::FloatRect _initialPlayArea;
    static std::map<U16,std::unique_ptr<Link>> _mapLinks;
    static std::list<Piece *> _lOrderedPieces;
    static std::string _name;
    static U16 _nPiecesX;
    static U16 _nPiecesY;

    void mergeLinks(const U16 l1id_, const U16 l2id_, const sf::Vector2f offset_)
    {
        auto link1 = _mapLinks.find(l1id_);
        auto link2 = _mapLinks.find(l2id_);
        if(link1 == _mapLinks.end() || link2 == _mapLinks.end())
        {
            std::string dbg("One of the links, what you want to merge cannot be found! IDs: ");
            dbg += intToStr(l1id_) + std::string(",") + intToStr(l2id_);
            debugPrint(std::move(dbg));
        }
        else
        {
            Link *smaller=getSmallerLink(link1->second.get(),link2->second.get());
            Link *bigger = smaller==link1->second.get() ? link2->second.get() : link1->second.get();
            //I merge the smaller one to the bigger, so I need to copy fewer piece objects
            for(auto &it: smaller->getPieces())
            {
                it->move(offset_);
                if(it->getNumFreeSides() > 0)
                {
                    for(auto &it2: bigger->getPieces())
                    {
                        if(it2->getNumFreeSides() > 0)
                        {
                            if(gridDist(it->getPosInPuzzle(), it2->getPosInPuzzle()) == 1)
                            {
                                it->decreaseFreeSides(it->getRelativeDirection(&(*it2), false));
                                it2->decreaseFreeSides(it2->getRelativeDirection(&(*it), false));
                            }
                        }
                    }
                }
            }

            bigger->merge(*smaller);
            auto smallerIt = smaller==link1->second.get() ? link1 : link2;
            _mapLinks.erase(smallerIt);
        }
    }


    void savePuzzle()
    {
        std::ofstream file("Save/save1.sav", std::ofstream::out | std::ofstream::binary);
        U16 nameLength = _name.length();
        write<U16>(file,nameLength);
        file.write(_name.c_str(),nameLength);
        sf::Image *puzzleImage = ImageManager::getImage("puzzleTexture");
        U32 x = puzzleImage->getSize().x;
        U32 y = puzzleImage->getSize().y;
        write<U32>(file,x);
        write<U32>(file,y);
        const U8 *pixels = puzzleImage->getPixelsPtr();
        file.write((char*)pixels,x*y*4);

        write<U16>(file,_nPiecesX);
        write<U16>(file,_nPiecesY);
        write<U16>(file,_pieceSize);
        std::unordered_set<Piece *> allPieces(_lOrderedPieces.begin(),_lOrderedPieces.end());
        allPieces.insert(InputManager::getSelectedPieces().begin(),InputManager::getSelectedPieces().end());
        for(const auto &piece: allPieces)
        {
            piece->writeOut(file);
        }

        write<const float>(file, WindowManager::getZoomFactor());

        file.close();
    }


    void loadPuzzle()
    {
        // This needs only testing. If I'm in the game and press key 'l' then it remove all existing puzzle data and
        // loads the previously saved one.
        destroy();

        std::ifstream file("Save/save1.sav", std::ifstream::in | std::ifstream::binary);
        U16 nameLength;
        read<U16>(file,&nameLength);
        char *name = new char[nameLength];
        file.read(name,nameLength);
        _name = std::string(name,nameLength);
        delete[] name;

        U32 image_x;
        U32 image_y;
        read<U32>(file,&image_x);
        read<U32>(file,&image_y);
        U8 *pixels = new U8[image_x*image_y*4];
        file.read((char *)pixels,image_x*image_y*4);

        sf::Image *cuttedImage = new sf::Image();
        cuttedImage->create(image_x,image_y,pixels);
        delete[] pixels;
        sf::Texture *cuttedTex = new sf::Texture();
        cuttedTex->loadFromImage(*cuttedImage);
        ImageManager::addTexture("puzzleTexture",cuttedTex);

        read<U16>(file,&_nPiecesX);
        read<U16>(file,&_nPiecesY);
        read<U16>(file,&_pieceSize);
        float destPieceSize = _pieceSize*1.5f;
        Edge::setTextureCoordOffset(_nPiecesY*destPieceSize);
        WindowManager::setMinZoomFactor(_pieceSize/200.f);

        U32 numPieces = _nPiecesX * _nPiecesY;
        for(int i=0; i < numPieces; ++i)
        {
            PieceData currentPiece = Piece::readIn(file);
            U16 x = currentPiece._puzzlePosX;
            U16 y = currentPiece._puzzlePosY;
            sf::Sprite sp(*cuttedTex,sf::IntRect(x*destPieceSize,y*destPieceSize,
                                                 destPieceSize,destPieceSize));
            sp.rotate(90.f * currentPiece._direction);
            sp.setPosition(currentPiece._position);
            Piece *p = new Piece(x,y,currentPiece._linkId,sp,std::move(currentPiece._border),currentPiece._direction,currentPiece._freeSides);

            Link *link = getLink(currentPiece._linkId);
            if(link == nullptr)
            {
                Link *newLink = new Link(currentPiece._linkId);
                newLink->addPiece(std::unique_ptr<Piece>(p));
                _mapLinks.insert(std::pair<U16,std::unique_ptr<Link>>(currentPiece._linkId,std::unique_ptr<Link>(newLink)));
            }
            else
            {
                link->addPiece(std::unique_ptr<Piece>(p));
            }
            _lOrderedPieces.push_back(p);
        }

        updatePlayArea(true);

        float zoom;
        read<float>(file,&zoom);
        WindowManager::resetView();
        WindowManager::setZoomFactor(zoom);

        file.close();
    }


    void createPuzzle(const std::string &filePath_, const std::string &name_, const int pieceNum_)
    {
        _pieceSize = pieceNum_; //std::sqrt((float)(loadedSize.x*loadedSize.y)/(float)pieceNum_);
        _pieceSize += _pieceSize % 2;
        sf::Image *loaded = ImageManager::loadImage(filePath_);
        if(_pieceSize < 50.f)
        {
            const float scaleFactor = 50.f/_pieceSize;
            sf::Image *scaled = ImageManager::scaleImage(*loaded,scaleFactor);
            delete loaded;
            loaded = scaled;
            _pieceSize = 50.f;
        }

        _name = name_;
        sf::Vector2u loadedSize = loaded->getSize();

        sf::Vector2u cutoff((loadedSize.x%_pieceSize)/2,(loadedSize.y%_pieceSize)/2);
        sf::Vector2f imageSize(loadedSize.x-2*cutoff.x,loadedSize.y-2*cutoff.y);

        // If there were some numerical error we distract the differences as well.
        imageSize.x -= (int)imageSize.x%_pieceSize;
        imageSize.y -= (int)imageSize.y%_pieceSize;

        sf::Image *origImg=new sf::Image();
        origImg->create(imageSize.x,imageSize.y);
        origImg->copy(*loaded,0,0,sf::IntRect(cutoff.x,cutoff.y,imageSize.x+cutoff.x,imageSize.y+cutoff.y));
        delete loaded;
        ImageManager::addImage(_name,origImg);

        //Create the borders of the peices.
        _nPiecesX = imageSize.x/_pieceSize;
        _nPiecesY = imageSize.y/_pieceSize;
        std::vector<std::vector<PieceBorder>> borders;

        const sf::Vector2f destImageSize(1.5f*imageSize.x,1.5f*imageSize.y);
        sf::Texture *texToSave = new sf::Texture();
        texToSave->create(std::max(destImageSize.x, 1200.f),destImageSize.y+_colors.size()*300);
        drawCurveBorderTexture(texToSave,destImageSize.y);
        ImageManager::addTexture("puzzleTexture",texToSave);
        createBorders(borders,true);
        WindowManager::setMinZoomFactor(_pieceSize/200.f);

        sf::Image *dest = new sf::Image;
        dest->create(destImageSize.x,destImageSize.y,sf::Color::Transparent);
        createPieceSprites(origImg,dest,borders);
        texToSave->update(*dest);

        //Construct pieces after I sliced the picture.
        U16 linkId = 0;
        const float destPieceSize = _pieceSize*1.5f;
        float offset = (destPieceSize-_pieceSize)/2;

        // i goes for X coordinate, j goes for Y coordinate.
        Random::initialize(sf::Vector2f(-imageSize.x/2, imageSize.x/2), sf::Vector2f(-imageSize.y/2, imageSize.y/2));
        for(U16 i=0; i < _nPiecesX; ++i)
        {
            for(U16 j=0; j < _nPiecesY; ++j)
            {
                sf::Sprite sp(*texToSave,sf::IntRect(i*destPieceSize,j*destPieceSize,destPieceSize,destPieceSize));
                sp.setPosition(i*_pieceSize-offset,j*_pieceSize-offset);
                Piece *p=new Piece(i,j,linkId,sp,std::move(borders[i][j]));
                p->setPosition(Random::randomPixel());
                Link *l=new Link(linkId);
                l->addPiece(std::unique_ptr<Piece>(p));
                _lOrderedPieces.push_back(p);
                _mapLinks.insert(std::pair<U16,std::unique_ptr<Link>>(linkId,std::unique_ptr<Link>(l)));
                ++linkId;
            }
        }

        updatePlayArea(true);
    }


    void destroy()
    {
        _mapLinks.clear();
        _lOrderedPieces.clear();
        _name=std::string("");
        _nPiecesX=0;
        _nPiecesY=0;
        _pieceSize=0;
        ImageManager::removeTexture("puzzleTexture");
    }


    std::list<Piece *> getPieceOrderForRender()
    {
        std::list<Piece *> bottomUpPieceOrder;
        for(auto p = _lOrderedPieces.rbegin(); p != _lOrderedPieces.rend(); ++p)
        {
            bottomUpPieceOrder.push_back(*p);
        }

        return bottomUpPieceOrder;
    }


    bool addPiecesToSelection(sf::Vector2f &mousePos_, std::unordered_set<Piece *> &selection_)
    {
        bool addedAny = false;
        for(auto p: _lOrderedPieces)
        {
            if(p->isPointInside(mousePos_))
            {
                addedAny = true;
                auto linkPieces = getLink(p->getLinkId())->getPieces();
                for(auto linkP: linkPieces)
                {
                    selection_.insert(linkP);
                    _lOrderedPieces.remove(linkP);
                }
                break;
            }
        }

        return addedAny;
    }

    void selectEdgePieces(std::unordered_set<Piece *> &selection_)
    {
        for(auto p: _lOrderedPieces)
        {
            sf::Vector2i pos = p->getPosInPuzzle();
            if(pos.x == 0 || pos.x == _nPiecesX-1 || pos.y == 0 || pos.y == _nPiecesY-1)
            {
                auto linkPieces = getLink(p->getLinkId())->getPieces();
                for(auto linkP: linkPieces)
                {
                    selection_.insert(linkP);
                    _lOrderedPieces.remove(linkP);
                }
            }
        }
    }


    void putDownPieces(std::unordered_set<Piece *> &selection_)
    {
        for(auto piece: selection_)
        {
            for(auto const it: _lOrderedPieces)
            {
                if(piece->checkMatch(it))
                {
                    Piece *smaller = getSmallerLinkPiece(piece,it);
                    Piece *bigger = smaller==piece ? it : piece;

                    sf::Vector2f offset = smaller->getOffsetToMatch(bigger);

                    mergeLinks(piece->getLinkId(),it->getLinkId(),offset);
                }
            }

            bool isPut=false;

            for(auto it=_lOrderedPieces.begin(); it != _lOrderedPieces.end(); ++it)
            {
                if(getLink(piece->getLinkId())->getSize() <= getLink((*it)->getLinkId())->getSize())
                {
                    _lOrderedPieces.insert(it,piece);
                    isPut=true;
                    break;
                }
            }

            if(!isPut)
            {
                //Put the piece at the end.
                _lOrderedPieces.push_back(piece);
            }
        }

        updatePlayArea();
        selection_.clear();
    }


    Link* getLink(const U16 id)
    {
        auto link=_mapLinks.find(id);
        if(link==_mapLinks.end())
        {
            std::string dbg("You've requested a not existent link with id:");
            dbg+=intToStr(id);
            debugPrint(std::move(dbg));
            return nullptr;
        }
        else
        {
            return link->second.get();
        }
    }


    Piece* getSmallerLinkPiece(Piece *p1_, Piece *p2_)
    {
        Link *l1=getLink(p1_->getLinkId());
        Link *l2=getLink(p2_->getLinkId());
        Link *smaller=getSmallerLink(l1,l2);

        return smaller==l1? p1_ : p2_;
    }


    void loadCurveData(std::vector<sf::Vector2f> &pts, const float &scaleFactor)
    {
        //Load curve's control points.
        std::ifstream curvePts("ControlPoints.txt");

        sf::Vector2f base;
        float var;

        curvePts >> var;
        base.x=var*scaleFactor;
        curvePts >> var;
        base.y=var*scaleFactor;
        pts.push_back(base);

        sf::Vector2f curvePt;
        curvePts >> var;
        while(var)
        {
            curvePt.x=base.x+var*scaleFactor;
            curvePts >> var;
            curvePt.y=base.y+var*scaleFactor;
            pts.push_back(curvePt);

            curvePts >> var;
        }
    }


    void generateBorderDirections(std::vector<std::vector<DirEnum>> &hdirs,
                                  std::vector<std::vector<DirEnum>> &vdirs,
                                  const bool random_)
    {
        std::srand(std::time(0));
        vdirs.resize(_nPiecesX-1);
        for(U16 i=0; i<_nPiecesX-1; ++i)
        {
            vdirs[i].resize(_nPiecesY);
            for(U16 j=0; j < _nPiecesY; ++j)
            {
                DirEnum d;
                if(random_)
                {
                    d = std::rand()%2 == 0 ? eRight : eLeft;
                }
                else
                {
                    d = j%2 == i%2 ? eLeft : eRight;
                }
                vdirs[i][j] = d;
            }
        }

        hdirs.resize(_nPiecesX);
        for(U16 i=0; i < _nPiecesX; ++i)
        {
            hdirs[i].resize(_nPiecesY-1);
            for(U16 j=0; j < _nPiecesY-1; ++j)
            {
                DirEnum d;
                if(random_)
                {
                    d = std::rand()%2 == 0 ? eUp : eDown;
                }
                else
                {
                    d = j%2 == i%2 ? eDown : eUp;
                }
                hdirs[i][j] = d;
            }
        }
    }


    void createBorders(std::vector<std::vector<PieceBorder>> &borders_, const bool random_)
    {
        borders_.resize(_nPiecesX);
        for(auto &column: borders_)
        {
            column.resize(_nPiecesY);
        }

        std::vector<std::vector<DirEnum>> horizontalDirs;
        std::vector<std::vector<DirEnum>> verticalDirs;

        generateBorderDirections(horizontalDirs,verticalDirs,random_);

        CurvyEdge up = getBaseCurvyEdge(true);
        CurvyEdge right(up);
        right.rotateRight();
        CurvyEdge down(right);
        down.rotateRight();
        down.move(sf::Vector2f(_pieceSize,0.f));
        CurvyEdge left(down);
        left.rotateRight();

        LineEdge horizontal = getBaseLineEdge(true);
        LineEdge vertical(horizontal);
        vertical.rotateRight();

        for(U16 i=0; i<_nPiecesX; ++i)
        {
            for(U16 j=0; j<_nPiecesY; ++j)
            {
                Edge *topBorder, *rightBorder, *bottomBorder, *leftBorder;
                if(j==0)
                {
                    topBorder = new LineEdge(horizontal);
                }
                else
                {
                    if(horizontalDirs[i][j-1] == eUp)
                    {
                        topBorder = new CurvyEdge(up);
                    }
                    else
                    {
                        topBorder = new CurvyEdge(down);
                        topBorder->flipInside();
                    }
                }

                if(i == _nPiecesX-1)
                {
                    rightBorder = new LineEdge(vertical);
                }
                else
                {
                    if(verticalDirs[i][j] == eRight)
                    {
                        rightBorder = new CurvyEdge(right);
                    }
                    else
                    {
                        rightBorder = new CurvyEdge(left);
                        rightBorder->flipInside();
                    }
                }
                rightBorder->move(sf::Vector2f(_pieceSize,0.f));

                if(j == _nPiecesY - 1)
                {
                    bottomBorder = new LineEdge(horizontal);
                    bottomBorder->flipInside();
                }
                else
                {
                    if(horizontalDirs[i][j] == eUp)
                    {
                        bottomBorder = new CurvyEdge(up);
                        bottomBorder->flipInside();
                    }
                    else
                    {
                        bottomBorder = new CurvyEdge(down);
                    }
                }
                bottomBorder->move(sf::Vector2f(0.f,_pieceSize));

                if(i == 0)
                {
                    leftBorder = new LineEdge(vertical);
                    leftBorder->flipInside();
                }
                else
                {
                    if(verticalDirs[i-1][j] == eRight)
                    {
                        leftBorder = new CurvyEdge(right);
                        leftBorder->flipInside();
                    }
                    else
                    {
                        leftBorder = new CurvyEdge(left);
                    }
                }

                std::vector<Edge*> edges{topBorder, rightBorder, bottomBorder, leftBorder};
                for(auto &e: edges)
                {
                    e->move(sf::Vector2f(i*_pieceSize,j*_pieceSize));
                }

                PieceBorder border(edges);
                borders_[i][j]=std::move(border);
            }
        }
    }


    void createPieceSprites(sf::Image *origImg_, sf::Image *destImg_, std::vector<std::vector<PieceBorder>> &borders_)
    {
        sf::Vector2u imageSize(origImg_->getSize());
        U16 pieceCol=0, pieceRow=0;
        U16 iCounter=0,jCounter=0;
        U16 destPosX,destPosY;
        float destPieceSize=_pieceSize*1.5f;
        for(U16 i=0; i<imageSize.x; ++i)
        {
            jCounter=0;
            pieceRow=0;

            for(U16 j=0; j<imageSize.y; ++j)
            {
                sf::Vector2f thisPixel((float)i,(float)j);
                if(borders_[pieceCol][pieceRow].isInside(thisPixel))
                {
                    destPosX=pieceCol;
                    destPosY=pieceRow;
                }
                else
                {
                    if(pieceRow > 0 && borders_[pieceCol][pieceRow-1].isInside(thisPixel))
                    {
                        destPosX = pieceCol;
                        destPosY = pieceRow-1;
                    }
                    else if(pieceCol < _nPiecesX-1 && borders_[pieceCol+1][pieceRow].isInside(thisPixel))
                    {
                        destPosX = pieceCol+1;
                        destPosY = pieceRow;
                    }
                    else if(pieceRow < _nPiecesY-1 && borders_[pieceCol][pieceRow+1].isInside(thisPixel))
                    {
                        destPosX = pieceCol;
                        destPosY = pieceRow+1;
                    }
                    else
                    {
                        destPosX = pieceCol-1;
                        destPosY = pieceRow;
                    }

                }

                U16 origCenterX=destPosX*_pieceSize+_pieceSize/2;
                U16 origCenterY=destPosY*_pieceSize+_pieceSize/2;
                U16 destCenterX=destPosX*destPieceSize+destPieceSize/2;
                U16 destCenterY=destPosY*destPieceSize+destPieceSize/2;
                I16 diffFromOrigCenterX=i-origCenterX;
                I16 diffFromOrigCenterY=j-origCenterY;

                destImg_->setPixel(destCenterX+diffFromOrigCenterX, destCenterY+diffFromOrigCenterY, origImg_->getPixel(i,j));

                if(++jCounter == _pieceSize)
                {
                    ++pieceRow;
                    jCounter = 0;
                }
            }

            if(++iCounter == _pieceSize)
            {
                ++pieceCol;
                iCounter = 0;
            }
        }
    }

    CurvyEdge getBaseCurvyEdge(const bool scale_)
    {
        float scaleFactor = 1.0;
        if(scale_)
        {
            scaleFactor=(float)_pieceSize/400.f;	//The curve control points are set for 400 pixel wide pieces.
        }
        std::vector<sf::Vector2f> pts;
        std::vector<I8> dirs{-1, -1, -1, 1, 1, 1, 1, -1, -1, -1};
        loadCurveData(pts,scaleFactor);

        CurvyEdge up(true, eDown, eUp, pts, dirs, sf::Vector2f(scaleFactor,scaleFactor));	//It's a horizontal plus top edge.

        return up;
    }


    LineEdge getBaseLineEdge(const bool scale_)
    {
        float scaleFactor = 1.0;
        if(scale_)
        {
            scaleFactor=(float)_pieceSize/400.f;	//The curve control points are set for 400 pixel wide pieces.
        }
        return LineEdge(true, eDown, sf::Vector2f(0.f, 0.f), sf::Vector2f(scaleFactor * 400.f, 0.f),
                        sf::Vector2f(scaleFactor, scaleFactor));
    }


    void drawCurveBorderTexture(sf::Texture *texture_, const U16 destImageSize_)
    {
        sf::RenderTexture target;
        target.create(1200,_colors.size()*300);
        const float thickness = 0.03f*400.f;

        CurvyEdge up = getBaseCurvyEdge(false); // Load curve for 400 px wide edge.
        up.move(sf::Vector2f(0.25f*400.f,0.25f*400.f));
        for(U8 i=0; i < _colors.size(); ++i)
        {
            up.rawRender(target,_colors[i],thickness);
            up.move(sf::Vector2f(0.f,300.f));
        }

        LineEdge lineEdge = getBaseLineEdge(false);
        lineEdge.move(sf::Vector2f(0.25f*400.f + 600.f, 0.25f*400.f));
        for(U8 i=0; i < _colors.size(); ++i)
        {
            lineEdge.rawRender(target,_colors[i],thickness);
            lineEdge.move(sf::Vector2f(0.f,300.f));
        }

        sf::Image img = target.getTexture().copyToImage();
        img.flipVertically();
        texture_->update(img,0,destImageSize_);
        Edge::setTextureCoordOffset(destImageSize_);
    }


    Link* getSmallerLink(Link *l1, Link *l2)
    {
        U16 size1=l1->getSize();
        U16 size2=l2->getSize();

        if(size1 < size2)
        {
            return l1;
        }
        else if(size1==size2)
        {
            if(l1->getId() < l2->getId())
            {
                return l1;
            }
            else
            {
                return l2;
            }
        }
        else
        {
            return l2;
        }
    }

    U16 getPieceNumber()
    {
        return _nPiecesX*_nPiecesY;
    }

    const sf::FloatRect &getPlayArea()
    {
        return _playArea;
    }

    void updatePlayArea(const bool reset_)
    {
        if(reset_)
        {
            _playArea = sf::FloatRect(0.f,0.f,0.f,0.f);
        }
        for(auto &p: _lOrderedPieces)
        {
            _playArea = combineRectangles(_playArea, p->getBoundRect());
        }
    }
}
