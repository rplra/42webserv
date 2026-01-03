import cgi
import os
import html

upload_dir = "www/uploads"
os.makedirs(upload_dir, exist_ok=True)

print("<html><body>")
print("<h1>Upload a File</h1>")

form = cgi.FieldStorage()
file_item = form["file"] if "file" in form else None

if file_item is not None and file_item.filename:
    # Sanitize the filename
    filename = os.path.basename(file_item.filename)
    filepath = os.path.join(upload_dir, filename)

    # Prevent overwriting by adding a counter
    # base, ext = os.path.splitext(filename)
    # counter = 1
    # while os.path.exists(filepath):
    #     filename = f"{base}_{counter}{ext}"
    #     filepath = os.path.join(upload_dir, filename)
    #     counter += 1

    # Save the file
    with open(filepath, "wb") as f:
        f.write(file_item.file.read())

    print(f"<h2>File '{html.escape(filename)}' uploaded successfully!</h2>")

print('<br><a href="/upload"><button type="button">Upload Another File</button></a>')
print('<a href="/cgi-bin/uploads.py"><button type="button">View Uploaded Files</button></a>')

print("</body></html>")
