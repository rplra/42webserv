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

# echo "TEST 1  : Test Virtual Host (setup post 8080)"
# echo "EXPECTED: Page loading successfully"
# echo "---------------------------------------------------------"
# curl --resolve webserv.example.com:8080:127.0.0.1 http://webserv.example.com:8080/
# echo ""

# echo "TEST 2  : Overflow Body Cap (max 2000 bytes, need to setup 8082 in config)"
# echo "EXPECTED: 413 Payload Too Large"
# echo "---------------------------------------------------------"
# curl -X POST -H "Content-Type: text/plain" --data "$(printf '%2001s' | tr ' ' 'A')" http://localhost:$PORT/
# echo ""

# echo "TEST 3  : Method not Allowed (curl)"
# echo "EXPECTED: 405 Method not Allowed"
# echo "---------------------------------------------------------"
# curl -X POST -H "Content-Type: text/plain" --data "$(printf '%20s' | tr ' ' 'A')" http://localhost:8082/images
# echo ""

# echo "TEST 4  : Method not Allowed (netcat)"
# echo "EXPECTED: 405 Method not Allowed"
# echo "----------------------------------------"
# send_request "POST /images HTTP/1.1\r\nHost: localhost:8082\r\nContent-Length: 11\r\n\r\nHELLO WORLD"
# echo ""

# echo "TEST 5  : POST request (netcat)"
# echo "EXPECTED: Show HELLO WORLD"
# echo "----------------------------------------"
# send_request "POST /large HTTP/1.1\r\nHost: localhost\r\nContent-Length: 11\r\n\r\nHELLO WORLD"
# echo ""

echo "TEST 6   : POST request, (upload file, curl)"
echo "EXPECTED : File uploaded"
echo "File     : basic.conf"
echo "----------------------------------------"
echo fufu > upload_test.txt
curl -X POST http://localhost:8081/cgi-bin/upload_basic.py -F "file=@upload_test.txt"
echo ""

# echo "TEST 7   : DELETE request, (curl)"
# echo "(need to change allowed_methods in /uploads to DELETE)"
# echo "EXPECTED : File deleted"
# echo "File     : basic.conf"
# echo "----------------------------------------"
# curl -X DELETE http://localhost:8081/uploads/upload_test.txt
# echo ""

# echo "TEST 8   : CGI DELETE request, (curl)"
# echo "(need to change allowed_methods in /uploads to DELETE)"
# echo "EXPECTED : File deleted"
# echo "File     : basic.conf"
# echo "----------------------------------------"
# curl -X DELETE http://localhost:8081/cgi-bin/delete_basic.py?file=upload_test.txt
# echo ""
