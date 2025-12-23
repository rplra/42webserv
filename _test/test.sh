#!/bin/bash

# --------------------------------------------------------------
# :: Readme ::
# Start up the webserver, then run this script to test
# --------------------------------------------------------------

# Configuration
PORT=8080

# Function to send HTTP request via netcat
send_request() {
    echo -en "$1" | nc -w 1 localhost $PORT
}

# echo "TEST_1: Test different Hostnames (setup post 8080)"
# echo "EXPECTED: Page loading successfully"
# echo "---------------------------------------------------------"
# curl --resolve webserv.example.com:8080:127.0.0.1 http://webserv.example.com/
# echo ""

echo "TEST_2: Overflow Body Cap (max 2000 bytes, need to setup 8082 in config)."
echo "EXPECTED: 413 Payload Too Large"
echo "---------------------------------------------------------"
curl -X POST -H "Content-Type: text/plain" --data "$(printf '%2001s' | tr ' ' 'A')" http://127.0.0.1:8082/
echo ""
