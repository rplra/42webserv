#!/bin/bash

# --------------------------------------------------------------
# :: Readme ::
# Start up the webserver, then run this script to test
# --------------------------------------------------------------

# Colours
GREEN="\033[38;2;168;204;124m"
RED="\033[38;2;191;97;106m"
ORANGE="\033[38;2;255;135;0m";
RESET="\033[0m"

# virtual.conf
PORT=8000

# Function to run virtual host test
# $1 = hostname
# $2 = expected body 
run_virtual_test() {
    local host=$1
    local expected=$2
    local test_name=$3

    echo -e "${ORANGE}${test_name}${RESET}"
	echo "---------------------------------------------------------"
    echo -e "EXPECTED: $expected"

    output=$(curl -s --resolve "$host:$PORT:127.0.0.1" "http://$host:$PORT/")
    echo "RESULT  : $output"

    if [[ "$output" == "$expected" ]]; then
        echo -e "${GREEN}OK!${RESET}"
    else
        echo -e "${RED}ERROR!${RESET}"
        echo "GOT: $output"
    fi
    echo ""
}

# Run all tests
run_virtual_test default "<h1>Default</h1>" "TEST 1  : Test virtual host (default)"
run_virtual_test www.default.com "<h1>Default</h1>" "TEST 2  : Test virtual host (www.default.com)"
run_virtual_test example "<h1>Example</h1>" "TEST 3  : Test virtual host (example)"
run_virtual_test www.example.com "<h1>Example</h1>" "TEST 4  : Test virtual host (www.example.com)"
run_virtual_test test "<h1>Test</h1>" "TEST 5  : Test virtual host (test)"
run_virtual_test www.test.com "<h1>Test</h1>" "TEST 6  : Test virtual host (www.test.com)"


# manual test
# RUN ./webserv conf/virtual.conf

# curl --resolve example:8000:127.0.0.1 http://example:8000/
# curl --resolve www.example.com:8000:127.0.0.1 http://www.example.com:8000/

# curl --resolve test:8000:127.0.0.1 http://test:8000/
# curl --resolve www.test.com:8000:127.0.0.1 http://www.test.com:8000/

# curl --resolve default:8000:127.0.0.1 http://default:8000/
# curl --resolve www.default.com:8000:127.0.0.1 http://www.default.com:8000/
