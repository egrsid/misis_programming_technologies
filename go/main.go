package main

import (
	"bufio"
	"encoding/json"
	"os"
)

type input struct {
	FailOpen  []string   `json:"fail_open"`
	FailClose []string   `json:"fail_close"`
	FailWork  []string   `json:"fail_work"`
	Ops       [][]string `json:"ops"`
}

// TODO: run ops, return the event log ending with "result ...".
// Use defer, error and an interface.
func solve(in input) []string {
	_ = in
	return nil
}

func main() {
	var in input
	if err := json.NewDecoder(bufio.NewReader(os.Stdin)).Decode(&in); err != nil {
		panic(err)
	}

	out, _ := json.Marshal(solve(in))
	os.Stdout.Write(out)
}
