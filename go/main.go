package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"os"
)

type input struct {
	Container string `json:"container"`
	Workload  string `json:"workload"`
	N         uint64 `json:"n"`
	Seed      uint64 `json:"seed"`
}

// ---------- Генератор splitmix64 (общий для всех языков) ----------
type rng struct{ state uint64 }

func (r *rng) next() uint64 {
	r.state += 0x9E3779B97F4A7C15
	z := r.state
	z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9
	z = (z ^ (z >> 27)) * 0x94D049BB133111EB
	return z ^ (z >> 31)
}

func (r *rng) rnd() uint64 { return r.next() % 1_000_000 }

// ---------- Решение ----------

// TODO: container ∈ {array, linked, deque} → слайс / container/list / СВОЙ кольцевой
// буфер с ростом ×2; workload ∈ {push_back, push_front, queue, sorted_insert} — см. README.
// checksum = Σ (i + 1) · a[i] mod 1 000 000 007.
func run(container, workload string, n, seed uint64) (int, uint64) {
	r := rng{seed}
	_, _, _ = container, workload, r
	return int(n), 0
}

func main() {
	var in input
	if err := json.NewDecoder(bufio.NewReader(os.Stdin)).Decode(&in); err != nil {
		panic(err)
	}
	n, sum := run(in.Container, in.Workload, in.N, in.Seed)
	fmt.Printf(`{"len": %d, "checksum": %d}`, n, sum)
}
