import sys
import json

MASK = (1 << 64) - 1
P = 1_000_000_007


# ---------- Генератор splitmix64 (общий для всех языков) ----------
class Rng:
    __slots__ = ("state",)

    def __init__(self, seed: int) -> None:
        self.state = seed & MASK

    def next(self) -> int:
        self.state = (self.state + 0x9E3779B97F4A7C15) & MASK
        z = self.state
        z = ((z ^ (z >> 30)) * 0xBF58476D1CE4E5B9) & MASK
        z = ((z ^ (z >> 27)) * 0x94D049BB133111EB) & MASK
        return z ^ (z >> 31)

    def rnd(self) -> int:
        return self.next() % 1_000_000


# ---------- Решение ----------
def run(container: str, workload: str, n: int, seed: int) -> tuple[int, int]:
    # TODO: container ∈ {array, linked, deque} → list / СВОЙ двусвязный список на классе
    # с __slots__ / collections.deque; workload ∈ {push_back, push_front, queue,
    # sorted_insert} — см. README. checksum = Σ (i + 1) · a[i] mod 1 000 000 007.
    r = Rng(seed)
    return n, 0


def main() -> None:
    obj = json.loads(sys.stdin.read())
    length, checksum = run(obj["container"], obj["workload"], obj["n"], obj["seed"])
    sys.stdout.write(json.dumps({"len": length, "checksum": checksum}))


if __name__ == "__main__":
    main()
