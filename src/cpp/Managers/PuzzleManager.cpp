#include "GameCore/GameApp.hpp"
#include "Managers/PuzzleManager.hpp"
#include "Managers/ImageManager.hpp"
#include "Puzzle/Link.hpp"
#include "Utils/Debug.hpp"
#include "Utils/Helper.hpp"
#include "Utils/Random.hpp"
#include <fstream>
#include <tuple>


namespace PuzzleManager
{
    void mergeLinks(const U16 l1id_, const U16 l2id_, const sf::Vector2f offset_);
    Link* getLink(const U16 id_);
    // Calls getSmallerLink with the pieces' link id
    Piece* getSmallerLinkPiece(Piece *p1_, Piece *p2_);
    void loadCurveData(std::vector<sf::Vector2f> &pts_, std::vector<I16> &dirs_, const float &scaleFactor_);
    void createBorders(std::vector<std::vector<PieceBorder>> &borders_, std::vector<sf::Vector2f> &pts_,
                       std::vector<I16> &dirs_, const sf::Vector2f &pieceSize_, const sf::Vector2f &imageSize_,
                       std::vector<std::tuple<sf::Color,float,sf::Texture *>> &borderGraphics_);
    void createPieceSprites(sf::Image *origImg_, sf::Image *destImg_, std::vector<std::vector<PieceBorder>> &borders_,
                            sf::Vector2f &pieceSize_);

    //Select the smaller link. If they have the same size, it returns the one with smaller id.
    Link* getSmallerLink(Link *l1, Link *l2);

    static std::map<U16,std::unique_ptr<Link>> _mapLinks;
    static std::list<Piece *> _lOrderedPieces;
    static std::string _sName;
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
                if(it->getFreeSides() > 0)
                {
                    for(auto &it2: bigger->getPieces())
                    {
                        if(it2->getFreeSides() > 0)
                        {
                            if(gridDist(it->getPosInPuzzle(), it2->getPosInPuzzle()) == 1)
                            {
                                it->decreaseFreeSides();
                                it2->decreaseFreeSides();
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


    void createPuzzle()
    {
        sf::Image loaded;
        loaded.loadFromFile(TempOptions.pictureToSolve);
        sf::Vector2f pieceSize(TempOptions.pieceSize,TempOptions.pieceSize);
        sf::Vector2u loadedSize=loaded.getSize();
        sf::Vector2u cutoff((loadedSize.x%(int)TempOptions.pieceSize)/2,(loadedSize.y%(int)TempOptions.pieceSize)/2);
        sf::Vector2f imageSize(loadedSize.x-2*cutoff.x,loadedSize.y-2*cutoff.y);

        sf::Image *origImg=new sf::Image();
        origImg->create(imageSize.x,imageSize.y);
        origImg->copy(loaded,0,0,sf::IntRect(cutoff.x,cutoff.y,imageSize.x+cutoff.x,imageSize.y+cutoff.y));
        ImageManager::addImage(TempOptions.pictureToSolve,origImg);

        //Load curve data from txt files (control points and dirs).
        float scaleFactor=pieceSize.x/400.f;	//The curve control points are set for 400 pixel wide pieces.
        std::vector<sf::Vector2f> pts;
        std::vector<I16> dirs;
        loadCurveData(pts,dirs,scaleFactor);

        //Create the borders of the peices.
        _nPiecesX = imageSize.x/pieceSize.x;
        _nPiecesY = imageSize.y/pieceSize.y;
        std::vector<std::vector<PieceBorder>> borders;

        sf::Texture *black=new sf::Texture(), *red=new sf::Texture();
        std::vector<std::tuple<sf::Color,float,sf::Texture *>> borderGraphData{std::tuple<sf::Color,float,sf::Texture *>{sf::Color::Black,0.02*pieceSize.x,black},
                                                                               std::tuple<sf::Color,float,sf::Texture *>{sf::Color::Red,0.02f*pieceSize.x,red}};
        createBorders(borders,pts,dirs,pieceSize,imageSize,borderGraphData);
        ImageManager::addTexture("blackBorder",black);
        ImageManager::addTexture("redBorder",red);
        PieceBorder::setColorTextures({{sf::Color::Black,black},{sf::Color::Red,red}});

        std::string destPic("testpicture_cutted.png");
        sf::Image *dest=ImageManager::newImage(destPic);
        dest->create(1.5f*imageSize.x,1.5f*imageSize.y,sf::Color::Transparent);
        createPieceSprites(origImg,dest,borders,pieceSize);
        ImageManager::saveImage(destPic);

        //Construct pieces after I sliced the picture.
        sf::Texture *destTex=ImageManager::getTexture(destPic);
        U16 linkId=0;
        U16 destPieceSizeX=pieceSize.x*1.5f;
        U16 destPieceSizeY=pieceSize.y*1.5f;
        float offsetX = (destPieceSizeX-pieceSize.x)/2;
        float offsetY = (destPieceSizeY-pieceSize.y)/2;
        for(U16 i=0; i<_nPiecesX; ++i)
        {
            for(U16 j=0; j<_nPiecesY; ++j)
            {
                sf::Sprite sp(*destTex,sf::IntRect(i*destPieceSizeX,j*destPieceSizeY,destPieceSizeX,destPieceSizeY));
                sp.setPosition(i*pieceSize.x-offsetX,j*pieceSize.y-offsetY);
                Piece *p=new Piece(i,j,pieceSize.x,linkId,sp,std::move(borders[i][j]));
                p->setPosition(Random::randomPixel());
                Link *l=new Link(linkId);
                l->addPiece(std::unique_ptr<Piece>(p));
                _lOrderedPieces.push_back(p);
                _mapLinks.insert(std::pair<U16,std::unique_ptr<Link>>(linkId,std::unique_ptr<Link>(l)));
                ++linkId;
            }
        }
    }


    void destroy()
    {
        _mapLinks.clear();
        _lOrderedPieces.clear();
        _sName=std::string("");
        _nPiecesX=0;
        _nPiecesY=0;

        PieceBorder::clearColorTextures();
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


    void addPiecesToSelection(sf::Vector2f &mousePos_, std::unordered_set<Piece *> &selection_)
    {
        for(auto p: _lOrderedPieces)
        {
            if(p->isPointInside(mousePos_))
            {
                auto linkPieces = getLink(p->getLinkId())->getPieces();
                for(auto linkP: linkPieces)
                {
                    selection_.insert(linkP);
                    _lOrderedPieces.remove(linkP);
                }
                return;
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


    void loadCurveData(std::vector<sf::Vector2f> &pts, std::vector<I16> &dirs, const float &scaleFactor)
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

        //Load curve directions.
        std::ifstream curveDirs("curveDirs.txt");
        I16 dvar;

        while(curveDirs >> dvar)
        {
            dirs.push_back(dvar);
        }
    }


    void createBorders(std::vector<std::vector<PieceBorder>> &borders_, std::vector<sf::Vector2f> &pts_,
                       std::vector<I16> &dirs_, const sf::Vector2f &pieceSize_, const sf::Vector2f &imageSize_,
                       std::vector<std::tuple<sf::Color,float,sf::Texture *>> &borderGraphics_)
    {
        borders_.resize(_nPiecesX);
        for(auto &column: borders_)
        {
            column.resize(_nPiecesY);
        }

        std::vector<std::pair<sf::RenderTexture,std::vector<sf::Vertex>>> drawData(borderGraphics_.size());
        sf::Vector2f destPieceSize(1.5f*pieceSize_.x,1.5f*pieceSize_.y);

        for(auto &d: drawData)
        {
            d.first.create(1.5f*imageSize_.x,1.5f*imageSize_.y);
        }

        CurvyEdge topBase(true,-1, 1,pts_,dirs_);	//It's a horizontal plus top edge.
        for(U16 i=0; i<_nPiecesX; ++i)
        {
            for(U16 j=0; j<_nPiecesY; ++j)
            {
                Edge *top, *right, *bottom, *left;
                bool standLayout=(i+j)%2==0;

                if(j==0)
                {
                    top=new LineEdge(true, -1, sf::Vector2f(0.f,0.f), sf::Vector2f(pieceSize_.x,0.f));
                }
                else
                {
                    top=new CurvyEdge(topBase);
                    if(!standLayout)
                    {
                        top->rotateRight();
                        top->rotateRight();
                        top->flipInside();
                        top->move(sf::Vector2f(pieceSize_.x,0.f));
                    }
                }

                if(i==_nPiecesX-1)
                {
                    right=new LineEdge(false, -1, sf::Vector2f(pieceSize_.x,0.f), sf::Vector2f(pieceSize_.x,pieceSize_.y));
                }
                else
                {
                    right=new CurvyEdge(topBase);
                    if(standLayout)
                    {
                        right->rotateLeft();
                        right->flipInside();
                        right->move(sf::Vector2f(pieceSize_.x,pieceSize_.y));
                    }
                    else
                    {
                        right->rotateRight();
                        right->move(sf::Vector2f(pieceSize_.x,0.f));
                    }
                }

                if(j==_nPiecesY-1)
                {
                    bottom=new LineEdge(true, 1, sf::Vector2f(0.f, pieceSize_.y), sf::Vector2f(pieceSize_.x, pieceSize_.y));
                }
                else
                {
                    bottom=new CurvyEdge(topBase);
                    if(standLayout)
                    {
                        bottom->rotateRight();
                        bottom->rotateRight();
                        bottom->move(sf::Vector2f(pieceSize_.x,pieceSize_.y));
                    }
                    else
                    {
                        bottom->flipInside();
                        bottom->move(sf::Vector2f(0.f,pieceSize_.y));
                    }
                }

                if(i==0)
                {
                    left=new LineEdge(false, 1, sf::Vector2f(0.f,0.f), sf::Vector2f(0.f,pieceSize_.y));
                }
                else
                {
                    left=new CurvyEdge(topBase);
                    if(standLayout)
                    {	//Left edge is a Minus
                        left->rotateRight();
                        left->flipInside();
                    }
                    else
                    {	//Left edge is a Plus
                        left->rotateLeft();
                        left->move(sf::Vector2f(0.f,pieceSize_.y));
                    }
                }

                std::vector<Edge*> edges{top, right, bottom, left};
                for(auto &e: edges)
                {
                    e->move(sf::Vector2f(0.25f*pieceSize_.x+i*1.5f*pieceSize_.x,0.25f*pieceSize_.y+j*1.5f*pieceSize_.y));
                    for(int k=0; k < borderGraphics_.size(); ++k)
                    {
                        e->render(drawData[k].first,std::get<0>(borderGraphics_[k]),std::get<1>(borderGraphics_[k]));
                    }

                    e->move(sf::Vector2f(-0.25f*pieceSize_.x - i*0.5f*pieceSize_.x,-0.25f*pieceSize_.y - j*0.5f*pieceSize_.y));
                }

                PieceBorder border(edges,sf::IntRect(i*destPieceSize.x,j*destPieceSize.y,destPieceSize.x,destPieceSize.y),
                                   sf::Vector2f(-0.25f*pieceSize_.x + i*pieceSize_.x,-0.25f*pieceSize_.y + j*pieceSize_.y));
                borders_[i][j]=std::move(border);
            }
        }

        for(int k=0;k < borderGraphics_.size();++k)
        {
            sf::Image tempForFlip = drawData[k].first.getTexture().copyToImage();
            //tempForFlip.flipHorizontally();
            tempForFlip.flipVertically();
            (std::get<2>(borderGraphics_[k]))->loadFromImage(tempForFlip);
        }
    }


    void createPieceSprites(sf::Image *origImg, sf::Image *destImg, std::vector<std::vector<PieceBorder>> &borders, sf::Vector2f &pieceSize)
    {
        sf::Vector2u imageSize(origImg->getSize());
        U16 pieceCol=0, pieceRow=0;
        U16 iCounter=0,jCounter=0;
        U16 destPosX,destPosY;
        U16 destPieceSizeX=pieceSize.x*1.5f;
        U16 destPieceSizeY=pieceSize.y*1.5f;
        for(U16 i=0; i<imageSize.x; ++i)
        {
            jCounter=0;
            pieceRow=0;

            for(U16 j=0; j<imageSize.y; ++j)
            {
                bool standLayout=(pieceCol+pieceRow)%2==0;

                if(borders[pieceCol][pieceRow].isInside(sf::Vector2f((float)i,float(j))))
                {
                    destPosX=pieceCol;
                    destPosY=pieceRow;
                }
                else
                {
                    if(standLayout)
                    {
                        if(j < pieceRow*pieceSize.y + 0.22f*pieceSize.y)
                        {
                            destPosX=pieceCol;
                            destPosY=pieceRow-1;
                        }
                        else if(j > (pieceRow+1)*pieceSize.y - 0.22f*pieceSize.y)
                        {
                            destPosX=pieceCol;
                            destPosY=pieceRow+1;
                        }
                        else if(i < pieceCol*pieceSize.x + 0.5f*pieceSize.x)
                        {
                            destPosX=pieceCol-1;
                            destPosY=pieceRow;
                        }
                        else
                        {
                            destPosX=pieceCol+1;
                            destPosY=pieceRow;
                        }
                    }
                    else
                    {	//The piece is horizontally oriented.
                        if(i < pieceCol*pieceSize.x + 0.22f*pieceSize.x)
                        {
                            destPosX=pieceCol-1;
                            destPosY=pieceRow;
                        }
                        else if(i > (pieceCol+1)*pieceSize.x - 0.22f*pieceSize.x)
                        {
                            destPosX=pieceCol+1;
                            destPosY=pieceRow;
                        }
                        else if(j < pieceRow*pieceSize.y + 0.5f*pieceSize.y)
                        {
                            destPosX=pieceCol;
                            destPosY=pieceRow-1;
                        }
                        else
                        {
                            destPosX=pieceCol;
                            destPosY=pieceRow+1;
                        }
                    }
                }

                U16 origCenterX=destPosX*pieceSize.x+pieceSize.x/2;
                U16 origCenterY=destPosY*pieceSize.y+pieceSize.y/2;
                U16 destCenterX=destPosX*destPieceSizeX+destPieceSizeX/2;
                U16 destCenterY=destPosY*destPieceSizeY+destPieceSizeY/2;
                I16 diffFromOrigCenterX=i-origCenterX;
                I16 diffFromOrigCenterY=j-origCenterY;

                destImg->setPixel(destCenterX+diffFromOrigCenterX, destCenterY+diffFromOrigCenterY, origImg->getPixel(i,j));

                if(++jCounter==pieceSize.y)
                {
                    ++pieceRow;
                    jCounter=0;
                }
            }

            if(++iCounter==pieceSize.x)
            {
                ++pieceCol;
                iCounter=0;
            }
        }
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
}
