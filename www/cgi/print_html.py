# serves html page for python
# only supports one dynamic message
# -------------------------------------------------------------------

def replace_message(line, message):
    # Replace all occurrences of {{message}} in the line
    return line.replace("{{message}}", message)

def print_html(filename, message):
    # Try opening the file and processing each line
    try:
        with open(filename, 'r') as file:
            # Read each line from the file
            for line in file:
                # Replace {{message}} with the provided message
                modified_line = replace_message(line, message)
                # Print the modified line
                print(modified_line, end='')  # end='' avoids extra newlines

    except FileNotFoundError:
        print(f"Error: The file '{filename}' was not found.")
    except Exception as e:
        print(f"Error: {e}")