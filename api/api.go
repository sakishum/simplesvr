package api

import (
	"fmt"
	"net"
	"time"
)

/*
 * @param
 * server: ip(string)
 * @return
 * retcode:
 */
func Demo(args string, server string) (retStr string, err error) {
	conn, err := net.Dial("udp", server)
	if err != nil {
		return
	}
	defer conn.Close()

	// encode
	cmd := "Demo"
	data := []byte(fmt.Sprintf("%s\x00%s", cmd, args))
	conn.SetWriteDeadline(time.Now().Add(time.Millisecond * 5))
	_, err = conn.Write(data)
	if err != nil {
		err = nil
		return
	}

	result := make([]byte, 256)
	conn.SetReadDeadline(time.Now().Add(time.Millisecond * 80))
	readLen, err := conn.Read(result)
	if err != nil || readLen <= 0 {
		err = fmt.Errorf("Demo() ReadTimeout %d, %v", readLen, err)
		return
	}

	// decode
	if result[0] != 0 {
		// error
		err = fmt.Errorf("Demo() code: %v, msg: %v", result[0], result[1:readLen])
		return
	}

	retStr = string(result[1:readLen])
	return
}
