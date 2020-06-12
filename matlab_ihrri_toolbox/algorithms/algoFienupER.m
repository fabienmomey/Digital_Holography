function [xopt,evolcost,varargout] = algoFienupER(x0,y,Hz,H_z,options)
% [xopt,varargout] = algoFienupER(x0,y,Hz,H_z,options)
%
%   This function performs the Fienup's Error-Reduction algorithm [1]
%   for phase retrieval from intensity measurements from a transmittance
%   plane zA propagated to a plane zB.
%
%   X0: initial image guess (complex deviation from the unit transmittance 
%       plane).
%
%   Y: data image (square root of intensity measurements).
%
%   Hz: function handle to perform the propagation operator  (see 
%       getFresnelPropagation and propagationOperator functions).
%   H_z: function handle to perform the backpropagation operator (see 
%       getFresnelPropagation and propagationOperator functions).
%
%   OPTIONS: (structure of algorithm parameters)
%       * BETA       : convergence parameter for the input-output strategy
%                      (default: 1)
%                      If BETA=1, the Error-Reduction algorithm 
%                       (~ Gerchberg-Saxton [2]) is performed.
%       * TYPE_OBJ: 
%           > 'dephasing' (default): purely and weakly dephasing object.
%               => the unkown image X is real and represents the imaginary
%               part of the transmittance, which is approximately
%               considered as the targeted phase-shift image.
%           > 'absorbing': purely absorbing object.
%               => the unkown image  X is real and represents the opposite
%               of the opacity : X = T - 1.
%           > 'unknown' : mix object.
%               => the unkown image X is complex and represents the complex 
%               deviation from the unit transmittance plane.
%       * MU         :  if set, a soft-tresholding (prox L1) of parameter 
%                       MU will be performed.
%                       => if TYPE_OBJ is set to 'unkown', this
%                       soft-thresholding will be applied to both the real
%                       and imaginary parts separately.
%                       This corresponds to solving the following inverse 
%                       problem with L1 regularization : 
%                             f(x) = || |1+H.x| - y ||_2^2 + mu * || x ||_1
%                       at each iteration.
%       * RCONST = [XRMIN,XRMAX]: a 2-element vector giving hard constraint
%                          parameter for the real part of X
%                           \_ default: [-Inf,Inf]
%       * ICONST = [XIMIN,XIMAX]: a 2-element vector giving hard constraint
%                          parameter for the imag part of X
%                           \_ default: [-Inf,Inf]
%       * SUPPORT    : mask of a support constraint to be enforced.
%       * MAXITER    : maximum number of iterations  (default: 100) 
%       * FLAG_COST  : compute cost at each iteration
%       * XTRUE      : ground truth. If set, output information giving the
%                      evolution of the SNR and the cost can be computed.
%       * VERBOSE    : verbose mode (default: true)
%
% References
%
% - [1] J. Fienup, “Phase retrieval algorithms: a comparison,”
%                   Applied optics, vol. 21, no. 15, pp. 2758–2769, 1982.
% - [2] R. Gerchberg and W. Saxton, “A practical algorithm for the 
%                   determination of phase from image and diffraction 
%                   plane pictures,” Optik, vol. 35, p. 237, 1972.
%
% Created: 05/27/2020 (mm/dd/yyyy)
% Author:   Fabien Momey
%           Laboratoire Hubert Curien UMR CNRS 5516,
%           Université Jean Monnet,
%           F-42000 Saint-Étienne,
%           France
%           fabien.momey@univ-st-etienne.fr
%

%% Extract size (in pixels) of the field of view
[npix_W, npix_H] = size(y);

%% Extract reconstruction parameters

% BETA convergence relaxation parameter 
% -> if BETA = 1 (default) => Error-Reduction algorithm
% -> if BETA < 1 => Hybrid Input-Output (HIO) algorithm (see ref [1])
if (~isfield(options, 'beta'))
    options.beta = 1.0;
end
beta = options.beta;
if (beta==1)
     warning('BETA=1: Error-Reduction algorithm is performed.');
end

% MU hyperparameter for performing a soft-thresholding 
% (default: 0 => no soft-thresholding)
if (~isfield(options, 'mu'))
    options.mu = 0.0;
end
mu = options.mu;

% if (~isfield(options, 'gam'))
%     options.gam = 1.0;
% end
% gam = options.gam;

% Extracting hard constraints bounds
flag_rconst = true;
if (~isfield(options, 'rconst'))
    warning('RCONST not set: set to [-Inf,Inf] (no constraint).');
    options.rconst = [-Inf,Inf];
    flag_rconst = false;
else
    if (any(size(options.rconst)~=[1,2]) && any(size(options.rconst)~=[2,1]))
        error('RCONST must be a 2-scalar vector.');
        flag_rconst = false;
    end
end
rconst = options.rconst;

flag_iconst = true;
if (~isfield(options, 'iconst'))
    warning('ICONST not set: set to [-Inf,Inf] (no constraint).');
    options.iconst = [-Inf,Inf];
    flag_iconst = false;
else
    if (any(size(options.iconst)~=[1,2]) && any(size(options.iconst)~=[2,1]))
        error('ICONST must be a 2-scalar vector.');
        flag_iconst = false;
    end
end
iconst = options.iconst;

% Extract TYPE_OBJ and modify hard constraints bounds 
% if TYPE_OBJ is "dephasing" or "absorbing"
if (isfield(options, 'type_obj'))
    type_obj = options.type_obj;
    if (strcmp(type_obj,'dephasing'))
        options.rconst = [0.0,0.0];
        flag_rconst = true;
    elseif (strcmp(type_obj,'absorbing'))
        options.iconst = [0.0,0.0];
        flag_iconst = true;
    end
else
    type_obj = 'unknown';
end

if (mu>0.0)
    flag_softthreshod = true;
    % determine behaviour of the soft-thresholding if constraints are set
    % and TYPE_OBJ is "dephasing" or "absorbing"
    if (strcmp(type_obj,'dephasing'))
        if (options.iconst(1)>=0.0 && options.iconst(2)>0.0 && options.iconst(1)<=options.iconst(2))
            disp('Positivity constraint set.');
            flag_const = 1;                                                         % positivity
        elseif (options.iconst(2)<=0.0 && options.iconst(1)<0.0 && options.iconst(2)<=options.iconst(1))
            disp('Negativity constraint set.');
            flag_const = -1;                                                        % negativity
        else
            disp('No constraint set.');
            flag_const = 0;                                                         % no constraint
        end
    else
        if (options.rconst(1)>=0.0 && options.rconst(2)>0.0 && options.rconst(1)<=options.rconst(2))
            disp('Positivity constraint set.');
            flag_const = 1;                                                         % positivity
        elseif (options.rconst(2)<=0.0 && options.rconst(1)<0.0 && options.rconst(2)<=options.rconst(1))
            disp('Negativity constraint set.');
            flag_const = -1;                                                        % negativity
        else
            disp('No constraint set.');
            flag_const = 0;                                                         % no constraint
        end
    end  
else
    flag_softthreshod = false;
end

if (~isfield(options, 'support'))
    flag_support = false;
else
    if (any(size(options.support)~=[npix_W, npix_H]))
        error('SUPPORT mask must be same size as X0 and Y.');
    end
    flag_support = true;
    support = options.support;
end

if (~isfield(options, 'maxiter') || ~isscalar(options.maxiter))
    options.maxiter = 100;
end
maxiter = options.maxiter;

if (~isfield(options, 'flag_cost'))
    flag_cost = false;
else
    flag_cost = options.flag_cost;
end

if (~isfield(options, 'xtrue'))
    flag_snr = false;
else
    if (any(size(options.xtrue)~=[npix_W, npix_H]))
        error('GROUND TRUTH image must be same size as X0 and Y.');
    end
    flag_snr = true;
    xtrue = options.xtrue;
    normxtrue = norm(xtrue);
    evolsnr = 20*log10(normxtrue/norm(xtrue(:)-x0(:)));
end

if (~isfield(options, 'evolx'))
    options.evolx = false;
    flag_evolx = false;
else
    flag_evolx = true;
    evolx = norm(x0(:));
end

if (~isfield(options, 'verbose'))
    options.verbose = true;
end
verbose = options.verbose;

% Reconstruction
xopt = x0;
xold = x0;

%% Step 1 : propagation step
%xopt_prime = Hz*xopt+1.0;

% Compute Cost (data-fidelity)
if (flag_cost)
    evolcost = sum(sum( abs(abs(Hz(xopt) + 1.0) - y).^2 )) + mu*sum(abs(xopt(:))) ; 
end

if verbose
    fprintf('Iter:\t%03d\t| ', 0);
    if (flag_cost)
        fprintf('Cost:\t%5.2e\t| ', evolcost);
    end
    if (flag_snr)
        fprintf('SNR:\t%5.2e dB\t| ', evolsnr);
    end
    if (flag_evolx)
        fprintf('Evol X:\t%5.2e\t| ', evolx);
    end
    fprintf('\n');
end

%% Initialize the scaling factor
c=1.0;

for i=1:maxiter
 
    %% Forward propagation
    uopt = c*(Hz(xopt)+1.0);
     
    %% Calculate optimal scaling parameter c
	%c = sum(abs(uopt(:)).*y(:))/sum(abs(uopt(:)).*abs(uopt(:))) ; 

    %% Gradient descent step
    idnz = find(abs(uopt)~=0.0); 
    uoptnorm = uopt;
    uoptnorm(idnz) = uopt(idnz)./abs(uopt(idnz));
    xopt_prime = xopt - 2*H_z((uoptnorm.*(abs(uopt)-y)));
    
    %% Decompose object
    xoptreal = real(xopt_prime);
    xoptimag = imag(xopt_prime);
    
    %% Apply bound constraints  
    if (flag_rconst)
        idoutconst = find(xoptreal<rconst(1) | xoptreal>rconst(2));
        if (beta~=1)
            xoptreal(idoutconst) = real(xopt(idoutconst)) - beta*xoptreal(idoutconst);
        else
            xoptreal(idoutconst) = 0.0;
        end
    end
    if (flag_iconst)
        idoutconst = find(xoptimag<iconst(1) | xoptimag>iconst(2));
        if (beta~=1)
            xoptimag(idoutconst) = imag(xopt(idoutconst)) - beta*xoptimag(idoutconst);
        else
            xoptimag(idoutconst) = 0.0;
        end
    end
    
    %% Apply soft-thresholding operator
    if (flag_softthreshod)
        if (strcmp(type_obj,'dephasing'))
            xoptimag = softThresholdingOperator(xoptimag,mu,flag_const);
        elseif (strcmp(type_obj,'absorbing'))
            xoptreal = softThresholdingOperator(xoptreal,mu,flag_const);
        else
            xoptnew = softThresholdingOperator(xoptreal+1i*xoptimag,mu,flag_const,true);
            xoptreal = real(xoptnew);
            xoptimag = imag(xoptimag);
            clear xoptnew;
        end
    end
    
    %% Apply support constraint
    if (flag_support)
        if (beta~=1)
            xoptreal = real(xopt) - beta*(~support).*xoptreal;
            xoptimag = imag(xopt) - beta*(~support).*xoptimag;
        else
            xoptreal = support.*xoptreal;
            xoptimag = support.*xoptimag;
        end
    end
    
    %% Reconstitute object
    xopt = xoptreal + 1i*xoptimag;
      
    % Compute Cost (data-fidelity)
    if (flag_cost)      
        evolcost(i+1) = sum(sum( abs(abs(Hz(xopt) + 1.0) - y).^2)) + mu*sum(abs(xopt(:)));
    end
    
    % Compute SNR if required
    if (flag_snr)
        evolsnr(i+1) = 20*log10(normxtrue/norm(xtrue(:)-xopt(:)));
    end
    
    % Compute Evol X if required
    if (flag_evolx)
        evolx(i+1) = norm(xopt(:)-xold(:));
    end
    
    xold = xopt;
     
    if verbose
        fprintf('Iter:\t%03d\t| ', i);
        if (flag_cost)
            fprintf('Cost:\t%5.2e\t| ', evolcost(i+1));
        end
        if (flag_snr)
            fprintf('SNR:\t%5.2e dB\t| ', evolsnr(i+1));
        end
        if (flag_evolx)
            fprintf('Evol X:\t%5.2e\t| ', evolx(i+1));
        end
        fprintf('\n');
    end
    
    if (flag_snr)
        varargout{1} = evolsnr;
        if (flag_evolx)
            varargout{2} = evolx;
        end
    else
        if (flag_evolx)
            varargout{1} = evolx;
        end
    end

end

        
        


