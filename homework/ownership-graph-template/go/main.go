package main

import (
	"bufio"
	"encoding/json"
	"os"
)

type input struct {
	Vars int     `json:"vars"`
	Ops  [][]any `json:"ops"`
}

// TODO: run ops, return the event log ("d<id>", "u<n>", "leak<id>").
// Count references by hand; free fields without recursion.
func solve(nvars int, ops [][]any) []string {
	_, _ = nvars, ops
	return nil
}

func main() {
	var in input
	if err := json.NewDecoder(bufio.NewReader(os.Stdin)).Decode(&in); err != nil {
		panic(err)
	}

	res := solve(in.Vars, in.Ops)

	if res == nil {
		res = []string{}
	}

	out, _ := json.Marshal(res)
	os.Stdout.Write(out)
}
