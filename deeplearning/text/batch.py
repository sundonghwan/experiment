import torch

def make_batch(text, char_to_idx, seq_len, batch_size):
    xs, ys = [], []
    for _ in range(batch_size):
        i = torch.randint(0, len(text) - seq_len - 1, (1,))
        chunk = text[i:i+seq_len+1]
        xs.append([char_to_idx[c] for c in chunk[:-1]])
        ys.append([char_to_idx[c] for c in chunk[1:]])
    return torch.tensor(xs), torch.tensor(ys)