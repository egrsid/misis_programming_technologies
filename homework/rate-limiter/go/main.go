package main

import (
	"bufio"
	"encoding/json"
	"os"
	"strings"
)

type input struct {
	Rules [][2]int64 `json:"rules"`
	Users []int64    `json:"users"`
	Times []int64    `json:"times"`
}

// TODO: для каждого запроса вернуть 'A' (пропустить) или 'D' (отклонить).
//
// Ограничение для Go: без container/list. Очередь — на слайсе и без утечки памяти;
// объясните в комментарии, почему `q = q[1:]` в цикле держит в памяти весь старый массив.
func solve(rules [][2]int64, users, times []int64) string {
	_, _ = rules, times
	return strings.Repeat("A", len(users))
}

func main() {
	var in input
	if err := json.NewDecoder(bufio.NewReader(os.Stdin)).Decode(&in); err != nil {
		panic(err)
	}
	res, _ := json.Marshal(solve(in.Rules, in.Users, in.Times))
	os.Stdout.Write(res)
}
