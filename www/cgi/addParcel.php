<?php
    // Get Input
    $id = mt_rand(1000, 9999);
    $sender = $_POST['sender'] ?? '';
    $weight = $_POST['weight'] ?? '';

    $csvPath = '../csv/parcels.csv';

    if ($id && $sender && $weight) {
        if (($handle = fopen($csvPath, 'a')) !== FALSE) {
            fputcsv($handle, [$id, $sender, $weight], ',', '"', '\\');
            fclose($handle);
        }
    }

    echo "<html><body>";
    echo "Parcel added successfully.";
    echo "<br>Parcel ID: " . htmlspecialchars($id);
    echo "<br>Sender: " . htmlspecialchars($sender);
    echo "<br>Weight: " . htmlspecialchars($weight);
    echo "<br>";

    $allParcels = [];
    if (($handle = fopen($csvPath, 'r')) !== FALSE) {
        while (($data = fgetcsv($handle, 0, ',', '"', '\\')) !== FALSE) {
            $allParcels[] = $data;
        }
        fclose($handle);
    }
    echo "<h3>All Parcels:</h3>";
    echo "<table border='1'><tr><th>ID</th><th>Sender</th><th>Weight (kg)</th></tr>";
    foreach ($allParcels as $parcel) {
        echo "<tr>";
        foreach ($parcel as $field) {
            echo "<td>" . htmlspecialchars($field) . "</td>";
        }
        echo "</tr>";
    }
    echo "</table>";
    echo "<br><a href=\"/index\">Back to Main</a>";
    echo "</body></html>";
?>