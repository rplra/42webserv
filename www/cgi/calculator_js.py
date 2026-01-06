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
                result = "Division by zero is not supported"
        else:
            result = "Unknown operation"
    except ValueError:
        result = "Invalid input"

# Print CGI headers ----------------------------------------------------------------
print(f"Status: 200 OK")
print("Content-Type: text/plain")
print("")

print(result)