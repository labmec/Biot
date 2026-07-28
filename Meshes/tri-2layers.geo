SetFactory("OpenCASCADE");

lc = 0.12;

// --------------------------------------------------
// Domain corners
// --------------------------------------------------

Point(1) = {0,0,0,lc};
Point(2) = {1,0,0,lc};
Point(3) = {1,1,0,lc};
Point(4) = {0,1,0,lc};

// Layer interface
Point(5) = {0,0.5,0,lc};
Point(6) = {1,0.5,0,lc};

// Fault
Point(7) = {0.25,0.20,0,lc/2};
Point(8) = {0.50,0.50,0,lc/2};
Point(9) = {0.75,0.80,0,lc/2};

// --------------------------------------------------
// Boundary
// --------------------------------------------------

Line(1) = {1,2};

Line(2) = {2,6};
Line(3) = {6,3};

Line(4) = {3,4};

Line(5) = {4,5};
Line(6) = {5,1};

// Interface
Line(7) = {5,8};
Line(8) = {8,6};

// Fault
Line(9)  = {7,8};
Line(10) = {8,9};

// Auxiliary connections
Line(11) = {1,7};
Line(12) = {7,2};

Line(13) = {5,9};
Line(14) = {9,3};

// --------------------------------------------------
// Bottom layer
// --------------------------------------------------

// left of fault
Curve Loop(1) = {11,9,-7,6};
Plane Surface(1) = {1};

// right of fault
Curve Loop(2) = {12,2,-8,-9};
Plane Surface(2) = {2};

// --------------------------------------------------
// Top layer
// --------------------------------------------------

// left of fault
Curve Loop(3) = {7,10,-13,5};
Plane Surface(3) = {3};

// right of fault
Curve Loop(4) = {13,14,-3,-10};
Plane Surface(4) = {4};

// --------------------------------------------------
// Physical groups
// --------------------------------------------------

Physical Surface("layer_1") = {1,2};
Physical Surface("layer_2") = {3,4};

Physical Surface("domain") = {1,2,3,4};

Physical Curve("Fault") = {9,10};

Physical Curve("bottom") = {1};
Physical Curve("top") = {4};

Physical Curve("left") = {5,6};
Physical Curve("right") = {2,3};

Physical Point("dirXY") = {1};
Physical Point("dirY") = {2};

// --------------------------------------------------

Mesh.Algorithm = 6;
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;

//Mesh 2;