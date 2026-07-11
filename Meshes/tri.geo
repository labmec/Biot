lc = 1.0;

Point(1) = {0.0, 0,   0, lc};
Point(2) = {1.0, 0,   0, lc};
Point(3) = {1.0, 1.0, 0, lc};
Point(4) = {0.0, 1.0, 0, lc};

Line(1) = {1,2};
Line(2) = {2,3};
Line(3) = {3,4};
Line(4) = {4,1};

Curve Loop(1) = {1,2,3,4};
Plane Surface(1) = {1};

Physical Curve("bottom", 6) = {1};
Physical Curve("right", 5)  = {2};
Physical Curve("top", 7)    = {3};
Physical Curve("left", 4)   = {4};

Physical Surface("domain", 1) = {1};

// One element per edge
Transfinite Curve {1,2,3,4} = 2;

// Structured interpolation from the 4 corners
Transfinite Surface {1} Alternate;

// Triangular mesh (do not recombine)
Mesh.RecombineAll = 0;

//Mesh 2;