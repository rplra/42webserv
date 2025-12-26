#!/bin/bash

# --------------------------------------------------------------
# :: Readme ::
# Start up the webserver, then run this script to test
# --------------------------------------------------------------

# Configuration
PORT=8082

# Function to send HTTP request via netcat
send_request() {
    echo -en "$1" | nc -w 1 localhost $PORT
}

# echo "TEST 1  : Test different Hostnames (setup post 8080)"
# echo "EXPECTED: Page loading successfully"
# echo "---------------------------------------------------------"
# curl --resolve webserv.example.com:8080:127.0.0.1 http://webserv.example.com/
# echo ""

# echo "TEST 2  : Overflow Body Cap (max 2000 bytes, need to setup 8082 in config)"
# echo "EXPECTED: 413 Payload Too Large"
# echo "---------------------------------------------------------"
# curl -X POST -H "Content-Type: text/plain" --data "$(printf '%2001s' | tr ' ' 'A')" http://127.0.0.1:8082/
# echo ""

# echo "TEST 3  : Method not Allowed (curl)"
# echo "EXPECTED: 405 Method not Allowed"
# echo "---------------------------------------------------------"
# curl -X POST -H "Content-Type: text/plain" --data "$(printf '%20s' | tr ' ' 'A')" http://127.0.0.1:8082/images
# echo ""

echo "TEST 4  : Method not Allowed (netcat)"
echo "EXPECTED: 405 Method not Allowed"
echo "----------------------------------------"
send_request "POST /images HTTP/1.1\r\nHost: 127.0.0.1:8082\r\nContent-Length: 11\r\n\r\nHELLO WORLD"
echo ""

# echo "TEST 5  : Send POST request (netcat)"
# echo "EXPECTED: Show HELLO WORLD"
# echo "----------------------------------------"
# send_request "POST /test HTTP/1.1\r\nHost: localhost\r\nContent-Length: 11\r\n\r\nHELLO WORLD"
# echo ""