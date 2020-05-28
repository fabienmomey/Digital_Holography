let filename = document.getElementById('destname').innerText;
let startz = parseFloat(document.getElementById('startz').innerText);
let endz = parseFloat(document.getElementById('endz').innerText);
let stepz = parseFloat(document.getElementById('stepz').innerText);

document.getElementById('result').setAttribute('src', 'output/propagation/' + filename + '/res0.png');

let z_slider = document.getElementById('z_slider');
var slider_zoom = document.getElementById('slider_zoom');
var zoom_value = document.getElementById('zoomi')

let nb = (endz-startz)/stepz;
z_slider.setAttribute("max", nb.toString());

document.getElementById('depth').innerText = '(' + startz + ' m)';

// Evènement : lorque le z_slider est bougé
z_slider.oninput = function() {
    document.getElementById('result').setAttribute('src', 'output/propagation/' + filename + '/res' + this.value + '.png');
    document.getElementById('depth').innerText = '(' + (startz + this.value*stepz) + ' m)';
    // Initiate zoom effect:
imageZoom("result", "myresult");
};

// Evènement : lorque le slider_zoom est bougé
slider_zoom.oninput = function() {
    zoom_value.innerHTML = "" + this.value + "px";
};