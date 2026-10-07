package main

import (
	"encoding/json"
	"os"
)

type Input struct {
	Prices     []int64 `json:"prices"`
	Rules      string  `json:"rules"`
	Amounts    []int64 `json:"amounts"`
	Thresholds []int64 `json:"thresholds"`
}

func solve(in Input) []int64 {
	// TODO: Создайте три типа Discount с apply и общий конвейер без ветвления по типам.
	return []int64{}
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
