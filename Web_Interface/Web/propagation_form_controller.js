//Vérifie si la chaine de caratères en entrée correspond à un flottant
function checkValidFloat(str) {
    let pattern = /^[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?$/;
    return pattern.test(str);
}

//Vérifie la validité de la valeur de l'objet et met en page en fonction du résultat
function check(obj){
    if(checkValidFloat(obj.value)) {
        obj.classList.remove('is-invalid');
        obj.classList.add('is-valid');
        return true;
    }
    else {
        obj.classList.remove('is-valid');
        obj.classList.add('is-invalid');
        return false;
    }
}

let startDoc = document.getElementById('start_input');
let endDoc = document.getElementById('end_input');
let stepDoc = document.getElementById('step_input');
let lambdaDoc = document.getElementById('lambda_input');
let ppDoc = document.getElementById('pp_input');


// Lorsque l'on appuie sur une touche, on désélectionne les bouttons qui sélectionne le préset

startDoc.onkeyup = function(){
    check(this);
    document.getElementById('preset1').parentElement.classList.remove('active');
    document.getElementById('preset2').parentElement.classList.remove('active');
    document.getElementById('preset3').parentElement.classList.remove('active');
};

endDoc.onkeyup = function(){
    check(this);
    document.getElementById('preset1').parentElement.classList.remove('active');
    document.getElementById('preset2').parentElement.classList.remove('active');
    document.getElementById('preset3').parentElement.classList.remove('active');
};

stepDoc.onkeyup = function(){
    check(this);
    document.getElementById('preset1').parentElement.classList.remove('active');
    document.getElementById('preset2').parentElement.classList.remove('active');
    document.getElementById('preset3').parentElement.classList.remove('active');
};

lambdaDoc.onkeyup = function(){
    check(this);
    document.getElementById('preset1').parentElement.classList.remove('active');
    document.getElementById('preset2').parentElement.classList.remove('active');
    document.getElementById('preset3').parentElement.classList.remove('active');
};

ppDoc.onkeyup = function(){
    check(this);
    document.getElementById('preset1').parentElement.classList.remove('active');
    document.getElementById('preset2').parentElement.classList.remove('active');
    document.getElementById('preset3').parentElement.classList.remove('active');
};

// Evènement : lorsque l'on valide le formulaire
document.getElementById('submitButton').onclick = function() {
    document.getElementById('alert_zone').innerHTML = '';

    // Vérification
    if(document.getElementById('fileToUpload').value === '' && document.getElementById('presetFile').value === '') {
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Please select an image!</div>'
        return 0;
    }

    if(startDoc.value === ""){
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Starting depth is missing!</div>'
        return 0;
    }
    if(!check((startDoc))) {
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Starting depth is not valid!</div>'
        return 0;
    }

    if(endDoc.value === ""){
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Ending depth is missing!</div>'
        return 0;
    }
    if(!check((endDoc))) {
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Ending depth is not valid</div>'
        return 0;
    }

    if(stepDoc.value === ""){
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Step is missing!</div>'
        return 0;
    }
    if(!check((stepDoc))) {
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Step is not valid!</div>'
        return 0;
    }

    if(lambdaDoc.value === ""){
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Wavelength is missing!</div>'
        return 0;
    }
    if(!check((lambdaDoc))) {
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Wavelength is not valid!</div>'
        return 0;
    }

    if(ppDoc.value === ""){
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Pixel size is missing!</div>'
        return 0;
    }
    if(!check((ppDoc))) {
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Pixel size is not valid!</div>'
        return 0;
    }

    if(parseFloat(endDoc.value) <= parseFloat(startDoc.value)) {
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Ending depth is greater than starting depth!</div>'
        return 0;
    }

    if(parseFloat(stepDoc.value) > (parseFloat(endDoc.value) - parseFloat(stepDoc.value))) {
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Step is to big!</div>'
        return 0;
    }

    if((parseFloat(endDoc.value) - parseFloat(startDoc.value))/parseFloat(stepDoc.value) > 30) {
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">There are too many steps (>30) !</div>'
        return 0;
    }

    // Affichage de la roue qui tourne (Loading)
    document.getElementById('submitButton').innerHTML = '<span class="spinner-border spinner-border-sm" role="status" aria-hidden="true"></span> Loading...';
    document.getElementById('retropropagForm').submit();
};

// Evènement : sélection d'un fichier
document.getElementById('fileToUpload').onchange = function() {
    // On indique qu'on ne sélectionne pas une image pré-enregistrée
    document.getElementById('presetFile').value = '';
    // Affichage du nom dans la barre de saisie
    document.getElementById('fileLabel').innerText = this.files.item(0).name;
    // Preview
    let image = document.createElement('img');
    image.src = window.URL.createObjectURL(this.files.item(0));
    image.style.maxWidth = '10%';
    document.getElementById('preview').innerHTML = '';
    document.getElementById('preview').appendChild(image);
};

// Evenement : click du preset 1
document.getElementById('preset1').onchange = function() {
    document.getElementById('fileLabel').innerText = 'Reticle_LaHC.png';
    document.getElementById('presetFile').value = 'preset/Reticle_LaHC.png';
    document.getElementById('preview').innerHTML = '<img src="uploads/preset/Reticle_LaHC.png" style="max-width:10%;">';
    startDoc.value = "240e-3";
    endDoc.value = "340e-3";
    stepDoc.value = "5e-3";
    lambdaDoc.value = "0.532e-6";
    ppDoc.value = "7e-6";
};

// Evenement : click du preset 2
document.getElementById('preset2').onchange = function() {
    document.getElementById('fileLabel').innerText = 'HoloTestInj6.png';
    document.getElementById('presetFile').value = 'preset/HoloTestInj6.png';
    document.getElementById('preview').innerHTML = '<img src="uploads/preset/HoloTestInj6.png" style="max-width:10%;">';
    startDoc.value = "0.23";
    endDoc.value = "0.33";
    stepDoc.value = "0.01";
    lambdaDoc.value = "0.532e-6";
    ppDoc.value = "7e-6";
};

// Evenement : click du preset 3
document.getElementById('preset3').onchange = function() {
    document.getElementById('fileLabel').innerText = '???.png';
    document.getElementById('presetFile').value = 'preset/pika_holo.png';
    document.getElementById('preview').innerHTML = '<img src="uploads/preset/pika_holo.png" style="max-width:10%;">';
    startDoc.value = "2.1e-5";
    endDoc.value = "5e-5";
    stepDoc.value = "1e-6";
    lambdaDoc.value = "0.6e-6";
    ppDoc.value = "4.4e-8";
};

// Evenement : click du preset 4
document.getElementById('preset4').onchange = function() {
    document.getElementById('fileLabel').innerText = 'hello_world.png';
    document.getElementById('presetFile').value = 'preset/hello_world.png';
    document.getElementById('preview').innerHTML = '<img src="uploads/preset/hello_world.png" style="max-width:10%;">';
    startDoc.value = "1e-5";
    endDoc.value = "10e-5";
    stepDoc.value = "0.3e-5";
    lambdaDoc.value = "0.6e-6";
    ppDoc.value = "4.4e-8";
};