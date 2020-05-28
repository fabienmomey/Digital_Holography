<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8" name="viewport" content="width=device-width, initial-scale=1">
    <title>Digital Holography Reconstruction</title>
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
        <a class="w3-bar-item w3-button w3-black" href="index.php">Home</a>
        <a class="w3-bar-item w3-button" href="propagation.php">Back-Propagation</a>
        <a class="w3-bar-item w3-button" href="fienup.php">Fienup</a>
    </nav>

    <div class="w3-overlay w3-hide-large w3-animate-opacity" onclick="w3_close()" style="cursor:pointer" id="myOverlay"></div>

    <div class="w3-main" style="margin-left:250px;">

        <div id="myTop" class="w3-container w3-top w3-theme w3-large">
        <p><i class="fa fa-bars w3-button w3-black w3-hide-large w3-xlarge" onclick="w3_open()"></i>
        <span id="myIntro" class="w3-hide">Digital Holography Reconstruction</span></p>
        </div>

        <header class="w3-container w3-theme" style="padding:64px 32px">
        <h1 class="w3-xxxlarge">Digital Holography Reconstruction</h1>
        </header>

        <!-- Contenue Page -->

        <div class="w3-container" style="padding:32px">

        <h2>Presentation of the Hubert Curien Laboratory</h2>

        <p>The Hubert Curien laboratory is a joint research unit (UMR 5516 ) of the Jean Monnet University, Saint-Etienne, the National Research Centre "CNRS" 
            and the Institut d’Optique Graduate School. It is composed of about 90 researchers, professors and assistant professors, 20 engineers and administrative staff and 
            130 PhD and post-PhD students. This makes the Hubert Curien laboratory with a total of about 240 staff the most important research structure of Saint-etienne. 
            Our research activities are organized according to two scientific departments: Optics, photonics and microwave and Computer science, telecom and image. 
            The research activity is structured into scientific projects within 6 main teams: Micro/Nano structuring, Radiation-matter Interaction, 
            Image Science & computer vision, Data Intelligence, Connected Intelligence and Secure Embedded Systems & Hardware Architetctures.
        </p>
        <p>Created in 2006, the Hubert Curien laboratory is a joint research unit (UMR 5516 ) of the Jean Monnet University, Saint-Etienne, the National Research Centre "CNRS" and the 
            Institut d’Optique Graduate School. It is composed of about 240 members who work on scientific topics related to optics, photonics and microwave, computer science, telecom and image.
        </p>

        <br><br>

        <h2>Presentation of Digital Holographic Microscopy for Biological Imaging</h2>

        <p>Health care domain is always on the lookout for new simple and effective devices, for biological research and screening/identification/diagnostic of diseases. 
        High resolution 3D quantitative imaging of cell structures and of their dynamics is a powerful approach in various biomedical applications. 
        Digital holography is a state of the art technique providing quantitative images of biological samples with added functionalities over conventional microscopes: 
        label-free, high contrast, quantitative. It is based on the principle of interferences between a coherent illumination beam and the beam diffracted through the sample. 
        The resulting interference patterns (holograms) are recorded by a digital sensor and reconstructed (refocused) numerically to estimate the objects size, phase profile, 
        refractive index, absorption coefficient. All these data lead to identification and classification of phenotypes of biological objects (e.g. cells, bacteria).</p>

        <p>To better optimize reconstruction and improve the classification results, we co-design both holographic setups and reconstruction algorithms based on inverse problems approach.</p>
        <br>

        <div class="w3-container w3-center">
            <img src="videoOfHolo.gif" alt="car" style="width:80%;max-width:320px">
            <p>Video of holograms of cells (recorded at CEA Leti, Cédric Allier).</p>
        </div>

        <div class="w3-padding-64 w3-content w3-text-black" id="contact">
    <h2 class="w3-text-black">Contact</h2>
    <hr style="width:200px" class="w3-opacity">

    <div class="w3-section">
      <p><i class="fa fa-map-marker fa-fw w3-text-black w3-xxlarge w3-margin-right"></i> Bâtiment F 18 Rue du Professeur Benoît Lauras, 42000 Saint-Etienne, FRANCE</p>
      <p><i class="fa fa-phone fa-fw w3-text-black w3-xxlarge w3-margin-right"></i> Phone: +33 (0)4 77 91 57 80</p>
      <p><i class="fa fa-envelope fa-fw w3-text-black w3-xxlarge w3-margin-right"> </i> Email: <a href="mailto:corinne.fournier@univ-st-etienne.fr">corinne.fournier@univ-st-etienne.fr</a></p>
    </div><br>
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