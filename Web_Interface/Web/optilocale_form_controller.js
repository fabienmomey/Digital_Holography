

let canvas = document.getElementById("preview");
let context = canvas.getContext("2d");
let image = undefined;
let widthRatio = 1;
let heightRatio = 1;
let posX = 0;
let posY = 0;


let xDoc = document.getElementById('x_input');
let yDoc = document.getElementById('y_input');
let zDoc = document.getElementById('z_input');
let rDoc = document.getElementById('r_input');
let n1Doc = document.getElementById('n1_input');
let n1iDoc = document.getElementById('n1i_input');
let lambdaDoc = document.getElementById('lambda_input');
let pixDoc = document.getElementById('pix_input');
let n0Doc = document.getElementById('n0_input');
let xeDoc = document.getElementById('xe_input');
let yeDoc = document.getElementById('ye_input');
let nDoc = document.getElementById('n_input');

context.font = "30px Arial";
context.textAlign = "center";
context.fillText("Preview",canvas.width/2, canvas.height/2);


// Affichage de l'image et du pointeur
function draw(pos) {
    context.clearRect(0, 0, canvas.width, canvas.height);

    if(image !== undefined) {
        context.drawImage(image, posX, posY, image.naturalWidth*widthRatio, image.naturalHeight*heightRatio);
    }
    else {
        context.fillStyle = "#000000";
        context.fillText("Preview",canvas.width/2, canvas.height/2);
    }
    context.fillStyle = "#ff0000";
    context.fillRect (pos.x, pos.y, 4,4);
}

// Actualisation de la position du pointeur
canvas.onmousedown = function(e) {
    let pos = getMousePos(canvas, e);
    draw(pos);

    if(image !== undefined) {
        xDoc.value = ((pos.x - posX) / widthRatio).toFixed(0);
        yDoc.value = ((pos.y - posY) / heightRatio).toFixed(0);

        check(xDoc);
        check(yDoc);
    }

};

// Conversion des coordonnées du canvas aux coordonnées de l'image
xDoc.onkeyup = function() {
    let newPos = {x:0,y:0};
    newPos.x = this.value * widthRatio + posX;
    newPos.y = yDoc.value * heightRatio + posY;

    console.log(newPos);

    draw(newPos);
    check(this);
};

yDoc.onkeyup = function() {
    let newPos = {x:0,y:0};
    newPos.y = this.value * heightRatio + posY;
    newPos.x = xDoc.value * widthRatio + posX;

    console.log(newPos);

    draw(newPos);
    check(this);
};

zDoc.onkeyup = function() {
    check(this);
};

rDoc.onkeyup = function() {
    check(this);
};

n1Doc.onkeyup = function() {
    check(this);
};

n1iDoc.onkeyup = function() {
    check(this);
};

lambdaDoc.onkeyup = function() {
    check(this);
};

pixDoc.onkeyup = function() {
    check(this);
};

n0Doc.onkeyup = function() {
    check(this);
};

xeDoc.onkeyup = function() {
    check(this);
};

yeDoc.onkeyup = function() {
    check(this);
};

nDoc.onkeyup = function() {
    check(this);
};

function checkall() {
    let isValid = true;

    isValid = check(xDoc) && isValid;
    isValid = check(yDoc) && isValid;
    isValid = check(zDoc) && isValid;
    isValid = check(rDoc) && isValid;
    isValid = check(n1Doc) && isValid;
    isValid = check(n1iDoc) && isValid;
    isValid = check(lambdaDoc) && isValid;
    isValid = check(pixDoc) && isValid;
    isValid = check(n0Doc) && isValid;
    isValid = check(xeDoc) && isValid;
    isValid = check(yeDoc) && isValid;
    isValid = check(nDoc) && isValid;

    return isValid;
}

function readTextFile(file)
{
    let rawFile = new XMLHttpRequest();
    rawFile.open("GET", file, false);
    rawFile.onreadystatechange = function ()
    {
        if(rawFile.readyState === 4)
        {
            if(rawFile.status === 200 || rawFile.status == 0)
            {
                let allText = rawFile.responseText;

                allText = allText.replace(/\n/g, '<br>');
                document.getElementById('popup').innerHTML = allText;
            }
        }
    };
    rawFile.send(null);
}

function makeid(length) {
    let result           = '';
    let characters       = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    let charactersLength = characters.length;
    for (let i = 0; i < length; i++) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return result;
}

// Evènement : clic sur le boutton "Submit"
document.getElementById('submitButton').onclick = function() {
    if(checkall()) {
        // Création de l'id
        let id = makeid(20);
        document.getElementById('popup').innerHTML = '<span class="spinner-border spinner-border-sm" role="status" aria-hidden="true"></span> Loading...';
        document.getElementById('sessionID').value = id;
        document.getElementById('alert_zone').innerHTML = '';

        //Préparation de la requête
        let httpRequest = new XMLHttpRequest();
        // Comportement à la reception de la réponse du serveur
        httpRequest.onreadystatechange = function() {
            if(httpRequest.readyState === XMLHttpRequest.DONE) {
                if(httpRequest.status === 200) {
                    readTextFile('output/optilocale/'+id+'.txt');
                }
            }
        };
        httpRequest.open('POST', 'optilocale_results.php', true);
        httpRequest.send(new FormData(document.getElementById('optilocaleForm')));
    }
    else {
        document.getElementById('alert_zone').innerHTML = '<div class="alert alert-danger" role="alert">Invalid argument</div>';
    }

};

// Chargement et redimenssionnement de l'image
document.getElementById('fileToUpload').onchange = function() {
    document.getElementById('presetFile').value = '';
    document.getElementById('fileLabel').innerText = this.files.item(0).name;
    image = document.createElement('img');
    image.src = window.URL.createObjectURL(this.files.item(0));
    image.onload = function() {
        if(image.naturalWidth > canvas.width || image.naturalHeight > canvas.height) {
            if(image.naturalWidth > image.naturalHeight) {
                widthRatio = canvas.width/image.naturalWidth;
                heightRatio = widthRatio;
            }
            else {
                heightRatio = canvas.height/image.naturalHeight;
                widthRatio = heightRatio;
            }
        }
        else {
            widthRatio = heightRatio = 1;
        }

        posX = (canvas.width - image.naturalWidth*widthRatio)/2;
        posY = (canvas.height - image.naturalHeight*heightRatio)/2;

        context.clearRect(0, 0, canvas.width, canvas.height);
        context.drawImage(image, posX, posY, image.naturalWidth*widthRatio, image.naturalHeight*heightRatio);
    };
};

function loadPreset(fname) {
    let filename = "preset/"+fname;
    document.getElementById('presetFile').value = filename;
    document.getElementById('fileLabel').innerText = filename;
    image = new Image();
    image.src = "uploads/"+filename;
    image.onload = function() {
        if(image.naturalWidth > canvas.width || image.naturalHeight > canvas.height) {
            if(image.naturalWidth > image.naturalHeight) {
                widthRatio = canvas.width/image.naturalWidth;
                heightRatio = widthRatio;
            }
            else {
                heightRatio = canvas.height/image.naturalHeight;
                widthRatio = heightRatio;
            }
        }
        else {
            widthRatio = heightRatio = 1;
        }

        posX = (canvas.width - image.naturalWidth*widthRatio)/2;
        posY = (canvas.height - image.naturalHeight*heightRatio)/2;

        context.clearRect(0, 0, canvas.width, canvas.height);
        context.drawImage(image, posX, posY, image.naturalWidth*widthRatio, image.naturalHeight*heightRatio);
    };
}

document.getElementById('preset1').onchange = function() {
    loadPreset('Basler.png');
    zDoc.value = "1.0017e-5";
    rDoc.value = "5.5993e-7";
    n1Doc.value = "1.6348";
    n1iDoc.value = "0";
    lambdaDoc.value = "532e-9";
    pixDoc.value = "3.88e-8";
    n0Doc.value = "1.5193";
    xeDoc.value = "0";
    yeDoc.value = "0";
    nDoc.value = "10800";

    checkall();
};



