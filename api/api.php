<?php

class Demo 
{
    private $ip;
    private $port;

    public function __construct($ip, $port) {
        $this->ip = $ip;
        $this->port = $port;
    }   

    public function proc($args, &$retstr) {
        $sock = socket_create(AF_INET, SOCK_DGRAM, 0); 
        if (!$sock) {
            return -1; 
        }   
        socket_set_option($sock, SOL_SOCKET, SO_RCVTIMEO, array("sec"=>0, "usec"=>80000));

        $cmd = "Demo";
        $msg = sprintf("%s\x00%s", $cmd, $args);
        socket_sendto($sock, $msg, strlen($msg), 0, $this->ip, $this->port);
        $ret = socket_recv($sock, $buf, 256, 0); 
        socket_close($sock);
        if (!$ret) {
            return -2; 
        }   

        $retcode = ord($buf[0]);
        $retstr = substr($buf, 1); 
        if ($retcode != 0) {
            fprintf(STDERR, "Demo proc() code: %d, msg: %s\n", $retcode, $retstr);
            return -3; 
        }   

        return 0;
    }   
}

$ip = "127.0.0.1";
$port = 29506;
$obj = new Demo($ip, $port);

$args = "test msg";
$retstr = ""; 

$ret = $obj->proc($args, $retstr);
if ($ret != 0) {
    printf("proc() error: %d\n", $ret);
    exit(-1);
}

printf("retstr: %s\n", $retstr);

