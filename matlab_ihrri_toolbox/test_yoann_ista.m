clear all;
close all;
clc;

addpath(genpath('./'));

repetition = 100;
hologramme = double ( imread('./data/2019_06_07_billes1mu_63_JOSAA/Basler daA1920-30um (22030948)_20190607_122251706_0012_crop.tiff') );

taille_initiale = size(hologramme);
taille_finale = [2*taille_initiale(1), 2*taille_initiale(2)];

moyenne = median(hologramme(:));
hologramme = hologramme/moyenne;
hologramme_real = ones( taille_finale(1), taille_finale(2) ) ;
hologramme_real( taille_initiale(1)/2+1 : 3*taille_initiale(1)/2, taille_initiale(2)/2+1 : 3*taille_initiale(2)/2 ) = hologramme;

%Paramètres
z_s = 7.2822e-6 ;
mag = 56.7; 
lambda = 532e-9;
n_0 = 1.52; 
pixel_size = 2.2e-6 / mag; 
fov_width = taille_finale(1); 
fov_height = taille_finale(2); 

%Kernels
[Gz] = getFresnelPropagator(fov_width, fov_height, pixel_size,lambda, n_0, z_s, false, 'intensity', true);

%Gz = real(Hz);
figure, imagesc(Gz), colorbar, colormap(gray), title('Gz');

holo_iteration = zeros(taille_finale(1), taille_finale(2));
t=1/(2*max(Gz(:).*Gz(:))); %0.01;
mu=1;

for i=1:repetition
    
    TF = fft2(holo_iteration);
    %TF = real(TF);
    produit_TF = TF .* Gz;
    TF_inverse = ifft2(produit_TF);
    TF_inverse = real(TF_inverse);
    
    m_tild = 1 + TF_inverse;
    
    c = sum(sum( m_tild .* hologramme_real)) / sum(sum(m_tild .* m_tild));
    
    r = c*m_tild - hologramme_real;
    
    TF = fft2(r);
    %TF = real(TF);
    produit_TF = TF .* Gz;
    TF_inverse = ifft2(produit_TF);
    r = real(TF_inverse);
    
    holo_iteration = holo_iteration - 2*t*c*r;
    
    %holo_iteration = max(0.0, holo_iteration - mu*t);
    holo_iteration = sign(holo_iteration).*max(0.0, abs(holo_iteration) - mu*t);
    
    %holo_iteration = min(1.57, holo_iteration);
end

im_finale = holo_iteration( taille_initiale(1)/2+1 : 3*taille_initiale(1)/2, taille_initiale(2)/2+1 : 3*taille_initiale(2)/2 );
figure, imagesc(im_finale), colorbar, colormap(gray), title('Image finale');