import os
import urllib.parse
import cgi

method = os.environ.get("REQUEST_METHOD", "")
query = os.environ.get("QUERY_STRING", "")

form = cgi.FieldStorage()

# Determine effective method
effective_method = method
if method == "POST" and form.getvalue("_method") == "DELETE":
    effective_method = "DELETE"

# Enforce method
if effective_method != "DELETE":
    print("Status: 405 Method Not Allowed")
    print("<h1>Method Not Allowed</h1>")
    exit()

# Get filename (from query or form)
filename = (
    form.getvalue("file")
    or urllib.parse.parse_qs(query).get("file", [None])[0]
)

if not filename:
    print("<h1>No file specified</h1>")
    exit()

filename = urllib.parse.unquote(filename)
path = os.path.join("www/uploads", os.path.basename(filename))

if os.path.exists(path):
    os.remove(path)
    print("<h1>File deleted</h1>")
else:
    print("<h1>File not found</h1>")

print('<a href="/cgi-bin/uploads.py">Back</a>')
