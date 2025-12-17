import cgi
import os
import urllib.parse

form = cgi.FieldStorage()
filename = form.getvalue("file")  # filename to delete

upload_dir = "www/uploads"
filepath = os.path.join(upload_dir, filename)

if filename:
    filename = urllib.parse.unquote(filename)
    path = os.path.join(upload_dir, os.path.basename(filename))

    if os.path.exists(path):
        os.remove(path)
        print("<html><body>")
        print("<h1>File deleted</h1>")
    else:
        print("<h1>File not found</h1>")
else:
    print("<h1>No file specified</h1>")

print('<a href="/cgi-bin/uploads.py">Back to uploads</a>')
print("</body></html>")
