import torch
from torch.utils.data import Dataset
from auxiliary_func import create_nn_input


class ChessDataset(Dataset):
    def __init__(self, games, move_to_int, transform=None):
        self.games = games
        self.move_to_int = move_to_int
        self.transform = transform

    def __len__(self):
        return len(self.games)

    def __getitem__(self, idx):
        X, y = create_nn_input([self.games[idx]])
        encoded_y = [self.move_to_int[move] for move in y]
        return torch.tensor(X, dtype=torch.float32), torch.tensor(encoded_y, dtype=torch.long)
