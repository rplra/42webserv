import cgi 

form = cgi.FieldStorage()

num1 = form.getvalue("num1", "")
num2 = form.getvalue("num2", "")
operation = form.getvalue("operation", "")

result = ""

if num1 and num2 and operation:
    try:
        n1 = float(num1)
        n2 = float(num2)
        if operation == "add":
            result = n1 + n2
        elif operation == "subtract":
            result = n1 - n2
        elif operation == "multiply":
            result = n1 * n2
        elif operation == "divide":
            if n2 != 0:
                result = n1 / n2
            else:
                result = "Error: Division by zero"
        else:
            result = "Error: Unknown operation"
    except ValueError:
        result = "Error: Invalid input"

print("<html><body>")
print("<h1>Result: {}</h1>".format(result))
print()
print("<h1>Simple Calculator</h1>")
print('<form method="post" action="/cgi-bin/calculator.py">')
print('        <label for="num1">Number 1:</label>')
print('        <input type="number" id="num1" name="num1" required><br><br>')
print('        <label for="num2">Number 2:</label>')
print('        <input type="number" id="num2" name="num2" required><br><br>')
print('        <label for="operation">Operation:</label>')
print('        <select id="operation" name="operation" required>')
print('            <option value="add">Add</option>')
print('            <option value="subtract">Subtract</option>')
print('            <option value="multiply">Multiply</option>')
print('            <option value="divide">Divide</option>')
print('        </select><br><br>')
print('        <button type="submit">Calculate</button>')
print('    </form>')
print('<a href="/">Back to Home</a><br>')
print('</body>')
print('</html>')