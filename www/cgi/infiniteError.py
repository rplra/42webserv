import cgi 

form = cgi.FieldStorage()

num1 = form.getvalue("num1", "")

result = ""

if num1:
    while True:
        result = 1 + 1 # Infinite loop to simulate an error

print("<html><body>")
print("<h1>This should never complete!</h1>")
print('</body>')
print('</html>')