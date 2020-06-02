<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8" name="viewport" content="width=device-width, initial-scale=1">
    <title>Digital Holography Reconstruction - Fienup</title>
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
        <a class="w3-bar-item w3-button w3-black" href="fienup.php">Fienup</a>
        <a class="w3-bar-item w3-button" href="history.php">History</a>
    </nav>

    <div class="w3-overlay w3-hide-large w3-animate-opacity" onclick="w3_close()" style="cursor:pointer" id="myOverlay"></div>

    <div class="w3-main" style="margin-left:250px;">

        <div id="myTop" class="w3-container w3-top w3-theme w3-large">
        <p><i class="fa fa-bars w3-button w3-black w3-hide-large w3-xlarge" onclick="w3_open()"></i>
        <span id="myIntro" class="w3-hide">Fienup</span></p>
        </div>

        <header class="w3-container w3-theme" style="padding:64px 32px">
        <h1 class="w3-xxxlarge">Fienup</h1>
        </header>

    <!-- Contenue Page -->

    <div class="w3-container" style="padding:32px">

        <div>
            <h2>Presets</h2>
            <p>
                <div class="btn-group btn-group-toggle" data-toggle="buttons">
                    <label class="btn btn-outline-dark btn-sm">
                        <input type="radio" name="options" id="preset1" autocomplete="off"> Preset 1
                    </label>
                    <label class="btn btn-outline-dark btn-sm">
                        <input type="radio" name="options" id="preset2" autocomplete="off"> Preset 2
                    </label>
                    <label class="btn btn-outline-dark btn-sm">
                        <input type="radio" name="options" id="preset3" autocomplete="off"> Preset 3
                    </label>
                    <label class="btn btn-outline-dark btn-sm">
                        <input type="radio" name="options" id="preset4" autocomplete="off"> Preset 4
                    </label>
                </div>

            </p>

            <h2>Parameters</h2>
                <span id="alert_zone"></span>
                <form action="fienup_results.php" method="post" enctype="multipart/form-data" id="retropropagForm" name="retropropagForm">
                    <div class="form-group">
                        <label for="fileToUpload">Select an image (jpg or png): </label>
                        <div class="custom-file">
                            <input type="file" class="custom-file-input" id="fileToUpload" name="fileToUpload" accept="image/*">
                            <label class="custom-file-label" for="customFile" id="fileLabel">Select an image</label>
                        </div>
                    </div>
                    <input id="presetFile" name="presetFile" type="hidden">
                    <span id="preview"></span>
                    <div class="form-group row">
                        <label for="start_input" class="col-sm-2 col-form-label">Starting depth (m):</label>
                        <div class="col-sm-10">
                            <input type="text" class="form-control" id="start_input" name="start_input">
                        </div>
                    </div>
                    <div class="form-group row">
                        <label for="end_input" class="col-sm-2 col-form-label">Ending depth (m):</label>
                        <div class="col-sm-10">
                            <input type="text" class="form-control" id="end_input" name="end_input">
                        </div>
                    </div>
                    <div class="form-group row">
                        <label for="step_input" class="col-sm-2 col-form-label">Step (m):</label>
                        <div class="col-sm-10">
                            <input type="text" class="form-control" id="step_input" name="step_input">
                        </div>
                    </div>
                    <div class="form-group row">
                        <label for="lambda_input" class="col-sm-2 col-form-label">Wavelength (m):</label>
                        <div class="col-sm-10">
                            <input type="text" class="form-control" id="lambda_input" name="lambda_input">
                        </div>
                    </div>
                    <div class="form-group row">
                        <label for="pp_input" class="col-sm-2 col-form-label">Pixel size (m):</label>
                        <div class="col-sm-10">
                            <input type="text" class="form-control" id="pp_input" name="pp_input">
                        </div>
                    </div>

                    <fieldset class="form-group">
                        <div class="row">
                            <legend class="col-form-label col-sm-2 pt-0">Zero-padding:</legend>
                            <div class="col-sm-10">
                                <div class="form-check">
                                    <input class="form-check-input" type="radio" name="zeropad_radio" id="gridRadios1" value="nopad">
                                    <label class="form-check-label" for="gridRadios1">
                                        No padding
                                    </label>
                                </div>
                                <div class="form-check">
                                    <input class="form-check-input" type="radio" name="zeropad_radio" id="gridRadios2" value="croppedpad" checked>
                                    <label class="form-check-label" for="gridRadios2">
                                        Cropped padding
                                    </label>
                                </div>
                                <div class="form-check disabled">
                                    <input class="form-check-input" type="radio" name="zeropad_radio" id="gridRadios3" value="fullpad">
                                    <label class="form-check-label" for="gridRadios3">
                                        Full padding
                                    </label>
                                </div>
                            </div>
                        </div>
                    </fieldset>


                    <button type="button" name="submitButton" id="submitButton" class="btn btn-dark">Upload Image</button>
                </form>
                <br>
                <p><button class="w3-button w3-black w3-round-xxlarge"><a href="index.php">Back</a></button></p>
        </div>
    </div>

     <!-- Footer -->

     <footer class="w3-container w3-padding-64 w3-center w3-opacity w3-light-grey w3-xlarge">
      <p class="w3-medium"><a href="https://laboratoirehubertcurien.univ-st-etienne.fr/en/index.html" target="_blank">Laboratoire Hubert Curien</a></p>
    </footer>

    <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
    <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
    <script src="utilities.js"></script>
    <script src="propagation_form_controller.js"></script>

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