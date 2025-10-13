<?php
$firstName = $_GET['first_name'] ?? '';
$lastName  = $_GET['last_name'] ?? '';
$login     = $_GET['login'] ?? '';
$consent   = isset($_GET['consent']) ? "Yes" : "No"; //box only appears in POST if checked
// modify so no profile if consent is not given
$firstName = htmlspecialchars($firstName, ENT_QUOTES, 'UTF-8');
$lastName  = htmlspecialchars($lastName, ENT_QUOTES, 'UTF-8');
$login     = htmlspecialchars($login, ENT_QUOTES, 'UTF-8');
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <link href="https://fonts.googleapis.com/css2?family=Roboto&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="styles.css">
    <title><?php echo $firstName . " " . $lastName; ?> – Profile</title>
</head>
<body>
    <div class="block">
        <h1>Profile Created</h1>
        <p><strong>First Name:</strong> <?php echo $firstName; ?></p>
        <p><strong>Last Name:</strong> <?php echo $lastName; ?></p>
        <p><strong>42 Login:</strong> <?php echo $login; ?></p>
        <p><strong>Consent to share data:</strong> <?php echo $consent; ?></p>

        <a href="register.html" class="lien-perso">⬅ Register another profile</a>
        <a href="index.html" class="lien-perso">⬅ Back to Portfolio</a>
    </div>

    <div class="toggle-container">
        <label class="switch">
            <input type="checkbox" id="darkModeToggle">
            <span class="slider"></span>
        </label>
        <span class="toggle-label">Mode sombre</span>
    </div>

    <script src="darkmode.js"></script>
</body>
</html>
