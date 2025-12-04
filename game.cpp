#include <fstream>
#include <cstdlib>
#include <optional>
#include "game.h"
#include "perft.h"
#include "draw.h"
#include "debug.h"

sf::SoundBuffer soundCaptureBuffer;
sf::SoundBuffer soundMoveBuffer;

void Game::init()
{
	if (!soundCaptureBuffer.loadFromFile("sound/sounds_capture.wav"))
		std::cout << "Cannot load sound/sounds_capture.wav \n";
	if (!soundMoveBuffer.loadFromFile("sound/sounds_move.wav"))
		std::cout << "Cannot load sound/sounds_move.wav \n";
	if (!font.openFromFile("font/celtig.ttf")) {
		std::cout << "Failed to load font/Poppins-Bold.ttf\n";
	}
}

void Game::startGame()
{
	init();
	chess.board.generateLegalMoves(chess.moveList);
	if (handleGameOver()) return;
	//std::thread perftThread(getPerftWithTime, chess.board, 6);
	//std::thread perftThread(perftTestDefaultFen);
	//Debugger dbg;
	//dbg.comparePerft(chess.board, 5, "stockfish.exe");

	while (win.isOpen())
	{
		draw();
		while (auto event = win.pollEvent())
		{
			if (settings.whiteView != chess.board.isWhiteTurn() && settings.mode == AI) {
				computer();
				playSound();
				if (handleGameOver()) {
					return;
				}
			}

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
							moveState.promotion = selectPromotion(moveState.to);
							if (!win.isOpen()) return;
						}
						makeMove();
						playSound();
						if (handleGameOver()) {
							return;
						}
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
		}
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

bool Game::handleGameOver()
{
	std::string resultText = "";
	sf::Color textColor = sf::Color::White;

	if (chess.board.isDraw(chess.moveList.count)) {
		resultText = "Draw";
	}
	else if (chess.board.isCheckmate(chess.moveList.count)) {
		bool whiteLost = chess.board.isWhiteTurn();

		if (settings.whiteView == whiteLost) {
			resultText = "You Lost";
		}
		else {
			resultText = "You Win!";
		}
	}
	else {
		return false;
	}

	sf::Text titleText(font);
	titleText.setString(resultText);
	titleText.setCharacterSize(60);
	titleText.setFillColor(textColor);
	titleText.setStyle(sf::Text::Bold);

	sf::FloatRect textBounds = titleText.getLocalBounds();
	titleText.setOrigin({ textBounds.position.x + textBounds.size.x / 2.0f, textBounds.position.y + textBounds.size.y / 2.0f });
	titleText.setPosition({ (SIDE * 8) / 2.0f, (SIDE * 8) / 2.0f - 50.0f });

	sf::RectangleShape btnRect({ 260.f, 60.f });
	btnRect.setOrigin({ 130.f, 30.f });
	btnRect.setPosition({ (SIDE * 8) / 2.0f, (SIDE * 8) / 2.0f + 50.0f });
	btnRect.setFillColor(sf::Color::White);

	sf::Text btnText(font);
	btnText.setString("Back to Menu");
	btnText.setCharacterSize(24);
	btnText.setFillColor(sf::Color::Black);

	sf::FloatRect btnTextBounds = btnText.getLocalBounds();
	btnText.setOrigin({ btnTextBounds.position.x + btnTextBounds.size.x / 2.0f, btnTextBounds.position.y + btnTextBounds.size.y / 2.0f });
	btnText.setPosition(btnRect.getPosition());

	sf::RectangleShape overlay({ (float)SIDE * 8, (float)SIDE * 8 });
	overlay.setFillColor(sf::Color(0, 0, 0, 150));

	while (win.isOpen())
	{
		while (auto event = win.pollEvent())
		{
			if (event->is<sf::Event::Closed>()) {
				win.close();
				return true;
			}
			else if (const auto mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
				if (mouseReleased->button == sf::Mouse::Button::Left) {
					sf::Vector2f mousePos((float)mouseReleased->position.x, (float)mouseReleased->position.y);

					if (btnRect.getGlobalBounds().contains(mousePos)) {
						return true;
					}
				}
			}
		}

		sf::Vector2i mPos = sf::Mouse::getPosition(win);
		sf::Vector2f mousePosF((float)mPos.x, (float)mPos.y);
		if (btnRect.getGlobalBounds().contains(mousePosF)) {
			btnRect.setFillColor(sf::Color(200, 200, 200));
		}
		else {
			btnRect.setFillColor(sf::Color::White);
		}

		win.clear();
		drawBoard(win);
		drawPieces(win, chess.board, settings.whiteView);

		win.draw(overlay);
		win.draw(titleText);
		win.draw(btnRect);
		win.draw(btnText);

		win.display();
	}
	return true;
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

SpecialMove Game::selectPromotion(int toSquare)
{
	bool whiteTurn = chess.board.isWhiteTurn();
	int col = toSquare % 8;
	int row = toSquare / 8;

	float visX = static_cast<float>((settings.whiteView ? col : 7 - col) * SIDE);
	float visY = static_cast<float>((settings.whiteView ? row : 7 - row) * SIDE);
	float sideF = static_cast<float>(SIDE);

	int direction = (visY < 4 * sideF) ? 1 : -1;

	std::string colorPrefix = whiteTurn ? "w" : "b";
	std::vector<std::string> suffixes = { "q", "r", "b", "n" };
	std::vector<SpecialMove> promos = { QUEEN_PROM, ROOK_PROM, BISHOP_PROM, KNIGHT_PROM };

	std::vector<sf::Texture> textures(4);
	for (int i = 0; i < 4; i++) {
		if (!textures[i].loadFromFile("img/" + colorPrefix + suffixes[i] + ".png")) {
			std::cout << "Cannot load promotion img\n";
		}
		textures[i].setSmooth(true);
	}

	while (win.isOpen())
	{
		while (const std::optional<sf::Event> event = win.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				win.close();
				return NONE;
			}
			else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
			{
				if (mouseReleased->button == sf::Mouse::Button::Left)
				{
					sf::Vector2f mPos(static_cast<float>(mouseReleased->position.x), static_cast<float>(mouseReleased->position.y));

					for (int i = 0; i < 4; i++) {
						float itemY = visY + (i * direction * sideF);

						sf::FloatRect btnRect({ visX, itemY }, { sideF, sideF });

						if (btnRect.contains(mPos)) {
							return promos[i];
						}
					}
				}
			}
		}
		win.clear();
		drawBoard(win);
		highlightSquare(win, chess.board, moveState.from, settings.whiteView, HIGHLIGHTED_WHITE, HIGHLIGHTED_BLACK);
		drawPossibleMoves(win, chess.moveList, moveState.from, settings.whiteView);
		int excludeSq = (moveState.isPressed && moveState.mouseMoved) ? moveState.from : -1;
		drawPieces(win, chess.board, settings.whiteView, excludeSq);
		drawRanksFiles(win, settings.whiteView);

		sf::Vector2i mousePosI = sf::Mouse::getPosition(win);
		sf::Vector2f mousePos(static_cast<float>(mousePosI.x), static_cast<float>(mousePosI.y));

		for (int i = 0; i < 4; i++) {
			float itemY = visY + (i * direction * sideF);

			sf::RectangleShape bg(sf::Vector2f(sideF, sideF));
			bg.setPosition({ visX, itemY });

			sf::FloatRect rect({ visX, itemY }, { sideF, sideF });
			bool hover = rect.contains(mousePos);

			bg.setFillColor(hover ? sf::Color(180, 180, 180) : sf::Color(240, 240, 240));
			bg.setOutlineThickness(2);
			bg.setOutlineColor(sf::Color::Black);
			win.draw(bg);

			sf::Sprite pSprite(textures[i]);
			pSprite.setPosition({ visX + sideF / 10.0f, itemY + sideF / 10.0f });
			pSprite.setScale({ 1.3f, 1.3f });
			win.draw(pSprite);
		}

		win.display();
	}
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
		chess.playedMoves.pop_back();
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
	highlightSquare(win, chess.board, moveState.from, settings.whiteView, HIGHLIGHTED_WHITE, HIGHLIGHTED_BLACK);
	if (!chess.playedMoves.empty()) {
		highlightSquare(win, chess.board, chess.playedMoves[chess.playedMoves.size() - 1].move.getFrom(), settings.whiteView, LAST_MOVE_WHITE, LAST_MOVE_BLACK);
		highlightSquare(win, chess.board, chess.playedMoves[chess.playedMoves.size() - 1].move.getTo(), settings.whiteView, LAST_MOVE_WHITE, LAST_MOVE_BLACK);
	}
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