<?php

function loadFile() {
    //Lecture du fichier de sortie
    $params = "output/propagation/".$_GET["value"]."/param.html";
    //Lecture des logs
    $log = "output/log/propag_".date("Ymd")."_".$_GET["value"].".txt";
    if(file_exists($params)) {
        echo "<h2>Parameters</h2>";
        echo file_get_contents($params);
        echo "<h2>Share</h2>";
        echo "<form><div class=\"form-group\"><label for=\"link\"><a target='_blank' rel='noopener noreferrer' href='propagation_download.php?value=".$_GET["value"]."'>Download files</a></label><br>";
        echo "<div class=\"form-group\"><label for=\"link\"><a target='_blank' rel='noopener noreferrer' href='contact.php?app=propagation&value=".$_GET["value"]."'>Contact us</a></label><br>";
        echo "<label for=\"link\">Sharable link:</label><div class=\"col-sm-10\"><input type='text' onclick='this.select();' class='form-control' name='link' value='localhost/stage%20labo/propagation_sharable.php?value=".$_GET["value"]."'></div></div></form></p>";
    }
    else {
        echo "<p>Something went wrong. Please try again or <a href='contact.php?app=propagation&value=".$_GET["value"]."'>contact us</a>.</p>";
        if(file_exists($log)) {
            $order   = array("\r\n", "\n", "\r");
            echo "<p><code>".str_replace($order, '<br>', file_get_contents($log))."</code></p>";
        }
    }
}

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8" name="viewport" content="width=device-width, initial-scale=1">
    <title>Digital Holography Reconstruction - Backpropagtion</title>
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
.slidecontainer {
  width: 100%; /* Width of the outside container */
}

/* The slider itself */
.slider {
  -webkit-appearance: none;  /* Override default CSS styles */
  appearance: none;
  width: 100%; /* Full-width */
  height: 25px; /* Specified height */
  background: #d3d3d3; /* Grey background */
  outline: none; /* Remove outline */
  opacity: 0.7; /* Set transparency (for mouse-over effects on hover) */
  -webkit-transition: .2s; /* 0.2 seconds transition on hover */
  transition: opacity .2s;
}

/* Mouse-over effects */
.slider:hover {
  opacity: 1; /* Fully shown on mouse-over */
}

/* The slider handle (use -webkit- (Chrome, Opera, Safari, Edge) and -moz- (Firefox) to override default look) */
.slider::-webkit-slider-thumb {
  -webkit-appearance: none; /* Override default look */
  appearance: none;
  width: 25px; /* Set a specific slider handle width */
  height: 25px; /* Slider handle height */
  background: #4CAF50; /* Green background */
  cursor: pointer; /* Cursor on hover */
}

.slider::-moz-range-thumb {
  width: 25px; /* Set a specific slider handle width */
  height: 25px; /* Slider handle height */
  background: #000000; /* Green background */
  cursor: pointer; /* Cursor on hover */
}

* {box-sizing: border-box;}

.img-zoom-container {
  position: relative;
}

.img-zoom-lens {
  position: absolute;
  border: 1px solid #d4d4d4;
  /*set the size of the lens:*/
  width: 100px;
  height: 100px;
}

.img-zoom-result {
  border: 1px solid #d4d4d4;
  /*set the size of the result div:*/
  width: 636px;
  height: 636px;
}
</style>

<script>

function imageZoom(imgID, resultID) {
  var img, lens, result, cx, cy;
  img = document.getElementById(imgID);
  result = document.getElementById(resultID);
  /*create lens:*/
  lens = document.createElement("DIV");
  lens.setAttribute("class", "img-zoom-lens");
  /*insert lens:*/
  img.parentElement.insertBefore(lens, img);
  /*calculate the ratio between result DIV and lens:*/
  cx = result.offsetWidth / lens.offsetWidth;
  cy = result.offsetHeight / lens.offsetHeight;
  /*set background properties for the result DIV:*/
  result.style.backgroundImage = "url('" + img.src + "')";
  result.style.backgroundSize = (img.width * cx) + "px " + (img.height * cy) + "px";
  /*execute a function when someone moves the cursor over the image, or the lens:*/
  lens.addEventListener("mousemove", moveLens);
  img.addEventListener("mousemove", moveLens);
  /*and also for touch screens:*/
  lens.addEventListener("touchmove", moveLens);
  img.addEventListener("touchmove", moveLens);
  function moveLens(e) {
    var pos, x, y;
    /*prevent any other actions that may occur when moving over the image:*/
    e.preventDefault();
    /*get the cursor's x and y positions:*/
    pos = getCursorPos(e);
    /*calculate the position of the lens:*/
    x = pos.x - (lens.offsetWidth / 2);
    y = pos.y - (lens.offsetHeight / 2);
    /*prevent the lens from being positioned outside the image:*/
    if (x > img.width - lens.offsetWidth) {x = img.width - lens.offsetWidth;}
    if (x < 0) {x = 0;}
    if (y > img.height - lens.offsetHeight) {y = img.height - lens.offsetHeight;}
    if (y < 0) {y = 0;}
    /*set the position of the lens:*/
    lens.style.left = x + "px";
    lens.style.top = y + "px";
    /*display what the lens "sees":*/
    result.style.backgroundPosition = "-" + (x * cx) + "px -" + (y * cy) + "px";
  }
  function getCursorPos(e) {
    var a, x = 0, y = 0;
    e = e || window.event;
    /*get the x and y positions of the image:*/
    a = img.getBoundingClientRect();
    /*calculate the cursor's x and y coordinates, relative to the image:*/
    x = e.pageX - a.left;
    y = e.pageY - a.top;
    /*consider any page scrolling:*/
    x = x - window.pageXOffset;
    y = y - window.pageYOffset;
    return {x : x, y : y};
  }
}
</script>

<body>

    <!-- NavBar a Gauche -->

    <nav class="w3-sidebar w3-bar-block w3-collapse w3-animate-left w3-card" style="z-index:3;width:250px;" id="mySidebar">
        <a class="w3-bar-item w3-button w3-border-bottom w3-large" href="#"><img src="logo_LaHC.png" style="width:80%;"></a>
        <a class="w3-bar-item w3-button w3-hide-large w3-large" href="javascript:void(0)" onclick="w3_close()">Close <i class="fa fa-remove"></i></a>
        <a class="w3-bar-item w3-button" href="index.php">Home</a>
        <a class="w3-bar-item w3-button w3-black" href="propagation.php">Back-Propagation</a>
        <a class="w3-bar-item w3-button" href="optilocale.php">Local Optimization</a>
    </nav>

    <div class="w3-overlay w3-hide-large w3-animate-opacity" onclick="w3_close()" style="cursor:pointer" id="myOverlay"></div>

    <div class="w3-main" style="margin-left:250px;">

        <div id="myTop" class="w3-container w3-top w3-theme w3-large">
        <p><i class="fa fa-bars w3-button w3-black w3-hide-large w3-xlarge" onclick="w3_open()"></i>
        <span id="myIntro" class="w3-hide">Backpropagation</span></p>
        </div>

        <header class="w3-container w3-theme" style="padding:64px 32px">
            <h1 class="w3-xxxlarge">Backpropagation</h1>
        </header>

    <!-- Contenue Page -->

    <div class="w3-container" style="padding:32px">


<?php loadFile(); ?>
<br>
    <h2>Results</h2>

    <!-- Slider Profondeur -->

    <form>
        <label for="z_slider">Depth <span id="depth"></span>: </label>
     <input type="range" class="slider" min="0" max="29" value="0" id="z_slider">
    </form>

    <!-- Image Resultat -->

    

<div class="w3-row w3-padding-32">
    <div class="img-zoom-container">
    <div class="w3-half">
  <img id="result" src="" width="636" height="636"></div>
  <div id="myresult" class="img-zoom-result w3-half"></div>
</div>
</div>

    <!-- Slider Zoom -->

    <form>
        <label for="slider_zoom">Zoom</label>
     <input type="range" class="slider" min="100" max="636" value="0" id="slider_zoom">
    </form>

 

    <button class="w3-button w3-black w3-round-xxlarge"><a href="propagation.php">Back</a></button>

    </div>
</div>

    <!-- Footer -->

    <footer class="w3-container w3-padding-64 w3-center w3-opacity w3-light-grey w3-xlarge">
      <p class="w3-medium"><a href="https://laboratoirehubertcurien.univ-st-etienne.fr/en/index.html" target="_blank">Laboratoire Hubert Curien</a></p>
    </footer>

<script src="propagation_res_controller.js"></script>

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

<script>
// Initiate zoom effect:
imageZoom("result", "myresult");
</script>



</body>
</html>
