#pragma once
#include <iostream>
#include "ChessEngine.h"
#include "draw.h"
#include "game.h"
#include "perft.h"

//https://core.ac.uk/download/572627675.pdf
//https://www.chessprogramming.org/Checks_and_Pinned_Pieces_(Bitboards)
//https://www.chessprogramming.org/Square_Attacked_By#AnyAttackBySide
//https://www.chessprogramming.org/Perft_Results
//https://www.chessprogramming.org/index.php?title=Bitboards&mobileaction=toggle_view_desktop



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

    getPerftWithTime(chess.board, 4);

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
                int i = getMoveIndex(chess.moveList, from, to);
                if (i == -1) {
                    from = to;
                }
                else {
                    int promotion = NONE;
                    if (chess.moveList.moves[i].isPromotion()) {
                    }
                    makeMove(window, chess, from, to, selectedPiece, i, (SpecialMove)promotion);
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
