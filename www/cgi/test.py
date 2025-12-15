import cgi 
import csv
import os

form = cgi.FieldStorage()

# Get form values, provide default if missing
name = form.getvalue("name", "Anonymous")
email = form.getvalue("email", "Not provided")
age = form.getvalue("age", "Unknown")

# Save data to CSV file
file_exists = os.path.isfile("/cgi/submissions.csv")
if not file_exists:
    with open("/cgi/submissions.csv", "w", newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["Name", "Email", "Age"])

with open("/cgi/submissions.csv", "a", newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow([name, email, age])

# Read all submissions
submissions = []
with open("/cgi/submissions.csv", "r") as csvfile:
    reader = csv.reader(csvfile)
    next(reader)
    for row in reader:
        submissions.append(row)

# Generate HTML response
print("Content-Type: text/html")
print()  # Blank line separates headers from body

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