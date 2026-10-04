package main

import (
	"encoding/json"
	"os"
)

type Input struct {
	Initial int64   `json:"initial"`
	Ops     string  `json:"ops"`
	IDs     []int64 `json:"ids"`
	Counts  []int64 `json:"counts"`
}
type Output struct {
	Results   string     `json:"results"`
	Snapshots [][3]int64 `json:"snapshots"`
}

func solve(in Input) Output {
	// TODO: Реализуйте Warehouse и его методы; состояние меняется только внутри объекта.
	return Output{Results: "", Snapshots: make([][3]int64, 0)}
}

func main() {
	var in Input
	if err := json.NewDecoder(os.Stdin).Decode(&in); err != nil {
		panic(err)
	}
	if err := json.NewEncoder(os.Stdout).Encode(solve(in)); err != nil {
		panic(err)
	}
}
