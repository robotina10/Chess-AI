import numpy as np
import torch
from chess import Board


def board_to_matrix(board: Board):
    # 8x8 is a size of the chess board.
    # 12 = number of unique pieces.
    # 13th board for legal moves (WHERE we can move)
    # maybe 14th for squares FROM WHICH we can move? idk
    matrix = np.zeros((13, 8, 8))
    piece_map = board.piece_map()

    # Populate first 12 8x8 boards (where pieces are)
    for square, piece in piece_map.items():
        row, col = divmod(square, 8)
        piece_type = piece.piece_type - 1
        piece_color = 0 if piece.color else 6
        matrix[piece_type + piece_color, row, col] = 1

    # Populate the legal moves board (13th 8x8 board)
    legal_moves = board.legal_moves
    for move in legal_moves:
        to_square = move.to_square
        row_to, col_to = divmod(to_square, 8)
        matrix[12, row_to, col_to] = 1

    return matrix


def board_to_tensor(board):
    """
    Converts a chess.Board object to a PyTorch tensor.
    :param board: A chess.Board object.
    :return: A PyTorch tensor of shape (13, 8, 8).
    """
    matrix = board_to_matrix(board)
    return torch.tensor(matrix, dtype=torch.float32)


def extract_board_and_move(game):
    """
    Extracts board states and moves from a game.
    :param game: A chess.pgn.Game object.
    :return: A list of (board, move) tuples.
    """
    board = game.board()
    board_move_pairs = []
    for move in game.mainline_moves():
        board_move_pairs.append((board.copy(), move))
        board.push(move)
    return board_move_pairs


def create_nn_input(games):
    """
    Creates input features and labels for the neural network.
    :param games: A list of chess.pgn.Game objects.
    :return: A tuple (X, encoded_y, move_to_int).
    """
    X = []
    y = []
    all_moves = []
    for game in games:
        for board, move in extract_board_and_move(game):
            X.append(board_to_tensor(board))  # Convert board to tensor
            y.append(move)
            all_moves.append(move)
    move_to_int = {move: idx for idx, move in enumerate(set(all_moves))}
    encoded_y = [move_to_int[move] for move in y]
    return X, encoded_y, move_to_int


def encode_moves(moves):
    move_to_int = {move: idx for idx, move in enumerate(set(moves))}
    return np.array([move_to_int[move] for move in moves], dtype=np.float32), move_to_int