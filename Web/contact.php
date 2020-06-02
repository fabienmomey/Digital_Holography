<?php
function writeSource() {
    if(isset($_GET["app"])) {
        echo "<input type='hidden' name='app' value ='".htmlspecialchars($_GET["app"])."'>";
    }
    else {
        echo "<input type='hidden' name='app' value =''>";
    }
    if(isset($_GET["value"])) {
        echo "<input type='hidden' name='value' value ='".htmlspecialchars($_GET["value"])."'>";
    }
    else {
        echo "<input type='hidden' name='value' value =''>";
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Digital Holography Reconstruction - Contact</title>
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
    <link rel="stylesheet" href="style.css">
</head>
<body>
<h1>Contact us</h1>
<!--<form action="contact_send.php" method="post">
    <div class="form-group">
        <label for="inputName">Name (optional)</label>
        <input type="text" class="form-control" name="inputName" placeholder="Enter name">
    </div>

    <div class="form-group">
        <label for="inputEmail">Email address (optional)</label>
        <input type="email" class="form-control" name="inputEmail" id="inputEmail" aria-describedby="emailHelp" placeholder="Enter email">
        <small id="emailHelp" class="form-text text-muted">We'll never share your email with anyone else.</small>
    </div>

    <div class="form-group">
        <label for="subject">Subject</label>
        <select class="form-control" name="subject">
            <option>General message</option>
            <option>Suggestion</option>
            <option>Bug report</option>
            <option>Other</option>
        </select>
    </div>

    <div class="form-group">
        <label for="message">Message</label>
        <textarea  class="form-control" name="message" rows="7"></textarea>
    </div>
    <input type="text" style="display:none;" name="website"/>

    <?php //writeSource(); ?>
    <button type="submit" class="btn btn-primary">Submit</button>
</form>--->

<p>
    You can send us an email at the following address : <a href="mailto:corinne.fournier@univ-st-etienne.fr">corinne.fournier@univ-st-etienne.fr</a>
</p>

</body>
