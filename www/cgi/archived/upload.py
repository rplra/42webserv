import  cgi
import  os
import  html

import  json
from    datetime import datetime
from    http.cookies import SimpleCookie
from    print_html import  print_html


upload_dir = "www/uploads"
os.makedirs(upload_dir, exist_ok=True)

form = cgi.FieldStorage()
file_item = form["file"] if "file" in form else None

raw_cookie = os.environ.get("HTTP_COOKIE", "")
cookie = SimpleCookie()
cookie.load(raw_cookie)

status = "201 Created"
message = ""
filename = None

if "session_id" not in cookie: # no session_id
    status = "400 Bad Request"
    message = "Session not found"

elif file_item is None or file_item.filename == "":
    status = "200 OK"
    message = "No file uploaded"

else:
    # Sanitize the filename
    filename = os.path.basename(file_item.filename)
    filepath = os.path.join(upload_dir, filename)

    # Prevent overwriting by adding a counter
    base, ext = os.path.splitext(filename)
    counter = 1
    while os.path.exists(filepath):
        filename = f"{base}_{counter}{ext}"
        filepath = os.path.join(upload_dir, filename)
        counter += 1

    # Save the file
    with open(filepath, "wb") as f:
        f.write(file_item.file.read())

    message = f"File '{filename}' uploaded successfully!"

    # link filename to cookie database --------------------------------------
    session_id = cookie["session_id"].value

    # open json file
    BASE_DIR = os.path.dirname(__file__)  # directory where your CGI script is
    MAP_FILE = os.path.join(BASE_DIR, "..", "json", "session_files.json")
    os.makedirs(os.path.dirname(MAP_FILE), exist_ok=True)

    try:
        with open(MAP_FILE, "r") as f:
            data = json.load(f)
    except (FileNotFoundError, json.JSONDecodeError):
        data = {}

    # save session_id:filename
    data.setdefault(session_id, []).append({
        "filename": filename,
        "uploaded_at": datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    })

    tmp = MAP_FILE + ".tmp"
    with open(tmp, "w") as f:
        json.dump(data, f, indent=2)
    os.replace(tmp, MAP_FILE)


# Print CGI headers ----------------------------------------------------------------
print(f"Status: {status}")
print("Content-Type: text/html")
print() # This blank line is CRITICAL

# Content body
print("<html><body>")
print("<h1>Upload a File</h1>")
print(f"<h2>{html.escape(message)}</h2>")
print('<br><a href="/upload"><button type="button">Upload Another File</button></a>')
print('<a href="/cgi-bin/uploads.py"><button type="button">View Uploaded Files</button></a>')

print("</body></html>")