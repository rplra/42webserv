import os
import urllib.parse
import cgi

method = os.environ.get("REQUEST_METHOD", "")
query = os.environ.get("QUERY_STRING", "")

form = cgi.FieldStorage()
status = "200 OK"
message = ""

# Determine effective method
effective_method = method
if method == "POST" and form.getvalue("_method") == "DELETE":
    effective_method = "DELETE"

# Enforce method
if effective_method != "DELETE":
    status = "405 Method Not Allowed"
    message = "Method Not Allowed"
    print(f"Status: {status}")
    print(f"<h1>{message}</h1>")
    exit()

# Get filename (from query or form)
filename = (
    form.getvalue("file")
    or urllib.parse.parse_qs(query).get("file", [None])[0]
)

if not filename:
    message = "No file specified"
    print(f"<h1>{message}</h1>")
    exit()

filename = urllib.parse.unquote(filename)
path = os.path.join("www/uploads", os.path.basename(filename))

if os.path.exists(path):
    os.remove(path)
    message = "File deleted"
else:
    message = "File not found"

print(f"<h1>{message}</h1>")
print('<a href="/cgi-bin/uploads.py">Back</a>')
