function checkValidFloat(str) {
    let pattern = /^[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?$/;
    return pattern.test(str);
}

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

function getMousePos(canvas, evt) {
    let rect = canvas.getBoundingClientRect();
    return {
        x: evt.clientX - rect.left,
        y: evt.clientY - rect.top
    };
}