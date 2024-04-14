#pragma once
#include <iostream>
#include "ChessEngine.h"
#include "draw.h"
#include "game.h"

int main()
{   
    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Chess", sf::Style::Close);
    window.setFramerateLimit(30);

    ChessEngine chess;

    int white = 1; //1 white 0 black
    bool isPressed = false;
    bool mouseMoved = false;
    int from  = -1;
    Pieces selectedPiece = EMPTY;

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
                    from = calcSquarePos(sf::Mouse::getPosition(window));
                    selectedPiece = chess.board.getPiece(from);
                    continue;
                }
                int to = calcSquarePos(sf::Mouse::getPosition(window));
                int i = getMoveIndex(chess.moveList, from, to);
                if (i == -1) {
                    from = to;
                }
                else {
                    int promotion = NONE;
                    if (chess.moveList.moves[i].isPromotion()) {
                        std::cin >> promotion;
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
                dragPiece(window, chess.board, pos, calcSquarePos(pos), selectedPiece);
                break;
            }
        }
        draw(window, chess, from, white);
    }
    return 0;
}
