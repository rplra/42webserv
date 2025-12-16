import cgi 
import csv
import os

form = cgi.FieldStorage()

id = form.getvalue("id", "")
description = form.getvalue("description", "")
done = form.getvalue("done", "")
priority = form.getvalue("priority", "")

csv_dir = "cgi" 
csv_path = os.path.join(csv_dir, "task.csv")

tasks = []
with open(csv_path, "r") as csvfile:
    reader = csv.reader(csvfile)
    next(reader)
    for row in reader:
        tasks.append(row)

filtered_tasks = []
for idx, desc, d, prio in tasks:
    if idx == id:
        filtered_tasks.append((idx, desc, d, prio))
    if description.lower() == desc.lower():
        filtered_tasks.append((idx, desc, d, prio))
    if done == d:
        filtered_tasks.append((idx, desc, d, prio))
    if priority == prio:
        filtered_tasks.append((idx, desc, d, prio))
    
print("<html><body>")
print(f"<h1>Filtered Tasks</h1>")
print('<table border="1">')
print("<tr><th>ID</th><th>Description</th><th>Done</th><th>Priority</th></tr>")
if not filtered_tasks:
    print("<tr><td colspan='4'>No tasks match the filter criteria.</td></tr>")
else: 
    for idx, desc, d, prio in filtered_tasks:
        print(f"<tr><td>{idx}</td><td>{desc}</td><td>{d}</td><td>{prio}</td></tr>")
print('</table>')
print('<a href="/filterForm">Back to Filter Form</a><br>')
print("</body></html>")