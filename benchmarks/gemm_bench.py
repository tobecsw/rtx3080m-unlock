import torch, statistics, sys

torch.backends.cuda.matmul.allow_tf32 = False
torch.backends.cudnn.allow_tf32 = False

N = int(sys.argv[1]) if len(sys.argv) > 1 else 8192
ITERS = 20
WARMUP = 5

def bench(dtype, tf32=False):
    torch.backends.cuda.matmul.allow_tf32 = tf32
    a = torch.randn(N, N, device="cuda", dtype=dtype)
    b = torch.randn(N, N, device="cuda", dtype=dtype)
    for _ in range(WARMUP):
        c = a @ b
    torch.cuda.synchronize()
    times = []
    for _ in range(ITERS):
        s = torch.cuda.Event(enable_timing=True)
        e = torch.cuda.Event(enable_timing=True)
        s.record()
        c = a @ b
        e.record()
        torch.cuda.synchronize()
        times.append(s.elapsed_time(e) / 1000.0)
    med = statistics.median(times)
    tflops = 2.0 * N * N * N / med / 1e12
    return tflops, med

print(f"GPU: {torch.cuda.get_device_name(0)}")
print(f"GEMM {N}x{N}, median of {ITERS} iters\n")
rows = []
for label, dtype, tf32 in [
    ("FP32 (no tensor)", torch.float32, False),
    ("TF32 (tensor)",    torch.float32, True),
    ("FP16 (tensor)",    torch.float16, False),
    ("BF16 (tensor)",    torch.bfloat16, False),
]:
    t, ms = bench(dtype, tf32)
    rows.append((label, t, ms))
    print(f"{label:<18} {t:8.2f} TFLOPS   ({ms*1000:7.2f} ms)")

# baseline FP32 for relative
base = rows[0][1]
print("\nRelative to FP32:")
for label, t, ms in rows:
    print(f"{label:<18} {t/base:6.2f}x")
