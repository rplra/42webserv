<?php
    $username = $_POST['username'] ?? '';
    $password = $_POST['password'] ?? '';

    $csvPath = '../csv/users.csv';
    
    if ($username && $password) {
        if (($handle = fopen($csvPath, 'r')) !== FALSE) {
            $authenticated = false;
            while (($data = fgetcsv($handle, 0, ',', '"', '\\')) !== FALSE) {
                if ($data[0] === $username && $data[1] === $password) {
                    $authenticated = true;
                    break;
                }
            }
            fclose($handle);
        }
    }

    if ($authenticated) {
        echo "<html><body>";
        echo "<h1>Log In Successful</h1>";
        echo "<p>Welcome, " . htmlspecialchars($username) . "!</p>";
        echo "<a href=\"/index\"><button type=\"button\">Go to Home Page</button></a><br><br>";
        echo "</body></html>";
        exit();
    } else {
        $message = "Error: Invalid username or password.";
        echo "<html><body>";
        echo "<h1>Log In</h1>";
        echo "<p style=\"color:red;\">" . htmlspecialchars($message) . "</p>";
        echo "<form action=\"/cgi-bin/logIn\" method=\"post\">";
        echo "<label for=\"username\">Username:</label>";
        echo "<input type=\"text\" id=\"username\" name=\"username\" required><br><br>";
        echo "<label for=\"password\">Password:</label>";
        echo "<input type=\"password\" id=\"password\" name=\"password\" required><br><br>";
        echo "<button type=\"submit\">Log In</button>";
        echo "</form>";
        echo "</body></html>";
        exit();
    }