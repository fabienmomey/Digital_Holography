<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8" name="viewport" content="width=device-width, initial-scale=1">
    <title>Digital Holography Reconstruction - History</title>
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
    <link rel="stylesheet" href="https://www.w3schools.com/w3css/4/w3.css">
    <link rel="stylesheet" href="https://www.w3schools.com/lib/w3-theme-black.css">
    <link rel="stylesheet" href="https://fonts.googleapis.com/css?family=Roboto">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
</head>

<style>
body {font-family: "Roboto", sans-serif; color: black;}
.w3-bar-block .w3-bar-item {
  padding: 16px;
  font-weight: bold;
}
</style>

<body>

  <!-- NavBar a Gauche -->

    <nav class="w3-sidebar w3-bar-block w3-collapse w3-animate-left w3-card" style="z-index:3;width:250px;" id="mySidebar">
        <a class="w3-bar-item w3-button w3-border-bottom w3-large" href="#"><img src="logo_LaHC.png" style="width:80%;"></a>
        <a class="w3-bar-item w3-button w3-hide-large w3-large" href="javascript:void(0)" onclick="w3_close()">Close <i class="fa fa-remove"></i></a>
        <a class="w3-bar-item w3-button" href="index.php">Home</a>
        <a class="w3-bar-item w3-button" href="propagation.php">Back-Propagation</a>
        <a class="w3-bar-item w3-button" href="fienup.php">Fienup</a>
        <a class="w3-bar-item w3-button w3-black" href="history.php">History</a>
    </nav>

    <div class="w3-overlay w3-hide-large w3-animate-opacity" onclick="w3_close()" style="cursor:pointer" id="myOverlay"></div>

    <div class="w3-main" style="margin-left:250px;">

        <div id="myTop" class="w3-container w3-top w3-theme w3-large">
        <p><i class="fa fa-bars w3-button w3-black w3-hide-large w3-xlarge" onclick="w3_open()"></i>
        <span id="myIntro" class="w3-hide">History</span></p>
        </div>

        <header class="w3-container w3-theme" style="padding:64px 32px">
        <h1 class="w3-xxxlarge">History</h1>
        </header>

        <!-- Contenue Page -->

        <div class="w3-container" style="padding:32px">

        <h2>Simulation history of reconstruction algorithms</h2>

        <br />

        <?php
        try
        {
	        // On se connecte à MySQL
	        $bdd = new PDO('mysql:host=localhost;port=3308;dbname=labhc_algorithme;charset=utf8', 'root', '');
        }
        catch(Exception $e)
        {
	        // En cas d'erreur, on affiche un message et on arrête tout
            die('Erreur : '.$e->getMessage());
        }

        // Si tout va bien, on peut continuer

        // On récupère tout le contenu de la table Historique
        $reponse = $bdd->query('SELECT * FROM historique');

        // On affiche chaque entrée une à une
        while ($donnees = $reponse->fetch())
        {
        ?>

        <p>

            <strong>Folder : <?php echo $donnees['Nom_Fichier']; ?></strong><br />
            Date : <?php echo $donnees['Date']; ?><br />
            Image used : <?php echo $donnees['Nom_Image']; ?><br />
            Location : <?php echo $donnees['Emplacement']; ?><br />
            <em>Input parameters : </em><br/>
            Starting depth (m) : <?php echo $donnees['Start_Input']; ?> <br />
            Ending depth (m) : <?php echo $donnees['End_Input']; ?> <br />
            Step (m) : <?php echo $donnees['Step_Input']; ?> <br />
            Wavelength (m) : <?php echo $donnees['Lamda_Input']; ?> <br />
            Pixel size (m) : <?php echo $donnees['Pp_Input']; ?> <br />
            Padding option : <?php echo $donnees['Zeropad_Ratio']; ?> <br />
            Algorithm used : <?php echo $donnees['Méthode']; ?> <br /><br />
            
        </p>

        <?php
        }
        $reponse->closeCursor(); // Termine le traitement de la requête
        ?>

        
    </div>

  <!-- Footer -->

    <footer class="w3-container w3-padding-64 w3-center w3-opacity w3-light-grey w3-xlarge">
      <p class="w3-medium"><a href="https://laboratoirehubertcurien.univ-st-etienne.fr/en/index.html" target="_blank">Laboratoire Hubert Curien</a></p>
    </footer>

<script>
// Open and close the sidebar on medium and small screens
function w3_open() {
  document.getElementById("mySidebar").style.display = "block";
  document.getElementById("myOverlay").style.display = "block";
}

function w3_close() {
  document.getElementById("mySidebar").style.display = "none";
  document.getElementById("myOverlay").style.display = "none";
}

// Change style of top container on scroll
window.onscroll = function() {myFunction()};
function myFunction() {
  if (document.body.scrollTop > 80 || document.documentElement.scrollTop > 80) {
    document.getElementById("myTop").classList.add("w3-card-4", "w3-animate-opacity");
    document.getElementById("myIntro").classList.add("w3-show-inline-block");
  } else {
    document.getElementById("myIntro").classList.remove("w3-show-inline-block");
    document.getElementById("myTop").classList.remove("w3-card-4", "w3-animate-opacity");
  }
}

// Accordions
function myAccordion(id) {
  var x = document.getElementById(id);
  if (x.className.indexOf("w3-show") == -1) {
    x.className += " w3-show";
    x.previousElementSibling.className += " w3-theme";
  } else { 
    x.className = x.className.replace("w3-show", "");
    x.previousElementSibling.className = 
    x.previousElementSibling.className.replace(" w3-theme", "");
  }
}
</script>

</body>

</html>