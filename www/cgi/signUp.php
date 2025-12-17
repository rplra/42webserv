<?php
    $username = $_POST['username'] ?? '';
    $password = $_POST['password'] ?? '';
    $confirm_password = $_POST['confirm_password'] ?? '';

    $csvPath = '../csv/users.csv';

    if ($username && $password && ($password === $confirm_password)) {
        if (($handle = fopen($csvPath, 'a')) !== FALSE) {
            fputcsv($handle, [$username, $password], ',', '"', '\\');
            fclose($handle);
            $message = "User registered successfully. Please click on the logIn button to log in.";
        } else {
            $errorMessage = "Error: Unable to open the CSV file.";
        }
    } else {
        $errorMessage = "Error: Invalid input or passwords do not match. Please try again.";
    }

    echo "<html><body>";
    echo "<h1>Sign Up</h1>";
    if (isset($errorMessage)) {
        echo "<p style=\"color:red;\">" . htmlspecialchars($errorMessage) . "</p>";
        echo "<form action=\"/cgi-bin/signUp.php\" method=\"post\">";
        echo "<label for=\"username\">Username:</label>";
        echo "<input type=\"text\" id=\"username\" name=\"username\" required><br><br>";
        echo "<label for=\"password\">Password:</label>";
        echo "<input type=\"password\" id=\"password\" name=\"password\" required><br><br>";
        echo "<label for=\"confirm_password\">Confirm Password:</label>";
        echo "<input type=\"password\" id=\"confirm_password\" name=\"confirm_password\" required><br><br>";
        echo "<button type=\"submit\">Sign Up</button>";
        echo "</form>";

    } else if (isset($message)) {
        echo "<p style=\"color:green;\">" . htmlspecialchars($message) . "</p>";
        echo "<a href=\"/login\"><button type=\"button\">Log In</button></a><br><br>";
    }
    echo "</body></html>";
?>
