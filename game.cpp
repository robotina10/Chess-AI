#include "game.h"
//Button promotionSelectionBtn("img/prom.png");

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
    //std::thread perftThread(getPerftWithTime, chess.board, 6);
    initSounds();
}

void Game::startGame()
{
    init();
    chess.board.generateLegalMoves(chess.moveList);
    bool isPressed = false;
    bool mouseMoved = false;
    int from = -1;
    Pieces selectedPiece = EMPTY;

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
                isPressed = true;
                if (from == -1) {
                    from = calcSquarePos(sf::Mouse::getPosition(win));
                    selectedPiece = chess.board.getPiece(from);
                    continue;
                }
                int to = calcSquarePos(sf::Mouse::getPosition(win));
                int moveIndex = getMoveIndex(from, to);
                if (moveIndex == -1) {
                    from = to;
                }
                else {
                    SpecialMove promotion = NONE;
                    if (chess.moveList.moves[moveIndex].isPromotion()) {
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
                                    //promotion = promotionSelectionBtn.pieceClicked(calcSquarePos(sf::Mouse::getPosition(window), whiteView));
                                    break;
                                }
                            }
                            if (promotion)
                                break;
                        }
                    }
                    makeMove(from, to, selectedPiece, moveIndex, promotion); // why window etc
                    playSound(MOVE_SOUND);

                    switch (settings.mode) {
                    case AI:
                        //computer(chess);
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
                sf::Vector2i pos = sf::Mouse::getPosition(win);
                dragPiece(win, chess.board, pos, calcSquarePos(pos), selectedPiece);
                break;
            }
        }
        draw(win, chess, from, settings.whiteTurn);
    }
}

void Game::setSettings(Settings settings)
{
    chess.board.setWhiteTurn(settings.whiteTurn);

}

void Game::playSound(Sounds soundType)
{
    sf::Sound sound;
    if (soundType == CAPTURE_SOUND) {
        sound.setBuffer(soundCaptureBuffer);
        sound.play();
    }
    else if (soundType == MOVE_SOUND) {
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

int Game::getMoveIndex(int from, int to)
{
    for (int i = 0; i < chess.moveList.count; i++) {
        if (from == chess.moveList.moves[i].getFrom() && to == chess.moveList.moves[i].getTo()) {
            return i;
        }
    }
    return -1;
}

void Game::makeMove(int &from, int to, Pieces &piece, int moveIndex, SpecialMove promotion)
{
    Move move = chess.moveList.moves[moveIndex];
    if (move.isPromotion()) {
        move.setPromotion(promotion);
    }
    chess.gamePositions.push_back(chess.board);
    chess.board.makeMove(move);
    chess.moveList.count = 0;
    chess.board.generateLegalMoves(chess.moveList);
    piece = EMPTY;
    from = -1;
}

void Game::computer()
{
    chess.gamePositions.push_back(chess.board);
    chess.board.makeMove(chess.board.rootNegaMax(3));
    chess.moveList.count = 0;
    chess.board.generateLegalMoves(chess.moveList);
}