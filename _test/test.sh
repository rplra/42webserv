#!/bin/bash

# --------------------------------------------------------------
# :: Readme ::
# Start up the webserver, then run this script to test
# --------------------------------------------------------------

# Configuration
PORT=8081

# Function to send HTTP request via netcat
send_request() {
    echo -en "$1" | nc -w 1 localhost $PORT
}

# echo "TEST 1   : Test Virtual Host (setup post 8081)"
# echo "EXPECTED : Page loading successfully"
# echo "File     : virtual.conf"
# echo "---------------------------------------------------------"
# curl --resolve www.default.com:8000:127.0.0.1 http://www.default.com:8000/
# echo ""

# echo "TEST 2   : Overflow Body Cap (max 2000 bytes, need to setup 8081 in config)"
# echo "EXPECTED : 413 Payload Too Large"
# echo "File     : basic.conf"
# echo "---------------------------------------------------------"
# curl -X POST -H "Content-Type: text/plain" --data "$(printf '%2001s' | tr ' ' 'A')" http://localhost:8081/upload
# echo ""

# echo "TEST 3   : POST success"
# echo "EXPECTED : 200 OK"
# echo "File     : basic.conf"
# echo "---------------------------------------------------------"
# # curl -X POST -H "Content-Type: text/plain" --data "$(printf '%2001s' | tr ' ' 'A')" http://localhost:8081/form
# # curl -v -X POST -H "Content-Type: text/plain" -d "hello" http://localhost:8081/form
# curl -s -i -X POST -H "Content-Type: text/plain" -d "hello" http://localhost:8081/form
# # curl -X POST http://localhost:8081/form -d "hello whale"
# echo ""

# echo "TEST 3   : Method not Allowed (curl)"
# echo "EXPECTED : 405 Method not Allowed"
# echo "File     : basic.conf"
# echo "---------------------------------------------------------"
# curl -s -i -X POST -H "Content-Type: text/plain" --data "$(printf '%20s' | tr ' ' 'A')" http://localhost:8081/images
# echo ""

# echo "TEST 4   : Method not Allowed (netcat)"
# echo "EXPECTED : 405 Method not Allowed"
# echo "File     : basic.conf"
# echo "----------------------------------------"
# send_request "POST /images HTTP/1.1\r\nHost: localhost:8081\r\nContent-Length: 11\r\n\r\nHELLO WORLD"
# echo ""

# echo "TEST 5   : POST request (netcat)"
# echo "EXPECTED : Show HELLO WORLD"
# echo "File     : basic.conf"
# echo "----------------------------------------"
# send_request "POST /form HTTP/1.1\r\nHost: localhost\r\nContent-Length: 11\r\n\r\nHELLO WORLD"
# echo ""

# echo "TEST 6   : CGI POST request, (upload, curl)"
# echo "EXPECTED : File uploaded"
# echo "File     : basic.conf"
# echo "----------------------------------------"
# echo fufu > upload_test.txt
# curl -s -i -X POST http://localhost:8081/cgi-bin/upload_basic.py -F "file=@upload_test.txt"
# echo ""

# echo "TEST 7   : DELETE request using path, (curl)"
# echo "           (need to change allowed_methods in /del_test to DELETE)"
# echo "EXPECTED : File deleted"
# echo "File     : basic.conf"
# echo "----------------------------------------"
# # curl -s -i -X DELETE http://localhost:8081/del_test/bl/upload_test.txt #404
# # curl -s -i -X DELETE http://localhost:8081/del_test/ #403
# curl -s -i -X DELETE http://localhost:8081/del_test/upload_test.txt #200
# echo ""

# echo "TEST 8   : DELETE request using query, (curl)"
# echo "           (need to change allowed_methods in /del_test to DELETE)"
# echo "EXPECTED : File deleted"
# echo "File     : basic.conf"
# echo "----------------------------------------"
# # curl -s -i -X DELETE http://localhost:8081/del_test?file=u.txt #404
# # curl -s -i -X DELETE http://localhost:8081/del_test?fil=upload_test.txt #400
# # curl -s -i -X DELETE http://localhost:8081/del_test?file= #400
# # curl -s -i -X DELETE http://localhost:8081/del_test? #403
# curl -s -i -X DELETE http://localhost:8081/del_test?file=upload_test.txt #200
# echo ""

# echo "TEST 9   : CGI DELETE request using query, (curl)"
# echo "           (need to change allowed_methods in /cgi-bin to DELETE)"
# echo "EXPECTED : File deleted"
# echo "File     : basic.conf"
# echo "----------------------------------------"
# curl -X DELETE http://localhost:8081/cgi-bin/delete_basic.py?file=upload_test.txt
# echo ""
