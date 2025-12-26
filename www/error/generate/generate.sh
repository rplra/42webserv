#!/bin/bash

./a.out 400 'Bad Request' 'The server could not understand your request due to invalid syntax'
./a.out 403 'Forbidden' "You don't have permission to access this resource"
./a.out 404 'Not Found' 'The page you requested could not be found'
./a.out 405 'Method Not Allowed' 'The HTTP method is not supported for this resource'
./a.out 411 'Length Required' 'This request did not include a Content-Length header, which is required'
./a.out 413 'Payload Too Large' 'The request is larger than the server can process'
./a.out 500 'Internal Server Error' 'The server encountered an unexpected condition'
./a.out 502 'Bad Gateway' 'The server received an invalid response from the upstream server'
./a.out 503 'Service Unavailable' 'The server is temporarily unable to handle the request'