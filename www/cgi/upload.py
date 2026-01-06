import  cgi
import  os
import  html

import  json
from    datetime import datetime
from    http.cookies import SimpleCookie
from    print_html import  print_html
from    randomString import generate_string


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

    # Rename file with random string
    base, ext = os.path.splitext(filename)
    encrpt_filename = generate_string() + ext
    filepath = os.path.join(upload_dir, encrpt_filename)

    # Save the uploaded file
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
        "encrypt_name": encrpt_filename,
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

print_html("www/html/upload_success.html", html.escape(message))