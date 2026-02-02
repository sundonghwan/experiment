import torch.nn as nn
import torch

class TransformerBlock(nn.Module):
    def __init__(self, d_model, n_heads):
        super().__init__()
        self.ln1 = nn.LayerNorm(d_model)
        self.attn = nn.MultiheadAttention(
            d_model, n_heads, batch_first=True
        )
        self.ln2 = nn.LayerNorm(d_model)
        self.ffn = nn.Sequential(
            nn.Linear(d_model, 4 * d_model),
            nn.ReLU(),
            nn.Linear(4 * d_model, d_model)
        )

    def forward(self, x):
        # Self-Attention
        h = self.ln1(x)
        attn_out, _ = self.attn(h, h, h)
        x = x + attn_out  # residual

        # FFN
        h = self.ln2(x)
        x = x + self.ffn(h)

        return x

class TinyTransformer(nn.Module):
    def __init__(self, vocab_size, d_model=64, n_heads=4, n_layers=2):
        super().__init__()
        self.embed = nn.Embedding(vocab_size, d_model)
        self.blocks = nn.ModuleList([
            TransformerBlock(d_model, n_heads)
            for _ in range(n_layers)
        ])
        self.ln_f = nn.LayerNorm(d_model)
        self.head = nn.Linear(d_model, vocab_size)

    def forward(self, x):
        x = self.embed(x)

        for i, block in enumerate(self.blocks):
            x = block(x)
            if not torch.isfinite(x).all():
                print("first non-finite at block", i)
                break

        # x = self.ln_f(x)
        logits = self.head(x)
        return logits