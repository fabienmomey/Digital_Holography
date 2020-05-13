<?php
$file_url = "output/archives/".$_GET["value"].".tar";

if(file_exists("output/propagation/".$_GET["value"])) {
    exec("tar -cf ".$file_url." -C output/propagation ".$_GET["value"]);
    sleep(0.5);
    header('Content-Description: File Transfer');
    header('Content-Type: application/octet-stream');
    header("Content-disposition: attachment; filename=\"" . basename($file_url) . "\"");
    readfile($file_url);
    exit();
}
else
    echo "File not found";
