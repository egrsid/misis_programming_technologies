package main

import (
	"encoding/json"
	"os"
)

type Order string

type Input struct {
	Left  [][3]int64 `json:"left"`
	Right [][3]int64 `json:"right"`
}
type Result struct {
	Sum   [3]int64 `json:"sum"`
	Order Order    `json:"order"`
	Fits  bool     `json:"fits"`
}

func solve(in Input) []Result {
	// TODO: Реализуйте Resources, сложение и частичный порядок; unordered не равен equal.
	return []Result{}
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
