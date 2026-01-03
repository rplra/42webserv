import os
import urllib.parse

effective_method = os.environ.get("REQUEST_METHOD", "")
query = os.environ.get("QUERY_STRING", "")

status = "200 OK"
message = ""

# Determine effective method
# effective_method = method
# if method == "POST" and form.getvalue("_method") == "DELETE":
    # effective_method = "DELETE"

# Enforce method
if effective_method != "DELETE":
    status = "405 Method Not Allowed"
    message = "Method Not Allowed"

else:
    # Get filename (from query or form)
    filename = (
        urllib.parse.parse_qs(query).get("file", [None])[0]
    )

    if not filename:
        message = "No file specified"
    else:
        # sanitize
        filename = urllib.parse.unquote(filename)
        path = os.path.join("www/uploads", os.path.basename(filename))

        if os.path.exists(path):
            os.remove(path)
            message = "File deleted"
        else:
            status = "404 Not Found"
            message = "File not found"

# print cgi header -----------------------------
print(f"Status: {status}")
print("Content-Type: text/html")
print() # This blank line is CRITICAL

print(f"<html><body><h1>{message}</h1>")
print('</body></html>')