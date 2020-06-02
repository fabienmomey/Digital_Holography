<?php

function isFloat($str) {
    if(preg_match("/^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$/", $str) !== 1){
        return false;
    }
    return true;
}

function runScript($filename) {

    $destname = htmlspecialchars($_POST["sessionID"]);

    $x = htmlspecialchars($_POST["x_input"]);
    $y = htmlspecialchars($_POST["y_input"]);
    $z = htmlspecialchars($_POST["z_input"]);
    $r = htmlspecialchars($_POST["r_input"]);
    $n1 = htmlspecialchars($_POST["n1_input"]);
    $n1i = htmlspecialchars($_POST["n1i_input"]);

    $lambda = htmlspecialchars($_POST["lambda_input"]);
    $pix = htmlspecialchars($_POST["pix_input"]);
    $n0 = htmlspecialchars($_POST["n0_input"]);
    $xe = htmlspecialchars($_POST["xe_input"]);
    $ye = htmlspecialchars($_POST["ye_input"]);
    $n = htmlspecialchars($_POST["n_input"]);

    $validInput = true;

    $validInput = $validInput and isFloat($x);
    $validInput = $validInput and isFloat($y);
    $validInput = $validInput and isFloat($z);
    $validInput = $validInput and isFloat($r);
    $validInput = $validInput and isFloat($n1);
    $validInput = $validInput and isFloat($n1i);
    $validInput = $validInput and isFloat($lambda);
    $validInput = $validInput and isFloat($pix);
    $validInput = $validInput and isFloat($n0);
    $validInput = $validInput and isFloat($xe);
    $validInput = $validInput and isFloat($ye);
    $validInput = $validInput and isFloat($n);
    $validInput = $validInput and file_exists($filename);

    if($validInput) {
        exec(escapeshellcmd("\"./bin/OptimisationLocale\" uploads/" . $filename . " " . $destname . " " . $x . " " . $y . " " . $z . " " . $r . " " . $n1 .
            " " . $n1i . " " . $lambda . " " . $pix . " " . $n0 ." " . $xe ." " . $ye . " " . $n));
    }
    else
        echo "Error : wrong arguments";
}

function uploadFile()
{
    if (strcmp($_POST["presetFile"], "") !== 0) {
        runScript($_POST["presetFile"]);
    } else {
        $target_dir = "uploads/";
        $target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
        $uploadOk = 1;
        $imageFileType = strtolower(pathinfo($target_file, PATHINFO_EXTENSION));
        // Check if image file is a actual image or fake image
        if (isset($_POST["submit"])) {
            $check = getimagesize($_FILES["fileToUpload"]["tmp_name"]);
            if ($check !== false) {
                $uploadOk = 1;
            } else {
                echo "File is not an image.<br>";
                $uploadOk = 0;
            }
        }
        /*// Check file size
        if ($_FILES["fileToUpload"]["size"] > 500000) {
            echo "Sorry, your file is too large.<br>";
            $uploadOk = 0;
        }*/
        // Allow certain file formats
        if ($uploadOk == 1 && $imageFileType != "jpg" && $imageFileType != "png" && $imageFileType != "jpeg"
            && $imageFileType != "gif") {
            echo "Sorry, only JPG, JPEG, PNG & GIF files are allowed.<br>";
            $uploadOk = 0;
        }
        // Check if $uploadOk is set to 0 by an error
        if ($uploadOk == 0) {
            echo "Sorry, your file was not uploaded.<br>";
            // if everything is ok, try to upload file
        } else {
            if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
                runScript(basename($_FILES["fileToUpload"]["name"]));
            } else {
                echo "Sorry, there was an error uploading your file.<br>";
            }
        }
    }

}


uploadFile();