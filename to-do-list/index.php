<?php
// Database configuration
$host = 'localhost';
$dbname = 'todo_db';
$username = 'mouse'; // Replace with your MySQL username
$password = '1234'; // Replace with your MySQL password

try {
    $pdo = new PDO("mysql:host=$host;dbname=$dbname", $username, $password);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch(PDOException $e) {
    die("Connection failed: " . $e->getMessage());
}

// Handle form submission to add a task
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['task'])) {
    $task = trim($_POST['task']);
    if (!empty($task)) {
        $stmt = $pdo->prepare("INSERT INTO tasks (task_text, completed) VALUES (?, 0)");
        $stmt->execute([$task]);
    }
}

// Handle task deletion
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['delete_id'])) {
    $id = $_POST['delete_id'];
    $stmt = $pdo->prepare("DELETE FROM tasks WHERE id = ?");
    $stmt->execute([$id]);
}

// Handle task completion toggle
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['toggle_id'])) {
    $id = $_POST['toggle_id'];
    $stmt = $pdo->prepare("UPDATE tasks SET completed = NOT completed WHERE id = ?");
    $stmt->execute([$id]);
}

// Fetch all tasks
$stmt = $pdo->query("SELECT * FROM tasks ORDER BY id DESC");
$tasks = $stmt->fetchAll(PDO::FETCH_ASSOC);
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Simple To-Do List</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: Arial, sans-serif;
        }

        body {
            background-image: url("image6.jpg");
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            padding: 20px;
        }

        .todo-container {
            background-color: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
            width: 100%;
            max-width: 400px;
        }

        h1 {
            text-align: center;
            color: #333;
            margin-bottom: 20px;
        }

        .input-container {
            display: flex;
            margin-bottom: 20px;
        }

        #todoInput {
            flex: 1;
            padding: 10px;
            border: 1px solid #ddd;
            border-radius: 5px 0 0 5px;
            font-size: 16px;
        }

        #addBtn {
            padding: 10px 20px;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 0 5px 5px 0;
            cursor: pointer;
            font-size: 16px;
        }

        #addBtn:hover {
            background-color: #45a049;
        }

        #todoList {
            list-style-type: none;
        }

        .todo-item {
            display: flex;
            align-items: center;
            padding: 10px;
            margin-bottom: 10px;
            background-color: #f9f9f9;
            border-radius: 5px;
            transition: background-color 0.3s;
        }

        .todo-item.completed {
            background-color: #e0e0e0;
            text-decoration: line-through;
            color: #666;
        }

        .todo-item button {
            margin-left: auto;
            padding: 5px 10px;
            background-color: #ff4444;
            color: white;
            border: none;
            border-radius: 3px;
            cursor: pointer;
        }

        .todo-item button:hover {
            background-color: #cc0000;
        }
    </style>
</head>
<body>
    <div class="todo-container">
        <h1>To-Do List</h1>
        <div class="input-container">
            <form method="POST" id="taskForm">
                <input type="text" id="todoInput" name="task" placeholder="Enter a task...">
                <button type="submit" id="addBtn">Add</button>
            </form>
        </div>
        <ul id="todoList">
            <?php foreach ($tasks as $task): ?>
                <li class="todo-item <?php echo $task['completed'] ? 'completed' : ''; ?>" data-id="<?php echo $task['id']; ?>">
                    <input type="checkbox" <?php echo $task['completed'] ? 'checked' : ''; ?> onchange="toggleTask(<?php echo $task['id']; ?>)">
                    <span style="margin-left: 10px; flex: 1;"><?php echo htmlspecialchars($task['task_text']); ?></span>
                    <button onclick="deleteTask(<?php echo $task['id']; ?>)">Delete</button>
                </li>
            <?php endforeach; ?>
        </ul>
    </div>

    <script>
        function addTask() {
            document.getElementById('taskForm').submit();
        }

        function deleteTask(id) {
            if (confirm("Are you sure you want to delete this task?")) {
                var formData = new FormData();
                formData.append('delete_id', id);

                fetch('', {
                    method: 'POST',
                    body: formData
                }).then(response => {
                    location.reload(); // Refresh the page to reflect changes
                });
            }
        }

        function toggleTask(id) {
            var formData = new FormData();
            formData.append('toggle_id', id);

            fetch('', {
                method: 'POST',
                body: formData
            }).then(response => {
                location.reload(); // Refresh the page to reflect changes
            });
        }

        // Event listeners for Enter key and add button
        document.getElementById('addBtn').addEventListener('click', addTask);
        document.getElementById('todoInput').addEventListener('keypress', function(e) {
            if (e.key === 'Enter') {
                addTask();
            }
        });
    </script>
</body>
</html>