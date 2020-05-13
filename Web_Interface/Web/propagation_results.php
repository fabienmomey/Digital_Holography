<?php
    // Renvoie vrai si la chaine $str de caractère représente un floatant, faux sinon
    function isFloat($str) {
        if(preg_match("/^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$/", $str) !== 1){
            return false;
        }
        return true;
    }

    // Renvoie une chaine aléatoire de $length caractères
    function generateRandomString($length = 10) {
        $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
        $charactersLength = strlen($characters);
        $randomString = '';
        for ($i = 0; $i < $length; $i++) {
            $randomString .= $characters[rand(0, $charactersLength - 1)];
        }
        return $randomString;
    }

    function runScript($filename) {
        //Génération de l'ID
        do {
            $destname = generateRandomString(20);
        } while(file_exists("output/propagation/".$destname) || is_dir("output/propagation/".$destname) );

        // Récupération des données
        $startz = htmlspecialchars($_POST["start_input"]);
        $endz = htmlspecialchars($_POST["end_input"]);
        $stepz = htmlspecialchars($_POST["step_input"]);
        $lambda = htmlspecialchars($_POST["lambda_input"]);
        $pp = htmlspecialchars($_POST["pp_input"]);
        $zeropadvalue = htmlspecialchars($_POST["zeropad_radio"]);

        if(strcmp($zeropadvalue, "nopad") === 0) {
            $zeropad = "0";
        }
        else if(strcmp($zeropadvalue, "fullpad") === 0) {
            $zeropad= "2";
        }
        else {
            $zeropad= "1";
        }

        // Test de validité des données
        $validInput = true;

        $validInput = $validInput and isFloat($startz);
        $validInput = $validInput and isFloat($endz);
        $validInput = $validInput and isFloat($stepz);
        $validInput = $validInput and isFloat($lambda);
        $validInput = $validInput and isFloat($pp);
        $validInput = $validInput and file_exists($filename);

        if($validInput) {
            //Commande qui excécute le programme avec les paramètres du formulaire
            exec(escapeshellcmd("\"./bin/Propagation\" uploads/" . $filename . " " . $destname . " " . $startz . " " . $endz . " " . $stepz . " " . $lambda . " " . $pp . " " . $zeropad));
            //Redirection vers l'affichage du résultat
            $newURL = "propagation_sharable.php?value=".$destname;
            sleep(0.1);
            header('Location: '.$newURL);
            exit();
        }
        else
            echo "Error : wrong arguments";
    }

    function uploadFile() {
        // Si on utilise une image d'un preset, pas besoin d'uploader l'image
        if(strcmp($_POST["presetFile"], "") !== 0) {
            runScript($_POST["presetFile"]);
        }
        else {
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
