SetFactory("OpenCASCADE");

// --------------------------------------------------
// Mesh size
// --------------------------------------------------

lc = 20.0;

// --------------------------------------------------
// Points
// --------------------------------------------------

Point(1)  = {0,   0,    0, lc};
Point(2)  = {100, 0,    0, lc};

Point(3)  = {0,   35.5, 0, lc};
Point(4)  = {100, 35.5, 0, lc};

Point(5)  = {0,   55.5, 0, lc};
Point(6)  = {100, 55.5, 0, lc};

Point(7)  = {0,   62,   0, lc};
Point(8)  = {100, 62,   0, lc};

Point(9)  = {0,   65.5, 0, lc};
Point(10) = {100, 65.5, 0, lc};

Point(11) = {0,   66.5, 0, lc};
Point(12) = {100, 66.5, 0, lc};

Point(13) = {0,   67,   0, lc};
Point(14) = {100, 67,   0, lc};

Point(15) = {100, 90,   0, lc};
Point(16) = {0,   90,   0, lc};

// --------------------------------------------------
// Horizontal lines
// --------------------------------------------------

Line(1) = {1, 2};
Line(2) = {3, 4};
Line(3) = {5, 6};
Line(4) = {7, 8};
Line(5) = {9, 10};
Line(6) = {11, 12};
Line(7) = {13, 14};
Line(8) = {16, 15};

// --------------------------------------------------
// Left vertical lines
// --------------------------------------------------

Line(9)  = {1, 3};
Line(10) = {3, 5};
Line(11) = {5, 7};
Line(12) = {7, 9};
Line(13) = {9, 11};
Line(14) = {11, 13};
Line(15) = {13, 16};

// --------------------------------------------------
// Right vertical lines
// --------------------------------------------------

Line(16) = {2, 4};
Line(17) = {4, 6};
Line(18) = {6, 8};
Line(19) = {8, 10};
Line(20) = {10, 12};
Line(21) = {12, 14};
Line(22) = {14, 15};

// --------------------------------------------------
// Layer 1
// --------------------------------------------------

Curve Loop(1) = {1, 16, -2, -9};
Plane Surface(1) = {1};

// Layer 2
Curve Loop(2) = {2, 17, -3, -10};
Plane Surface(2) = {2};

// Layer 3
Curve Loop(3) = {3, 18, -4, -11};
Plane Surface(3) = {3};

// Layer 4
Curve Loop(4) = {4, 19, -5, -12};
Plane Surface(4) = {4};

// Layer 5
Curve Loop(5) = {5, 20, -6, -13};
Plane Surface(5) = {5};

// Layer 6
Curve Loop(6) = {6, 21, -7, -14};
Plane Surface(6) = {6};

// Layer 7
Curve Loop(7) = {7, 22, -8, -15};
Plane Surface(7) = {7};

// --------------------------------------------------
// Embedded fault / fracture
// --------------------------------------------------

// Fault endpoints
//Point(1001) = {20,  0, 0, lc};
//Point(1002) = {80, 90, 0, lc};

// Fault line
//Line(100) = {1001, 1002};

// Embed fault inside all surfaces
//Curve{100} In Surface{1};
//Curve{100} In Surface{2};
//Curve{100} In Surface{3};
//Curve{100} In Surface{4};
//Curve{100} In Surface{5};
//Curve{100} In Surface{6};
//Curve{100} In Surface{7};

// --------------------------------------------------
// Physical groups - layers
// --------------------------------------------------

Physical Surface("layer_1") = {1};
Physical Surface("layer_2") = {2};
Physical Surface("layer_3") = {3};
Physical Surface("layer_4") = {4};
Physical Surface("layer_5") = {5};
Physical Surface("layer_6") = {6};
Physical Surface("layer_7") = {7};

// Entire domain
Physical Surface("domain") = {1,2,3,4,5,6,7};

// --------------------------------------------------
// Boundary physical groups
// --------------------------------------------------

Physical Curve("bottom") = {1};

Physical Curve("top") = {8};

Physical Curve("left") = {
    9, 10, 11, 12, 13, 14, 15
};

Physical Curve("right") = {
    16, 17, 18, 19, 20, 21, 22
};

// --------------------------------------------------
// Physical group - fault
// --------------------------------------------------

//Physical Curve("Fault") = {100};
//Physical Point("FracEnd") = {1001,1002};

// --------------------------------------------------
// Optional mesh controls
// --------------------------------------------------

// Horizontal refinement
nx = 10;

// Vertical refinement per layer
Transfinite Curve {1:8} = nx + 2;

Transfinite Curve {9,16}  = 6;
Transfinite Curve {10,17} = 3;
Transfinite Curve {11,18} = 3;
Transfinite Curve {12,19} = 3;
Transfinite Curve {13,20} = 2;
Transfinite Curve {14,21} = 2;
Transfinite Curve {15,22} = 4;

// Structured triangular mesh
For i In {1:7}
    Transfinite Surface {i};
EndFor

Mesh.Algorithm = 7;

// Generate 2D mesh
// Mesh 2;//+
Point(17) = {67.1, 90, 0.2, 1.0};
//+
Point(18) = {55.7, 67, 0.1, 1.0};
//+
Point(19) = {55.3, 66.5, 0.1, 1.0};
//+
Point(20) = {54.6, 65.5, 0.1, 1.0};
//+
Line(23) = {17, 18};
//+
Line(24) = {18, 19};
//+
Line(25) = {19, 20};
//+
Coherence;
//+
Line(26) = {20, 19};
//+
Line(27) = {20, 19};
//+
Line(28) = {19, 18};
//+
Line(29) = {18, 17};
