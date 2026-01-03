#!/bin/bash

# --------------------------------------------------------------
# :: Readme ::
# Start up the webserver, then run this script to test
# --------------------------------------------------------------

# basic.conf
PORT=8081

# Colours
GREEN="\033[38;2;168;204;124m"
RED="\033[38;2;191;97;106m"
ORANGE="\033[38;2;255;135;0m";
RESET="\033[0m"

# function to run POST with a body of given size
# $1 = test name
# $2 = url
# $3 = body size (bytes)
# $4 = expected status
run_limit_test() {
    local name="$1"
    local url="$2"
    local size="$3"
    local expected="$4"

    head -c "$size" /dev/zero > body.tmp

    status=$(curl -s -o /dev/null -w "%{http_code}" \
        -X POST \
        --data-binary @body.tmp \
        "$url")

    if [ "$status" = "$expected" ]; then
        echo -e "${GREEN}OK!${RESET}      : $name -> $status"
    else
        echo -e "${RED}ERROR!${RESET}   : $name"
        echo "EXPECTED : $expected"
        echo "GOT      : $status"
    fi

    rm -f body.tmp
}

# server-level client_max_body_size = 30000000
echo ""
echo -e "${ORANGE}SERVER SCOPE (30 MB)${RESET}"
echo "---------------------------------------------------------"
run_limit_test "POST / (1 MB)" http://localhost:$PORT/ 1024 405
run_limit_test "POST / (31 MB)" http://localhost:$PORT/ 31000000 413
run_limit_test "POST /form (31 MB)" http://localhost:$PORT/form 31000000 413

# location /upload client_max_body_size = 30
echo ""
echo -e "${ORANGE}LOCATION /upload/ (30 bytes)${RESET}"
echo "---------------------------------------------------------"
run_limit_test "POST /upload (10 bytes)" http://localhost:$PORT/upload/ 10 200
run_limit_test "POST /upload/ (31 bytes)" http://localhost:$PORT/upload/ 31 413

# location /cgi-bin client_max_body_size = 30
echo ""
echo -e "${ORANGE}LOCATION /cgi-bin (30 bytes)${RESET}"
echo "---------------------------------------------------------"
run_limit_test "POST CGI (10 bytes)" http://localhost:$PORT/cgi-bin/upload.py 10 200
run_limit_test "POST CGI (31 bytes)" http://localhost:$PORT/cgi-bin/upload.py 31 413
