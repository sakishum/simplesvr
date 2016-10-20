package api

import "testing"

func TestDemo(t *testing.T) {
	args := "Hello World!"
	server := ":29506"
	retStr, err := Demo(args, server)
	if err != nil {
		t.Fatal(err)
	}
	t.Log("retStr:", retStr)
}
