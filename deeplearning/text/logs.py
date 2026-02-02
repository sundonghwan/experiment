import torch

def log_tensor(name, t):
    print(
        f"{name:20s} | "
        f"mean={t.mean():+.3e} "
        f"std={t.std():+.3e} "
        f"max={t.max():+.3e} "
        f"nan={torch.isnan(t).any().item()}"
    )
    
def grad_norm(model):
    total = 0.0
    for p in model.parameters():
        if p.grad is None: 
            continue
        total += p.grad.detach().float().pow(2).sum().item()
    return (total ** 0.5)
