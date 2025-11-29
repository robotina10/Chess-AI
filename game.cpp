#include <fstream>
#include <cstdlib>
#include <optional>
#include "game.h"
#include "perft.h"
#include "draw.h"

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
    //chess.board.setWhiteTurn(settings.whiteView);
    initSounds();
}

void Game::startGame()
{
	init();
	//std::thread perftThread(getPerftWithTime, chess.board, 6);
	chess.board.generateLegalMoves(chess.moveList);
	while (win.isOpen())
	{
		while (auto event = win.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				win.close();
			}
			else if (const auto keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->code == sf::Keyboard::Key::Backspace)
					unMakeMove();
			}
			else if (const auto mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
			{
				moveState.isPressed = true;
				if (moveState.from == -1) {
					moveState.from = calcSquarePos(mouseButtonPressed->position);
					continue;
				}
				moveState.to = calcSquarePos(mouseButtonPressed->position);
				moveState.moveIndex = getMoveIndex();
				if (moveState.moveIndex == -1) {
					moveState.from = moveState.to;
					continue;
				}
				if (chess.moveList.moves[moveState.moveIndex].isPromotion()) {
					promSelectionBtn.setPos(sf::Vector2f((moveState.to % 8) * SIDE, 0));
					while (true) {
						win.draw(promSelectionBtn);
						win.display();
						while (auto promotionEvent = win.pollEvent())
						{
							if (promotionEvent->is<sf::Event::Closed>())
								win.close();
							else if (const auto promotionMousePressed = promotionEvent->getIf<sf::Event::MouseButtonPressed>()) {
								moveState.promotion = promPieceSelected(promSelectionBtn.getPos(), calcSquarePos(promotionMousePressed->position));
							}
						}
						if (moveState.promotion)
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

				switch (chess.gameState()) { // duplicate make func
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
			else if (event->is<sf::Event::MouseButtonReleased>())
			{
				moveState.isPressed = false;
				moveState.mouseMoved = false;
				//makeMove(window, chess, from , selectedPiece);
			}
			else if (const auto mouseMoved = event->getIf<sf::Event::MouseMoved>())
			{
				if (!moveState.isPressed || chess.board.isEmpty(moveState.from))
					continue;
				moveState.mouseMoved = true;
				sf::Vector2i pos = mouseMoved->position;
				(void)pos;
				//dragPiece(win, chess.board, pos, calcSquarePos(pos), moveInfo.selectedPiece);
			}
		}
		draw();
	}
}

void Game::playSound(Sounds soundType)
{
	if (soundType == Sounds::CAPTURE_SOUND) {
		sf::Sound sound(soundCaptureBuffer);
		sound.play();
	}
	else if (soundType == Sounds::MOVE_SOUND) {
		sf::Sound sound(soundMoveBuffer);
		sound.play();
	}
}

SpecialMove Game::promPieceSelected(sf::Vector2f btnPos, int squareClicked)
{
    int startingSquareOfImage = btnPos.x / SIDE;
    if (startingSquareOfImage == squareClicked)
        return QUEEN_PROM;
    else if (startingSquareOfImage + 8 == squareClicked)
        return KNIGHT_PROM;
    else if (startingSquareOfImage + 16 == squareClicked)
        return ROOK_PROM;
    else if (startingSquareOfImage + 24 == squareClicked)
        return BISHOP_PROM;
    else
        return NONE;
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
    if (settings.whiteView)
        return p.x + p.y * 8;
    return 63 - (p.x + p.y * 8);
}

int Game::getMoveIndex()
{
    for (int i = 0; i < chess.moveList.count; i++) {
        if (moveState.from == chess.moveList.moves[i].getFrom() && moveState.to == chess.moveList.moves[i].getTo())
            return i;
    }
    return -1;
}

void Game::makeMove()
{
    Move move = chess.moveList.moves[moveState.moveIndex];
    if (move.isPromotion()) {
        move.setPromotion(moveState.promotion);
        moveState.promotion = NONE;
    }
    chess.playedMoves.push_back(Position(move, chess.board.getPosInfo()));
    chess.board.makeMove(move);
    chess.moveList.count = 0;
    chess.board.generateLegalMoves(chess.moveList);
    moveState.from = -1;
    moveState.to = -1;
}

void Game::unMakeMove()
{
    if (!chess.playedMoves.empty()) {
        chess.board.unMakeMove(chess.playedMoves.back().move, chess.playedMoves.back().info);
        chess.moveList.count = 0;
        chess.board.generateLegalMoves(chess.moveList);
    }
}
void Game::draw()
{
	win.clear();
	drawBoard(win);
	highlightSquare(win, chess.board, moveState.from, settings.whiteView);
	drawPossibleMoves(win, chess.moveList, moveState.from, settings.whiteView);
	drawPieces(win, chess.board, settings.whiteView);
	drawRanksFiles(win);
	win.display();
}

void Game::computer()
{
    //neural Network
    std::ofstream myMove("chessNeuralNet/input.txt");
    myMove << chess.playedMoves.back().move.getStr();

    std::string venvPython = "chessNeuralNet/venv/Scripts/python.exe";
    std::string scriptPath = "chessNeuralNet/main.py";

    std::string command = venvPython + " " + scriptPath;
    system(command.c_str());

    std::ifstream nnMove("chessNeuralNet/output.txt");
    std::string moveStr;
    std::getline(nnMove, moveStr);
    std::cout << moveStr;
    
    myMove.close();
    nnMove.close();

    Move move;
	for (int i = 0; i < chess.moveList.count; i++) {
		if (chess.moveList.moves[i].getStr() == moveStr) {
			move = chess.moveList.moves[i];
			break;
		}
	}

    chess.playedMoves.push_back(Position(move, chess.board.getPosInfo()));
    chess.board.makeMove(move);
    chess.moveList.count = 0;
    chess.board.generateLegalMoves(chess.moveList);
    //"programmed ai"
    /*
    Move move = chess.board.searchPosition();
    chess.playedMoves.push_back(Position(move, chess.board.getPosInfo()));
    chess.board.makeMove(move);
    chess.moveList.count = 0;
    chess.board.generateLegalMoves(chess.moveList);
    */
}