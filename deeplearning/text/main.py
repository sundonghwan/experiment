import torch
from model.tiny import TinyTransformer
from batch import make_batch
from logs import log_tensor, grad_norm
from torch import nn
text = (
    "In publishing and graphic design, Lorem ipsum is a placeholder text commonly "
    "used to demonstrate the visual form of a document or a typeface without relying "
    "on meaningful content. Lorem ipsum may be used as a placeholder before the "
    "actual content is available."
)
chars = sorted(list(set(text)))
vocab_size = len(chars)
char_to_idx = {ch: i for i, ch in enumerate(chars)}


model = TinyTransformer(vocab_size)
opt = torch.optim.SGD(model.parameters(), lr=3e-3)
loss_fn = nn.CrossEntropyLoss()

for step in range(1000000):
    x, y = make_batch(text, char_to_idx, 32, 16)
    logits = model(x)

    loss = loss_fn(
        logits.view(-1, vocab_size),
        y.view(-1)
    )

    opt.zero_grad()
    loss.backward()
    opt.step()

    if step % 20 == 0:
        print(f"\nstep {step} | loss {loss.item():.4f}")
        log_tensor("logits", logits)
        log_tensor("embed.weight", model.embed.weight)
        print(f"grad norm: {grad_norm(model):.3e}")