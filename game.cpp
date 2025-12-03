#include <fstream>
#include <cstdlib>
#include <optional>
#include "game.h"
#include "perft.h"
#include "draw.h"
#include "debug.h"

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
	//std::thread perftThread(perftTestDefaultFen);
	//Debugger dbg;
	//dbg.comparePerft(chess.board, 5, "stockfish.exe");

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
				moveState.mouseMoved = false;
				moveState.dragPos = sf::Vector2f(mouseButtonPressed->position.x, mouseButtonPressed->position.y);

				int clickedSquare = calcSquarePos(mouseButtonPressed->position);
				int clickedPiece = getPieceIndexAt(clickedSquare);

				if (moveState.from == -1) {
					if (clickedPiece != -1) {
						moveState.from = clickedSquare;
						moveState.draggedPieceIndex = clickedPiece;
					}
				}
				else {
					if (clickedSquare == moveState.from) {
						moveState.draggedPieceIndex = clickedPiece;
					}
					else {
						bool switchSelection = false;
						if (clickedPiece != -1) {
							int fromPiece = getPieceIndexAt(moveState.from);
							if (fromPiece != -1) {
								bool fromWhite = (fromPiece % 2 != 0);
								bool clickedWhite = (clickedPiece % 2 != 0);
								if (fromWhite == clickedWhite) {
									switchSelection = true;
								}
							}
						}

						if (switchSelection) {
							moveState.from = clickedSquare;
							moveState.draggedPieceIndex = clickedPiece;
							moveState.to = -1;
						}
						else {
							moveState.to = clickedSquare;
						}
					}
				}
			}
			else if (event->is<sf::Event::MouseButtonReleased>())
			{
				bool attemptedMove = false;

				if (moveState.isPressed && moveState.mouseMoved) {
					moveState.to = calcSquarePos(sf::Mouse::getPosition(win));
					moveState.moveIndex = getMoveIndex();
					attemptedMove = true;
				}
				else if (moveState.isPressed && !moveState.mouseMoved) {
					if (moveState.from != -1 && moveState.to != -1 && moveState.from != moveState.to) {
						moveState.moveIndex = getMoveIndex();
						attemptedMove = true;
					}
				}

				if (attemptedMove) {
					if (moveState.moveIndex != -1) {
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
						playSound();

						moveState.from = -1;
					}
					else {
						if (moveState.mouseMoved) {
							moveState.from = -1;
						}
						if (moveState.to != -1 && !chess.board.isEmpty(moveState.to) && !moveState.mouseMoved) {
							moveState.from = moveState.to;
							moveState.draggedPieceIndex = getPieceIndexAt(moveState.from);
						}
					}
				}

				moveState.isPressed = false;
				moveState.mouseMoved = false;
				moveState.to = -1;
				moveState.draggedPieceIndex = -1;
				draw();
			}
			else if (const auto mouseMoved = event->getIf<sf::Event::MouseMoved>())
			{
				moveState.dragPos = sf::Vector2f(mouseMoved->position.x, mouseMoved->position.y);

				if (moveState.isPressed && moveState.from != -1) {
					moveState.mouseMoved = true;
				}
			}
			if (settings.whiteView != chess.board.isWhiteTurn() && settings.mode == AI) {
				computer();
				playSound();
			}
		}
		draw();
	}
}

void Game::computer()
{
	//neural Network
	/*std::ofstream myMove("chessNeuralNet/input.txt");
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
	chess.board.generateLegalMoves(chess.moveList);*/
	//"programmed ai"

	Move move = chess.board.searchPosition();
	chess.playedMoves.push_back(Position(move, chess.board.getPosInfo()));
	chess.board.makeMove(move);
	chess.moveList.count = 0;
	chess.board.generateLegalMoves(chess.moveList);

}

void Game::playSound()
{
	sf::SoundBuffer* targetBuffer = nullptr;

	if (chess.playedMoves[chess.playedMoves.size() - 1].move.isCapture() || chess.playedMoves[chess.playedMoves.size() - 1].move.getSpecialMove() == EN_PASSANT) {
		targetBuffer = &soundCaptureBuffer;
	}
	else {
		targetBuffer = &soundMoveBuffer;
	}
	if (soundPlayer.has_value())
		soundPlayer->setBuffer(*targetBuffer);
	else {
		soundPlayer.emplace(*targetBuffer);
	}
	soundPlayer->play();
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

int Game::getPieceIndexAt(int square)
{
	for (int i = 0; i < 12; i++) {
		if ((chess.board.getBitboard(i) >> square) & 1) {
			return i;
		}
	}
	return -1;
}

void Game::draw()
{
	win.clear();
	drawBoard(win);
	highlightSquare(win, chess.board, moveState.from, settings.whiteView);
	drawPossibleMoves(win, chess.moveList, moveState.from, settings.whiteView);

	int excludeSq = (moveState.isPressed && moveState.mouseMoved) ? moveState.from : -1;
	drawPieces(win, chess.board, settings.whiteView, excludeSq);

	drawRanksFiles(win, settings.whiteView);

	if (moveState.isPressed && moveState.mouseMoved && moveState.draggedPieceIndex != -1) {
		sf::Vector2i mousePos(static_cast<int>(moveState.dragPos.x), static_cast<int>(moveState.dragPos.y));
		dragPiece(win, chess.board, mousePos, -1, moveState.draggedPieceIndex);
	}

	win.display();
}