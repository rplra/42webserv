import cgi 
import csv
import os

form = cgi.FieldStorage()

# Get form values, provide default if missing
name = form.getvalue("name", "Anonymous")
email = form.getvalue("email", "Not provided")
age = form.getvalue("age", "Unknown")

# Save data to CSV file
csv_dir = "csv" 
csv_path = os.path.join(csv_dir, "submissions.csv")

if not csv_path:
    with open(csv_path, "w", newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["Name", "Email", "Age"])

with open(csv_path, "a", newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow([name, email, age])

# Read all submissions
submissions = []
with open(csv_path, "r") as csvfile:
    reader = csv.reader(csvfile)
    next(reader)
    for row in reader:
        submissions.append(row)
        
print("<html><body>")
print(f"<h1>Form submitted successfully</h1>")
print(f"<h1>Thank you, {name}!</h1>")
print(f"<p>Email: {email}</p>")
print(f"<p>Age: {age}</p>")

print()

print('<table border="1">')
print("<tr><th>Name</th><th>Email</th><th>Age</th></tr>")
for name, email, age in submissions:
    print(f"<tr><td>{name}</td><td>{email}</td><td>{age}</td></tr>")
print('</table>')

print('<a href="/form">Submit another response</a><br>')
print("</body></html>")