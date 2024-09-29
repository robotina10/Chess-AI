#pragma once
#include <iostream>
#include "ChessEngine.h"
#include "draw.h"
#include "game.h"
#include "perft.h"
#include "button.h"


int main()
{   
    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Chess", sf::Style::Close);
    window.setFramerateLimit(60);

    ChessEngine chess;

    bool whiteView = true;
    bool isPressed = false;
    bool mouseMoved = false;
    int from  = -1;
    Pieces selectedPiece = EMPTY;
    Button promotionSelectionBtn("img/prom.png");

   getPerftWithTime(chess.board, 2); 
    if (0) {
        chess.board.setWhiteTurn(0);
        whiteView = false;
    }
    chess.board.generateLegalMoves(chess.moveList);
    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
            {
                isPressed = true;
                if (from == -1) {
                    from = calcSquarePos(sf::Mouse::getPosition(window), whiteView);
                    selectedPiece = chess.board.getPiece(from);
                    continue;
                }
                int to = calcSquarePos(sf::Mouse::getPosition(window), whiteView);
                int moveIndex = getMoveIndex(chess.moveList, from, to);
                if (moveIndex == -1) {
                    from = to;
                }
                else {
                    SpecialMove promotion = NONE;
                    if (chess.moveList.moves[moveIndex].isPromotion()) {
                        promotionSelectionBtn.drawAndDisplay(window);
                        while (true) {
                            promotionSelectionBtn.drawAndDisplay(window);
                            while (window.pollEvent(event))
                            {
                                switch (event.type) {
                                case sf::Event::Closed:
                                    window.close();
                                    break;
                                case sf::Event::MouseButtonPressed:
                                    promotion = promotionSelectionBtn.pieceClicked(calcSquarePos(sf::Mouse::getPosition(window), whiteView));
                                    break;
                                }
                            }
                            if (promotion)
                                break;
                        }
                    }
                    makeMove(window, chess, from, to, selectedPiece, moveIndex, promotion);
                    std::cout << chess.board.saveBoardToFen() << "\n";
                    //getPerftWithTime(chess.board, 1);

                    switch (chess.gameState()) {
                    case DRAW:
                        std::cout << "DRAW\n";
                        break;
                    case WHITE_WIN:
                        std::cout << "WHITE_WIN\n";
                        break;
                    case BLACK_WIN:
                        std::cout << "BLACK_WIN\n";
                        break;
                    }
                                    }
                break;
            }
            case sf::Event::MouseButtonReleased:
                isPressed = false;
                mouseMoved = false;
                //makeMove(window, chess, from , selectedPiece);
                break;
            case sf::Event::MouseMoved:
                if (!isPressed || chess.board.isEmpty(selectedPiece))
                    continue;
                mouseMoved = true;
                sf::Vector2i pos = sf::Mouse::getPosition(window);
                dragPiece(window, chess.board, pos, calcSquarePos(pos, whiteView), selectedPiece);
                break;
            }
        }
        draw(window, chess, from, whiteView);
    }
    return 0;
}
