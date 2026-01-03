import os
import urllib.parse
import cgi
import json
from   http.cookies import SimpleCookie
from   print_html import print_html

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
    message = "Method Not Allowed ༼ ༎ຶ ෴ ༎ຶ༽"

else:
    # Get filename (from query or form)
    filename = (
        form.getvalue("file")
        or urllib.parse.parse_qs(query).get("file", [None])[0]
    )

    if not filename:
        message = "No file specified"

    else:
        filename = urllib.parse.unquote(filename)
        path = os.path.join("www/uploads", os.path.basename(filename))
        uploaded_at = form.getvalue("uploaded_at")

        # remove json entry --------------------------------------------------------
        #   load session id ----------------------------------------------
        raw_cookie = os.environ.get("HTTP_COOKIE", "")
        cookie = SimpleCookie()
        cookie.load(raw_cookie)
        session_id = cookie.get("session_id").value if "session_id" in cookie else None

        #   open json file ---------------------------------------------------------
        BASE_DIR = os.path.dirname(__file__)  # directory of CGI script
        MAP_FILE = os.path.join(BASE_DIR, "..", "json", "session_files.json")
        os.makedirs(os.path.dirname(MAP_FILE), exist_ok=True)

        try:
            with open(MAP_FILE, "r") as f:
                data = json.load(f)
        except (FileNotFoundError, json.JSONDecodeError):
            data = {}

        if session_id and session_id in data:
            # Select only entries that not match
            data[session_id] = [
                file_obj for file_obj in data[session_id]
                if not (file_obj.get("encrypt_name") == filename and file_obj.get("uploaded_at") == uploaded_at)
            ]

            # If the session list is empty after deletion, delete the session ID
            if not data[session_id]:
                del data[session_id]

        tmp = MAP_FILE + ".tmp"
        with open(tmp, "w") as f:
            json.dump(data, f, indent=2)
        os.replace(tmp, MAP_FILE)

        # remove file --------------------------------------------------------------
        if os.path.exists(path):
            os.remove(path)
            message = "File deleted successfully!"
        else:
            message = "File not found (´⊙ω⊙`)！"

# Print CGI headers ----------------------------------------------------------------
print(f"Status: {status}")
print("Content-Type: text/html")
print() # This blank line is CRITICAL

print_html("www/html/delete_success.html", message)