clear all;
close all;
clc;

addpath(genpath('./'));

%Nombre de répétitions
repetition = 100;

hologramme = double ( imread('./data/2019_06_07_billes1mu_63_JOSAA/Basler daA1920-30um (22030948)_20190607_122251706_0012_crop.tiff') );

hologramme = sqrt(hologramme);

taille_initiale = size(hologramme);
taille_finale = [2*taille_initiale(1), 2*taille_initiale(2)];

moyenne = median(hologramme(:));
hologramme = hologramme/moyenne;

figure, imagesc(hologramme), colorbar, colormap(gray), title('Hologramme initial');

hologramme_optimal = ones( taille_finale(1), taille_finale(2) ) ;
hologramme_optimal( taille_initiale(1)/2+1 : 3*taille_initiale(1)/2, taille_initiale(2)/2+1 : 3*taille_initiale(2)/2 ) = hologramme;
sqrt_holo = hologramme_optimal;

%Paramètres
z_s = 0.25*7.2822e-6 ;
mag = 56.7; 
lambda = 532e-9;
n_0 = 1.52; 
pixel_size = 2.2e-6 / mag; 
fov_width = taille_finale(1); 
fov_height = taille_finale(2); 

%On récupère les 2 kernel
[Hz, H_z] = getFresnelPropagator(fov_width, fov_height, pixel_size,lambda, n_0, z_s);

holo_temp = hologramme_optimal;

for i=1:repetition
    
    %Retro-propagation
    TF = fft2(ifftshift(holo_temp));
    produit_TF = TF .* H_z;
    TF_inverse = ifftshift(ifft2(produit_TF));
    
    %On recrée une image en passant le module à 1
    module1 = exp( 1i * max(angle(TF_inverse),0) );
    
    %Propagation
    TF = fft2(ifftshift(module1));
    produit_TF = TF .* Hz;
    TF_inverse = ifftshift(ifft2(produit_TF));
    
    %On remet les données initiales de l'hologramme
    holo_temp = sqrt_holo .* exp( 1i * angle(TF_inverse) );
    
end

%Recadrage et affichage de l'image
im_finale = module1( taille_initiale(1)/2+1 : 3*taille_initiale(1)/2, taille_initiale(2)/2+1 : 3*taille_initiale(2)/2 );
im_finale = angle(module1);
figure, imagesc(im_finale), colorbar, colormap(gray), title('Image finale');