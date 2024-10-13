#include "game.h"
#include "perft.h"

sf::SoundBuffer soundCaptureBuffer;
sf::SoundBuffer soundMoveBuffer;

void initSounds()
{
    if (!soundCaptureBuffer.loadFromFile("sound/sounds_capture.wav"))
        std::cout << "Cannot load sound/sounds_capture.wav \n";
    if (!soundMoveBuffer.loadFromFile("sound/sounds_move.wav"))
        std::cout << "Cannot load sound/sounds_move.wav \n";
}

void Game::init()
{
    initSounds();
}

void Game::startGame()
{
    init();
    //std::thread perftThread(getPerftWithTime, chess.board, 5);
    chess.board.generateLegalMoves(chess.moveList);
;
    while (win.isOpen())
    {
        sf::Event event;
        while (win.pollEvent(event))
        {
            switch (event.type) {
            case sf::Event::Closed:
                win.close();
                break;
            case sf::Event::MouseButtonPressed:
            {
                moveInfo.isPressed = true;
                if (moveInfo.from == -1) {
                    moveInfo.from = calcSquarePos(sf::Mouse::getPosition(win));
                    continue;
                }
                moveInfo.to = calcSquarePos(sf::Mouse::getPosition(win));
                moveInfo.moveIndex = getMoveIndex();
                if (moveInfo.moveIndex == -1) {
                    moveInfo.from = moveInfo.to;
                    continue;
                }
                if (chess.moveList.moves[moveInfo.moveIndex].isPromotion()) {
                    //promotionSelectionBtn.drawAndDisplay(window);
                    while (true) {
                        //promotionSelectionBtn.drawAndDisplay(window);
                        while (win.pollEvent(event))
                        {
                            switch (event.type) {
                            case sf::Event::Closed:
                                win.close();
                                break;
                            case sf::Event::MouseButtonPressed:
                                moveInfo.promotion = QUEEN_PROM;
                                //promotion = promotionSelectionBtn.pieceClicked(calcSquarePos(sf::Mouse::getPosition(window), whiteView));
                                break;
                            }
                        }
                        if (moveInfo.promotion)
                            break;
                    }
                }
                makeMove();
                playSound(Sounds::MOVE_SOUND);
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

                switch (settings.mode) {
                case AI:
                    computer();
                    break;
                }

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
                break;
            }
            case sf::Event::MouseButtonReleased:
                moveInfo.isPressed = false;
                moveInfo.mouseMoved = false;
                //makeMove(window, chess, from , selectedPiece);
                break;
            case sf::Event::MouseMoved:
                if (!moveInfo.isPressed || chess.board.isEmpty(moveInfo.from))
                    continue;
                moveInfo.mouseMoved = true;
                sf::Vector2i pos = sf::Mouse::getPosition(win);
                //dragPiece(win, chess.board, pos, calcSquarePos(pos), moveInfo.selectedPiece);
                break;
            }
        }
        draw(win, chess, moveInfo.from, settings.whiteTurn);
    }
}

void Game::playSound(Sounds soundType)
{
    sf::Sound sound;
    if (soundType == Sounds::CAPTURE_SOUND) {
        sound.setBuffer(soundCaptureBuffer);
        sound.play();
    }
    else if (soundType == Sounds::MOVE_SOUND) {
        sound.setBuffer(soundMoveBuffer);
        sound.play();
    }
}

int Game::calcSquarePos(sf::Vector2i p)
{
    p.x /= SIDE;
    p.y /= SIDE;
    if (p.x >= ROWS)
        p.x = ROWS - 1;
    if (p.y >= COLS)
        p.y = COLS - 1;
    if (p.x < 0)
        p.x = 0;
    if (p.y < 0)
        p.y = 0;
    if (settings.whiteTurn)
        return p.x + p.y * 8;
    return 63 - (p.x + p.y * 8);
}

int Game::getMoveIndex()
{
    for (int i = 0; i < chess.moveList.count; i++) {
        if (moveInfo.from == chess.moveList.moves[i].getFrom() && moveInfo.to == chess.moveList.moves[i].getTo())
            return i;
    }
    return -1;
}

void Game::makeMove()
{
    Move move = chess.moveList.moves[moveInfo.moveIndex];
    if (move.isPromotion()) {
        move.setPromotion(moveInfo.promotion);
        moveInfo.promotion = NONE;
    }
    chess.gamePositions.push_back(chess.board);
    chess.board.makeMove(move);
    chess.moveList.count = 0;
    chess.board.generateLegalMoves(chess.moveList);
    moveInfo.from = -1;
    moveInfo.to = -1;
}

void Game::computer()
{
    chess.gamePositions.push_back(chess.board);
    chess.board.makeMove(chess.board.searchRoot(5));
    chess.moveList.count = 0;
    chess.board.generateLegalMoves(chess.moveList);
}