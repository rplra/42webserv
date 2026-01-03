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

# Function to run method test
# $1 = HTTP method (GET, POST, DELETE)
# $2 = path
# $3 = expected status code
# $4 = optional body (for POST/PUT)
run_method_test() {
    local method=$1
    local path=$2
    local expected=$3
    local body=$4
    local test_name="${method} ${path}"

    echo -e "${ORANGE}TEST: $test_name${RESET}"
	echo "---------------------------------------------------------"
	echo -e "EXPECTED: $expected"

    # Construct curl command
    if [[ "$method" == "POST" || "$method" == "PUT" ]]; then
        response=$(curl -s -i -X "$method" -d "$body" "http://localhost:$PORT$path")
    else
        response=$(curl -s -i -X "$method" "http://localhost:$PORT$path")
    fi

    # Extract HTTP status code
    status_code=$(echo "$response" | head -n1 | awk '{print $2}')

    echo "RESULT  : $status_code"

    if [[ "$status_code" == "$expected" ]]; then
        echo -e "${GREEN}OK!${RESET}"
    else
        echo -e "${RED}ERROR!${RESET}"
        echo "GOT RESPONSE:"
        echo "$response"
    fi
    echo ""
}

# Run all tests based on basic.conf
# / -> GET only
run_method_test GET "/" 200
run_method_test POST "/" 405
run_method_test DELETE "/" 405

# /form/ -> GET, POST
run_method_test GET "/form/" 200
run_method_test POST "/form/" 200 "HELLO WORLD"
run_method_test DELETE "/form/" 405

# /images/ -> GET only
run_method_test GET "/images/" 200
run_method_test POST "/images/" 405
run_method_test DELETE "/images/" 405

# /upload/ -> GET, POST (client_max_body_size 30 bytes)
run_method_test GET "/upload/" 200
run_method_test POST "/upload/" 413 "$(printf 'A%.0s' {1..50})"
run_method_test DELETE "/upload/" 405

# /uploads/ -> GET only
run_method_test GET "/uploads/" 403
run_method_test POST "/uploads/" 405
run_method_test DELETE "/uploads/" 405

# /fruits/ -> GET only
run_method_test GET "/fruits/" 200
run_method_test POST "/fruits/" 405
run_method_test DELETE "/fruits/" 405

# /42 -> redirect
run_method_test GET "/42" 302

# /cgi-bin/upload_basic.py -> GET, POST, DELETE
run_method_test GET "/cgi-bin/upload_basic.py" 200
run_method_test POST "/cgi-bin/upload_basic.py" 200 "print('Hello from upload_basic.py')"
run_method_test DELETE "/cgi-bin/upload_basic.py" 200

# /cgi-bin/fruits.py -> GET, POST, DELETE
run_method_test GET "/cgi-bin/fruits.py" 200
run_method_test POST "/cgi-bin/fruits.py" 200 "print('Hello from fruits.py')"
run_method_test DELETE "/cgi-bin/fruits.py" 500

# unkwown/invalid method
run_method_test PUT "/" 400
run_method_test PATCH "/" 400
run_method_test OPTIONS "/" 400
run_method_test TRACE "/" 400
run_method_test CONNECT "/" 400
run_method_test ABCD "/form/" 400