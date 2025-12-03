#pragma once
#include <string>
#include "board.h"
#include "debug.h"
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>
#include <map>

class Debugger {
public:
    void comparePerft(Board& board, int depth, std::string stockfishPath = "stockfish.exe");

private:
    std::map<std::string, U64> getStockfishPerft(std::string fen, int depth, std::string stockfishPath);

    U64 myPerft(Board& board, int depth);

    std::string runStockfishCommand(std::string cmd);
};

std::string Debugger::runStockfishCommand(std::string cmd) {
    std::array<char, 128> buffer;
    std::string result;

    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::map<std::string, U64> Debugger::getStockfishPerft(std::string fen, int depth, std::string stockfishPath) {
    std::map<std::string, U64> results;

    std::string command = "(echo position fen " + fen + " && echo go perft " + std::to_string(depth) + ") | " + stockfishPath;

    std::string output = runStockfishCommand(command);
    std::stringstream ss(output);
    std::string line;


    while (std::getline(ss, line)) {
        if (line.rfind("info", 0) == 0) continue;

        if (line.find(':') != std::string::npos && line.find("Nodes searched") == std::string::npos) {
            std::stringstream lineStream(line);
            std::string moveStr;
            U64 nodes;

            lineStream >> moveStr;

            if (!moveStr.empty() && moveStr.back() == ':') {
                moveStr.pop_back();
            }

            lineStream >> nodes;

            if (!moveStr.empty()) {
                results[moveStr] = nodes;
            }
        }
    }
    return results;
}

U64 Debugger::myPerft(Board& board, int depth) {
    if (depth == 0) return 1;

    MoveList moveList(256);
    board.generateLegalMoves(moveList);

    if (depth == 1) return moveList.count;

    U64 nodes = 0;
    PosInfo info = board.getPosInfo();

    for (int i = 0; i < moveList.count; i++) {
        board.makeMove(moveList.moves[i]);
        nodes += myPerft(board, depth - 1);
        board.unMakeMove(moveList.moves[i], info);
    }
    return nodes;
}

void Debugger::comparePerft(Board& board, int depth, std::string stockfishPath) {
    if (depth == 0) return;

    std::cout << "\n--------------------------------------------------\n";
    std::cout << "COMPARING DEPTH " << depth << " AT FEN: " << board.saveBoardToFen() << "\n";
    std::cout << "--------------------------------------------------\n";

    // 1. Získat výsledky ze Stockfishe
    std::map<std::string, U64> sfResults = getStockfishPerft(board.saveBoardToFen(), depth, stockfishPath);

    // 2. Spoèítat výsledky tvého enginu
    std::map<std::string, U64> myResults;
    MoveList moveList(256);
    board.generateLegalMoves(moveList);
    PosInfo info = board.getPosInfo();

    U64 myTotal = 0;

    for (int i = 0; i < moveList.count; i++) {
        std::string moveStr = moveList.moves[i].getStr();

        board.makeMove(moveList.moves[i]);
        U64 nodes = myPerft(board, depth - 1);
        board.unMakeMove(moveList.moves[i], info);

        myResults[moveStr] = nodes;
        myTotal += nodes;
    }

    // 3. Porovnání
    bool errorFound = false;

    // A) Kontrola tahù, které má Stockfish
    for (auto const& [move, sfNodes] : sfResults) {
        if (myResults.find(move) == myResults.end()) {
            std::cout << "[MISSING MOVE] Stockfish has " << move << ", but you don't!\n";
            errorFound = true;

            // Zkusíme najít ten tah a provést ho pro debug (pokud by to šlo)
            // Tady je to tìžké, protože tvùj engine ten tah nevygeneroval.
        }
        else {
            if (myResults[move] != sfNodes) {
                std::cout << "[WRONG COUNT] Move " << move << ": You = " << myResults[move] << ", SF = " << sfNodes << "\n";

                // REKURZIVNÍ VOLÁNÍ NA CHYBNÉ VÌTVI
                std::cout << ">>> DIVING INTO " << move << " <<<\n";

                // Musíme najít ten tah v MoveListu, abychom ho zahráli
                for (int i = 0; i < moveList.count; i++) {
                    if (moveList.moves[i].getStr() == move) {
                        board.makeMove(moveList.moves[i]);
                        comparePerft(board, depth - 1, stockfishPath);
                        board.unMakeMove(moveList.moves[i], info);
                        break;
                    }
                }
                errorFound = true;
                // Obvykle staèí najít první chybu a jít do hloubky, 
                // pokud chceš vidìt všechny, smaž 'break' nebo 'return' po rekurzi.
                return;
            }
        }
    }

    // B) Kontrola tahù, které máš ty navíc
    for (auto const& [move, myNodes] : myResults) {
        if (sfResults.find(move) == sfResults.end()) {
            std::cout << "[ILLEGAL MOVE] You have " << move << ", but Stockfish doesn't!\n";
            errorFound = true;
            // Tady jsi vygeneroval nelegální tah.
            // Mùžeme se podívat, proè si engine myslí, že je legální.
        }
    }

    if (!errorFound) {
        std::cout << "Depth " << depth << " OK. Total nodes: " << myTotal << "\n";
    }
}