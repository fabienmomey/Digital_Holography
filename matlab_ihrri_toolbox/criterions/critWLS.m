function [fx,gx,varargout] = critWLS(x,y,Hz,H_z,varargin)
% [fx,gx,varargout] = critWLS(x,y,Hz,H_z,varargin)
%
%   This function computes the cost and gradient in X of a Weighted Least
%   Squares (WLS) criterion with a nonlinear model of intensity
%   measurements:
%
%                  F(X)    = || C*M(X) - Y ||_W^2
%
%                          = || C*|1+H.X|^2 - Y ||_W^2
%
%   The gradient G(X) writes:
%                                        /                               \
%                                        |           /                 \ |
%                  G(X)    = 4 * C * H^T | (1+H.X) . | C*|1+H.X|^2 - Y | |
%                                        |           \                 / |
%                                        \                               /
%
%   X: current image guess => a 2-component image ([width,height,2]), each
%   one corresponding respectively to the real and imaginary part of the
%   complex deviation from the unit transmittance plane)
%
%   Y: data image (square root of intensity measurements).
%
%   Hz: function handle to perform the propagation operator  (see 
%       getFresnelPropagation and propagationOperator functions).
%   H_z: function handle to perform the backpropagation operator (see 
%       getFresnelPropagation and propagationOperator functions).
%
%   In VARGARIN, 3 parameters can be given:
%   - C:    a constant scaling factor that accounts for the intensity of 
%           the incident wave |a_0|^2 as well as the detector gain and 
%           quantum efficiency [2]. If not set or <0, the scaling factor
%           can be computed "on-the-fly" (and returned in VARARGOUT):
%
%           C = <M(X),Y> / <M(X),M(X)>
%
%           where <A,B> stands for the scalar product of vectors A and B.
%
%   - W:    diagonal elements of the inverse noise covariance matrix C^{-1}
%           => under hypothesis of uncorrelated noise [2]. 
%
%   In VARGAROUT, 2 additional parameters can be extracted:
%   - [fx,gx,c] = critWLS(x,y,Hz,H_z,c) if c < 0 or c not set
%   - [fx,gx,residues] = critWLS(x,y,Hz,H_z,c) if c > 0
%   - [fx,gx,c,residues] = critWLS(x,y,Hz,H_z,c)
%
% References
%
% - [1] J. Fienup, “Phase retrieval algorithms: a comparison,”
%                   Applied optics, vol. 21, no. 15, pp. 2758–2769, 1982.
% - [2] F. Momey, L. Denis, T. Olivier, C. Fournier, "From Fienup’s phase 
%                   retrieval techniques to regularized inversion for 
%                   in-line holography: tutorial," JOSA A, vol. 36, no. 12, 
%                   D62-D80, 2019. 
%
% Created: 06/10/2020 (mm/dd/yyyy)
% Author:   Fabien Momey
%           Laboratoire Hubert Curien UMR CNRS 5516,
%           Université Jean Monnet,
%           F-42000 Saint-Étienne,
%           France
%           fabien.momey@univ-st-etienne.fr
%

%% Extract size (in pixels) of the field of view
[npix_W, npix_H, ncomp] = size(x);

%% Criterion parameters
flag_c = true;
if (nargin>4)
    c = varargin{1};
    if (c > 0)
        flag_c = false;
    end
end

flag_w = false;
if (nargin>5)
    w = varargin{2};
    flag_w = true;
end

%% Forward propagation
uopt = Hz(x(:,:,1)+1i*x(:,:,2))+1.0;
Iopt = abs(uopt).^2;
     
%% Calculate optimal scaling parameter c
if (flag_c && ~flag_w)
    c = sum(Iopt(:).*y(:))/sum(Iopt(:).*Iopt(:)) ;
elseif (flag_c && flag_w)
    c = sum(Iopt(:).*w(:).*y(:))/sum(Iopt(:).*w(:).*Iopt(:)) ;
end

%% Cost
cIopt_y = c*Iopt-y;
if (flag_w)
    cIopt_y = w.*cIopt_y;
end
fx = sum(cIopt_y(:).^2);

%% Gradient
gxcplx = 4*c*H_z((uopt.*(cIopt_y)));

gx = zeros(npix_W,npix_H,2);
gx(:,:,1) = real(gxcplx);
gx(:,:,2) = imag(gxcplx);

if (nargout > 2)
    if (nargout > 3)
        varargout{1} = c;
        varargout{2} = cIopt_y;
    else
        if (flag_c)
            varargout{1} = c;
        else
            varargout{1} = cIopt_y;
        end
    end
end

end