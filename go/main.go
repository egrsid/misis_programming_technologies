package main

import (
	"bufio"
	"encoding/json"
	"os"
)

type Purchase struct {
	Price       float64 `json:"price"`
	Description string  `json:"description"`
}

func classify(p Purchase) string {
	// TODO: классы electronics/groceries/clothing/transport/other.
	return "other"
}

func main() {
	line, _ := bufio.NewReader(os.Stdin).ReadString('\n')
	var p Purchase
	_ = json.Unmarshal([]byte(line), &p)
	out, _ := json.Marshal(classify(p))
	os.Stdout.Write(out) // одна JSON-строка
}
