#!/usr/bin/python3

import os
import time

# # Required header
print("Status: 200")
print("Content-Type: text/html")
print() # This blank line is CRITICAL

# Body
print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>Hello from Python CGI!</h1>")
print("</body>")
print("</html>")
