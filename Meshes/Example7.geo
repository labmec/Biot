SetFactory("OpenCASCADE");

// ==================================================
// PARAMETERS
// ==================================================

lc = 50.0;

// ==================================================
// LAYER POINTS
// ==================================================

Point(1)  = {0,   0,   0, lc};
Point(2)  = {200, 0,   0, lc};

Point(3)  = {0,   80,  0, lc};
Point(4)  = {86,   80,  0, lc};
Point(5)  = {200, 92.5,  0, lc};
Point(6)  = {88.5,   92.5,  0, lc};

Point(7)  = {0,   95,  0, lc};
Point(8)  = {89,   95,  0, lc};
Point(9)  = {200, 107.5,  0, lc};
Point(10)  = {91.5,   107.5,  0, lc};

Point(11)  = {0,   105, 0, lc};
Point(12)  = {91,   105, 0, lc};
Point(13)  = {200, 117.5, 0, lc};
Point(14)  = {93.5,   117.5, 0, lc};

Point(15)  = {0,   120, 0, lc};
Point(16)  = {94,   120, 0, lc};
Point(17) = {200, 132.5, 0, lc};
Point(18)  = {96.5,   132.5, 0, lc};

Point(19) = {0,   200, 0, lc};
Point(20) = {200, 200, 0, lc};

// Fault endpoints
Point(1001) = {70,  0,   0, lc};
Point(1002) = {110, 200, 0, lc};

// ==================================================
// OUTER BOUNDARY LINES
// ==================================================

// horizontal
Line(1) = {1,1001};
Line(23) = {1001,2};
Line(2) = {3,4};
Line(3) = {5,6};
Line(4) = {7,8};
Line(5) = {9,10};
Line(6) = {11,12};
Line(9)  = {13,14};
Line(10) = {15,16};
Line(11) = {17,18};
Line(12) = {19,1002};
Line(24) = {1002,20};

// left boundary
Line(13) = {1,3};
Line(14) = {3,7};
Line(15) = {7,11};
Line(16) = {11,15};
Line(17) = {15,19};

// right boundary
Line(18) = {2,5};
Line(19) = {5,9};
Line(20) = {9,13};
Line(21) = {13,17};
Line(22) = {17,20};

// ==================================================
// FAULT GEOMETRY
// ==================================================

// Fault line
Line(100) = {1001,4};
Line(101) = {4,6};
Line(102) = {6,8};
Line(103) = {8,12};
Line(104) = {12,10};
Line(105) = {10,14};
Line(106) = {14,16};
Line(107) = {16,18};
Line(108) = {18,1002};

// ==================================================
// LAYER SURFACES
// ==================================================

Curve Loop(1) = {1,100,-2,-13};
Plane Surface(1) = {1};

Curve Loop(2) = {23,18,-3,-101,-100};
Plane Surface(2) = {2};

Curve Loop(3) = {2,101,102,-4,-14};
Plane Surface(3) = {3};

Curve Loop(4) = {3,19,-5,-104,-103,-102};
Plane Surface(4) = {4};

Curve Loop(25) = {4,103,-6,-15};
Plane Surface(25) = {25};

Curve Loop(6) = {5,20,-9,-105};
Plane Surface(6) = {6};

Curve Loop(7) = {6,104,105,106,-10,-16};
Plane Surface(7) = {7};

Curve Loop(8) = {9,21,-11,-107,-106};
Plane Surface(8) = {8};

Curve Loop(9) = {10,107,108,-12,17};
Plane Surface(9) = {9};

Curve Loop(10) = {11,22,-24,-108};
Plane Surface(10) = {10};

// ==================================================
// PHYSICAL GROUPS
// ==================================================

// -------------------------
// Entire domain
// -------------------------
Physical Surface("domain") =
{
    1, 2,
    3, 4,
    25, 6,
    7, 8,
    9, 10
};

// -------------------------
// Geological layers
// -------------------------

// Layer 1 (bottom)
Physical Surface("layer_11") = {1};
Physical Surface("layer_12") = {2};

// Layer 2
Physical Surface("layer_21") = {3};
Physical Surface("layer_22") = {4};

// Layer 3
Physical Surface("layer_31") = {25};
Physical Surface("layer_32") = {6};

// Layer 4
Physical Surface("layer_41") = {7};
Physical Surface("layer_42") = {8};

// Layer 5 (top)
Physical Surface("layer_51") ={9};
Physical Surface("layer_52") = {10};

// -------------------------
// Fault
// -------------------------
Physical Curve("Fault") =
{
    100,101,102,103,104,105,106,107,108
};

// -------------------------
// External boundaries
// -------------------------

Physical Curve("bottom") =
{
    1,23
};

Physical Curve("top") =
{
    12,24
};

Physical Curve("left") =
{
    13,14,15,16,17
};

Physical Curve("right") =
{
    18,19,20,21,22
};

// Optional: each horizontal interface
Physical Curve("interface_1") = {2,3};
Physical Curve("interface_2") = {4,5};
Physical Curve("interface_3") = {6,9};
Physical Curve("interface_4") = {10,11};

// ==================================================
// MESH OPTIONS
// ==================================================

// Global size
Mesh.MeshSizeMin = lc/10;
Mesh.MeshSizeMax = lc;

// Distance field from the fault
Field[1] = Distance;
Field[1].CurvesList = {100,101,103,105,107,108};

Field[2] = Threshold;
Field[2].InField = 1;
Field[2].SizeMin = lc/4;
Field[2].SizeMax = lc;
Field[2].DistMin = 5;
Field[2].DistMax = 25;

// Extra refinement at the fault tips
Field[3] = Distance;
Field[3].PointsList = {1001,1002};

Field[4] = Threshold;
Field[4].InField = 3;
Field[4].SizeMin = lc/12;
Field[4].SizeMax = lc/4;
Field[4].DistMin = 2;
Field[4].DistMax = 10;

// Combine both fields
Field[5] = Min;
Field[5].FieldsList = {2,4};

Background Field = 5;

Mesh.Algorithm = 6;