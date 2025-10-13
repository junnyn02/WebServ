<?php
$rating = $_GET['rating'] ?? '';
$hire   = isset($_GET['hire']) ? "Yes" : "No";
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <link href="https://fonts.googleapis.com/css2?family=Roboto&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="styles.css">
    <title>Thank you</title>
</head>
<body>
    <div class="block">
        <h1>Thank you for your feedback!</h1>
        <p><strong>You rated my portfolio:</strong> <?php echo $rating; ?></p>
		<?php if ($hire == "Yes") :
		?>
			<p><strong>You said you would hire me! My contact information is on the website.</strong></p>
		<?php else : ?>
			<p><strong>You said you would not hire. Too bad :/</strong></p>
		<?php endif; ?>
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
