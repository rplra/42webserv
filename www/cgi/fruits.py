#!/usr/bin/env python3
import cgi

# "database" of fruits
fruits = [
    {"name": "Apple", "color": "Red"},
    {"name": "Banana", "color": "Yellow"},
    {"name": "Lemon", "color": "Yellow"},
    {"name": "Grapes", "color": "Purple"},
    {"name": "Orange", "color": "Orange"},
    {"name": "Strawberry", "color": "Red"},
    {"name": "Blueberry", "color": "Blue"},
    {"name": "Kiwi", "color": "Green"},
    {"name": "Mango", "color": "Orange"},
    {"name": "Watermelon", "color": "Green"},
    {"name": "Cherry", "color": "Red"},
    {"name": "Pineapple", "color": "Yellow"}
]

# get query parameter
form = cgi.FieldStorage()
query = form.getvalue("color", "").strip().lower()

# filter fruits (exact match, case-insensitive)
matches = [f for f in fruits if f["color"].lower() == query]

# HTML output
print("<html><body>")
if matches:
    print(f"<h1>Fruits with color '{query}'</h1>")
    print("<ul>")
    for f in matches:
        print(f"<li>{f['name']} - {f['color']}</li>")
    print("</ul>")
else:
    if query == "":
        print("<p>Please enter a color to search.</p>")
    else:
        print(f"<p>Can't find color '{query}'. Please enter a valid color.</p>")
print("<a href='/fruits/fruits.html'>Back to all fruits</a>")
print("</body></html>")
