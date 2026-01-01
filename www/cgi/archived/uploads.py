import  os
import  urllib.parse
import  mimetypes
import  html
import  json
from    http.cookies import SimpleCookie

upload_dir = "www/uploads"
os.makedirs(upload_dir, exist_ok=True)

# load session id ----------------------------------------------
raw_cookie = os.environ.get("HTTP_COOKIE", "")
cookie = SimpleCookie()
cookie.load(raw_cookie)
session_id = cookie.get("session_id").value if "session_id" in cookie else None

# open json file ----------------------------------------------
BASE_DIR = os.path.dirname(__file__)  # directory of CGI script
MAP_FILE = os.path.join(BASE_DIR, "..", "json", "session_files.json")
os.makedirs(os.path.dirname(MAP_FILE), exist_ok=True)

try:
    with open(MAP_FILE, "r") as f:
        session_data = json.load(f)
except (FileNotFoundError, json.JSONDecodeError):
    session_data = {}

# get associated file -----------------------------------------
files = []
if session_id and session_id in session_data:
    for file_obj in session_data[session_id]:
        name = file_obj.get("filename")
        path = os.path.join(upload_dir, name)
        if not os.path.isfile(path):
            continue
        size = os.path.getsize(path)
        mime = mimetypes.guess_type(name)[0] or "unknown"
        files.append({
            "name": name,
            "size": size,
            "type": mime,
            "uploaded_at": file_obj.get("uploaded_at")
        })

# Print CGI headers -------------------------------------------
print("Status: 200")
print("Content-Type: text/html")
print() # This blank line is CRITICAL

# generate HTML -----------------------------------------------
print("<!DOCTYPE html>")
print("<html><body>")
print("<h1>Your Uploaded Files</h1>")

if not files:
    print("<p>No files uploaded yet.</p>")
else:
    print("<table border='1'>")
    print("<tr><th>Name</th><th>Size (bytes)</th><th>Type</th><th>Uploaded At</th><th>Actions</th></tr>")

    for f in files:
        name = f["name"]
        safe_url = urllib.parse.quote(name)
        safe_html = html.escape(name)

        print("<tr>")
        print(f"<td>{safe_html}</td>")
        print(f"<td>{f['size']}</td>")
        print(f"<td>{html.escape(f['type'])}</td>")
        print(f"<td>{html.escape(f['uploaded_at'])}</td>")
        print("<td>")
        print(f'<a href="/uploads/{safe_url}" target="_blank">View</a> ')
        print(
            '<form action="/cgi-bin/delete.py" method="post" style="display:inline;">'
            f'<input type="hidden" name="file" value="{safe_html}">'
            '<input type="hidden" name="_method" value="DELETE">'
            '<button type="submit">Delete</button>'
            '</form>'
        )
        print("</td>")
        print("</tr>")

    print("</table>")

print('<br><a href="/upload"><button type="button">Upload another file</button></a>')
print("</body></html>")