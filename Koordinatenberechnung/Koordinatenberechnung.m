% Kalibration Koordinatensystem

% Testdaten
% Punkt i in Bild
Bp1 = [20;30];
Bp2 = [80;30];
Bp3 = [50;80];
Bp4 = [10;70];

% Punkt i in realen Koorinaten (der x-y-Kinematik)
Rp1 = [13;9];
Rp2 = [ 7;9];
Rp3 = [10;4];
Rp4 = [14;5];

% Koorinatenursprung des R-Koordinatensystem in B-Koordinaten
BpR_Org = [150; 120];

% Koorinatenursprung des B-Koordinatensystem in R-Koordinaten
RpB_Org = [15; 12];

temp = ones(3,4);
temp(1:2, 1:4) = [Bp1, Bp2, Bp3, Bp4]
erg = [Rp1, Rp2, Rp3, Rp4] * temp'*inv(temp*temp')

A = erg(1:2, 1:2);  % Rotationsmatrix von B nach R
RpB_Org_sim = erg(1:2,3)

BpR_Org_sim = inv( A ) * -RpB_Org