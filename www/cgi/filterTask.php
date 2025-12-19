<?php
    $id = $_GET['id'] ?? '';
    $description = $_GET['description'] ?? '';
    $done = $_GET['done'] ?? '';
    $priority = $_GET['priority'] ?? '';

    $csvPath = '../csv/task.csv'; 

    $allTasks = [];
    if (($handle = fopen($csvPath, 'r')) !== FALSE) {
        while (($data = fgetcsv($handle, 0, ',', '"', '\\')) !== FALSE) {
            $allTasks[] = $data;
        }
        fclose($handle);
    } 

    $filteredTasks = [];
    foreach ($allTasks as $task) {
        list($idx, $desc, $d, $prio) = $task;

        $desc = trim($desc);
        $d = trim($d);
        $prio = trim($prio);

        if (($id === '' || $id == $idx) &&
            ($description === '' || stripos($desc, $description) !== false) &&
            ($done === '' || $done == $d) &&
            ($priority === '' || $priority == $prio)) {
            $filteredTasks[] = [$idx, $desc, $d, $prio];
        }
    }

    echo "<html><body>";
    echo "<h2>Filtered Tasks:</h2>";
    if (!$filteredTasks) {
        echo "No tasks match the filter criteria.<br>";
    } else {
        echo "<table border='1'><tr><th>ID</th><th>Description</th><th>Done</th><th>Priority</th></tr>";
        foreach ($filteredTasks as $task) {
            echo "<tr>";
            foreach ($task as $field) {
                echo "<td>" . htmlspecialchars($field) . "</td>";
            }
            echo "</tr>";
        }
        echo "</table><br>";
    }
    echo '<a href="/filterTaskPHP">Back to Filter Form</a><br>';
    echo "</body></html>";
?>
