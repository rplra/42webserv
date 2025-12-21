import cgi 
import csv
import os

form = cgi.FieldStorage()

id = form.getvalue("id", "")
description = form.getvalue("description", "")
done = form.getvalue("done", "")
priority = form.getvalue("priority", "")

csv_dir = "www/csv" 
csv_path = os.path.join(csv_dir, "task.csv")

tasks = []
with open(csv_path, "r") as csvfile:
    reader = csv.reader(csvfile)
    next(reader)
    for row in reader:
        tasks.append(row)

filtered_tasks = []
for idx, desc, d, prio in tasks:
    desc = desc.strip()
    d = d.strip()
    prio = prio.strip()

    if id and str(idx) != id:
        continue
    if description and desc.lower() != description.lower():
        continue
    if done and d != done:
        print(f"done= {done}, d= {d}\n")
        continue
    if priority and prio != priority:
        continue

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
print('</table><br>')
print('<a href="/filterTaskPYTHON">Back to Filter Form</a><br>')
print("</body></html>")