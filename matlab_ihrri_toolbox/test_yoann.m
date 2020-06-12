clear all;
close all;
clc;

addpath(genpath('./'));

%% Experimental parameters
z_s = 12.5e-06;
mag = 56.7; 
lambda = 532e-9;
n_0 = 1.52; 
pixel_size = 4.4e-6 / mag; 

%% Hologramme complexe
% holo_real = double(imread('./data/Bead_simulations/real_cplx_hologram_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.tif'));
% holo_imag = double(imread('./data/Bead_simulations/imag_cplx_hologram_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.tif'));
% hologramme = double(holo_real+1i.*holo_imag);
hologramme = fitsread('./data/Bead_simulations/cplx_hologram_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.fits');
hologramme = hologramme(:,:,1)+1i*hologramme(:,:,2);
% hologramme = hologramme.*exp(2*1i*pi*(n_0/lambda)*z_s);
hologramme = hologramme(257:768,257:768);
%% Hologramme intensité
% hologramme = double(imread('./data/Bead_simulations/data_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.tif'));
% hologramme = sqrt(hologramme);

figure, imagesc(abs(hologramme)), colorbar, colormap(gray), title('hologramme');

taille_initiale = size(hologramme);
taille_finale = [2*taille_initiale(1), 2*taille_initiale(2)];

hologramme_optimal = ones(taille_finale(1), taille_finale(2));
%hologramme_optimale(taille_initiale(1)/2:3*taille_initiale(1)/2, taille_initiale(2)/2:3*taille_initiale(2)/2) = hologramme(:,:);

hologramme_optimal (floor(taille_initiale(1)/2)+1:floor(taille_initiale(1)/2)+taille_initiale(1),...
    floor(taille_initiale(2)/2)+1:floor(taille_initiale(2)/2)+taille_initiale(2)) = hologramme;

figure, imagesc(abs(hologramme_optimal)), colorbar, colormap(gray), title('padding');

fov_width = taille_finale(1); 
fov_height = taille_finale(2); 

[Hz, H_z] = getFresnelPropagator(fov_width, fov_height, pixel_size,lambda, n_0, z_s);

fourier_hologramme = fft2( double(hologramme_optimal) );
produit_TF = fourier_hologramme .* H_z;
img_reconstituee = ifft2(produit_TF);
%img_reconstituee = abs(img_reconstituee);

image_finale = img_reconstituee(floor(taille_initiale(1)/2)+1:floor(taille_initiale(1)/2)+taille_initiale(1),...
    floor(taille_initiale(2)/2)+1:floor(taille_initiale(2)/2)+taille_initiale(2));
figure, imagesc(abs(image_finale)), colorbar, colormap(gray), title('Image reconstituee module');
figure, imagesc(angle(image_finale)), colorbar, colormap(gray), title('Image reconstituee phase');

gtruth = fitsread('./data/Bead_simulations/ground_truth_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.fits');
gtruth = 1.0 + gtruth(:,:,1) + 1i*gtruth(:,:,2);
figure, imagesc(abs(gtruth)), colorbar, colormap(gray), title('Image originale module');
figure, imagesc(angle(gtruth)), colorbar, colormap(gray), title('Image originale phase');

