<?php
function sendMail() {
    //Checks
    if(empty($_POST["message"])) {
        echo "Message is empty";
        die();
    }

    if(!empty($_POST_["website"])){
        die();
    }

    $to = "cyril.li@telecom-st-etienne.fr";
    $subject = "New message from Digital Holography Reconstruction";
    $message = "You received a new message from ";

    if(!empty($_POST["inputName"])) {
        $message.=htmlspecialchars($_POST["inputName"]).".\n";
    }
    else {
        $message.="Anonymous.\n";
    }
    if(!empty($_POST["app"])) {
        $message.="This message was sent from <code>".htmlspecialchars($_POST["app"])."</code>";
        if(!empty($_POST["value"])) {
            $message.=" with value <code>".htmlspecialchars($_POST["value"])."</code>";
        }
        $message.=".\n";
    }
    if(!empty($_POST["inputEmail"])) {
        $message.="Contact email: ".htmlspecialchars($_POST["inputEmail"])."\n";
    }


    $message.="\n<code>";
    if(!empty($_POST["subject"])) {
        $message.="Subject: ".htmlspecialchars($_POST["subject"])."\n";
    }
    $message.=$_POST["message"]."</code>";

    $headers = array(
        'MIME-Version' => '1.0',
        'Content-type' => 'text/html; charset=iso-8859-1'
    );

    $message = str_replace("\n", "<br>", $message);
    $success = mail($to, $subject, $message, $headers);

    if($success) {
        echo "The message was sent successfully. You will be redirected";
    }
    else {
        echo error_get_last()['message'];
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
<?php sendMail(); ?>
</body>


<script>
    window.setTimeout(function(){ window.location = "index.php"; },3000);
</script>