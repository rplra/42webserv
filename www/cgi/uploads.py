import os
import urllib.parse
import mimetypes
import html

upload_dir = "www/uploads"
os.makedirs(upload_dir, exist_ok=True)

files = []

for name in os.listdir(upload_dir):
    path = os.path.join(upload_dir, name)
    if not os.path.isfile(path):
        continue

    size = os.path.getsize(path)
    mime = mimetypes.guess_type(name)[0] or "unknown"

    files.append({
        "name": name,
        "size": size,
        "type": mime
    })

print("<!DOCTYPE html>")
print("<html><body>")
print("<h1>Uploaded Files</h1>")

if not files:
    print("<p>No files uploaded yet.</p>")
else:
    print("<table border='1'>")
    print("<tr><th>Name</th><th>Size (bytes)</th><th>Type</th><th>Actions</th></tr>")

    for f in files:
        name = f["name"]
        safe_url = urllib.parse.quote(name)  # for links
        safe_html = html.escape(name)        # for display

        print("<tr>")
        print(f"<td>{safe_html}</td>")
        print(f"<td>{f['size']}</td>")
        print(f"<td>{html.escape(f['type'])}</td>")
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
