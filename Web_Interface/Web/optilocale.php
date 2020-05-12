<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Digital Holography Reconstruction -  Local Optimisation</title>
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
    <link rel="stylesheet" href="style.css">
</head>
<body>

<div>
    <h1>Local optimization</h1>
    <h2>Presets</h2>
    <p>
    <div class="btn-group btn-group-toggle" data-toggle="buttons">
        <label class="btn btn-outline-primary btn-sm">
            <input type="radio" name="options" id="preset1" autocomplete="off"> Preset 1
        </label>
        <label class="btn btn-outline-primary btn-sm">
            <input type="radio" name="options" id="preset2" autocomplete="off"> Preset 2
        </label>
        <label class="btn btn-outline-primary btn-sm">
            <input type="radio" name="options" id="preset3" autocomplete="off"> Preset 3
        </label>
    </div>

    </p>

    <h2>Parameters</h2>
    <form action="optilocale_results.php" method="post" enctype="multipart/form-data" id="optilocaleForm" name="optilocaleForm">
        <div class="form-group">
            <label for="fileToUpload">Select an image (jpg or png): </label>
            <div class="custom-file">
                <input type="file" class="custom-file-input" id="fileToUpload" name="fileToUpload" accept="image/*">
                <label class="custom-file-label" for="customFile" id="fileLabel">Select an image</label>
            </div>
            <input id="presetFile" name="presetFile" type="hidden">
            <input id="sessionID" name="sessionID" type="hidden">
        </div>
        <h4>Starting point</h4>
        <div class="form-group row">
            <label for="start_input" class="col-sm-2 col-form-label">Estimated x (pix, can be clicked on the image preview):</label>
            <div class="col-sm-10">
                <input type="text" class="form-control" id="x_input" name="x_input">
            </div>
        </div>
        <div class="form-group row">
            <label for="end_input" class="col-sm-2 col-form-label">Estimated y (pix, can be clicked on the image preview):</label>
            <div class="col-sm-10">
                <input type="text" class="form-control" id="y_input" name="y_input">
            </div>
        </div>
        <div class="form-group row">
            <label for="step_input" class="col-sm-2 col-form-label">Estimated z (m):</label>
            <div class="col-sm-10">
                <input type="text" class="form-control" id="z_input" name="z_input">
            </div>
        </div>
        <div class="form-group row">
            <label for="lambda_input" class="col-sm-2 col-form-label">Estimated radius (m):</label>
            <div class="col-sm-10">
                <input type="text" class="form-control" id="r_input" name="r_input">
            </div>
        </div>
        <div class="form-group row">
            <label for="n1_input" class="col-sm-2 col-form-label">Estimated refractive index (Re):</label>
            <div class="col-sm-10">
                <input type="text" class="form-control" id="n1_input" name="n1_input">
            </div>
        </div>
        <div class="form-group row">
            <label for="n1i_input" class="col-sm-2 col-form-label">Estimated refractive index (Im):</label>
            <div class="col-sm-10">
                <input type="text" class="form-control" id="n1i_input" name="n1i_input">
            </div>
        </div>

        <h4>General parameters</h4>
        <div class="form-group row">
            <label for="lambda_input" class="col-sm-2 col-form-label">Lambda (m):</label>
            <div class="col-sm-10">
                <input type="text" class="form-control" id="lambda_input" name="lambda_input">
            </div>
        </div>
        <div class="form-group row">
            <label for="pix_input" class="col-sm-2 col-form-label">Pixel size (m):</label>
            <div class="col-sm-10">
                <input type="text" class="form-control" id="pix_input" name="pix_input">
            </div>
        </div>
        <div class="form-group row">
            <label for="n0_input" class="col-sm-2 col-form-label">n0:</label>
            <div class="col-sm-10">
                <input type="text" class="form-control" id="n0_input" name="n0_input">
            </div>
        </div>
        <div class="form-group row">
            <label for="xe_input" class="col-sm-2 col-form-label">xe (m):</label>
            <div class="col-sm-10">
                <input type="text" class="form-control" id="xe_input" name="xe_input">
            </div>
        </div>
        <div class="form-group row">
            <label for="ye_input" class="col-sm-2 col-form-label">ye (m):</label>
            <div class="col-sm-10">
                <input type="text" class="form-control" id="ye_input" name="ye_input">
            </div>
        </div>
        <div class="form-group row">
            <label for="n_input" class="col-sm-2 col-form-label">N:</label>
            <div class="col-sm-10">
                <input type="text" class="form-control" id="n_input" name="n_input">
            </div>
        </div>

        <h2>Image</h2>
        <div id="optilocal_display">
            <canvas id="preview" width="800" height="800" style="border:1px solid #000000;"></canvas>
            <div id="popup"></div>
        </div>
        <br>
        <span id="alert_zone"></span>
        <button type="button" name="submitButton" id="submitButton" class="btn btn-primary">Upload Image</button>
    </form>
    <br>
    <p><a href="index.php">Back</a></p>


</div>
<script src="utilities.js"></script>
<script src="optilocale_form_controller.js"></script>
</body>