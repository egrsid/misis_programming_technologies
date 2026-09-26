package main

import (
	"bufio"
	"encoding/json"
	"os"
	"strings"
)

type input struct {
	Ops   string     `json:"ops"`
	Names []string   `json:"names"`
	Tags  [][]uint32 `json:"tags"`
}

// TODO: встроенный map не принимает свою хэш-функцию, поэтому соберите хэш-таблицу
// сами: map[uint64][]item, где ключ — ваш хэш товара, а внутри корзины товары
// сравниваются вашей функцией equal. Склеивать товар в строку-ключ нельзя.
// Название сравнивается без учёта регистра, теги — как множество.
func solve(ops string, names []string, tags [][]uint32) string {
	_, _ = names, tags
	return strings.Repeat("-", len(ops))
}

func main() {
	var in input
	if err := json.NewDecoder(bufio.NewReader(os.Stdin)).Decode(&in); err != nil {
		panic(err)
	}
	res, _ := json.Marshal(solve(in.Ops, in.Names, in.Tags))
	os.Stdout.Write(res)
}
