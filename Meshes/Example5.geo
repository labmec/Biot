SetFactory("OpenCASCADE");

// ==================================================
// PARAMETERS
// ==================================================

lc = 25.0;

// ==================================================
// LAYER POINTS
// ==================================================

Point(1)  = {0,   0,   0, lc};
Point(2)  = {200, 0,   0, lc};

Point(3)  = {0,   65,  0, lc};
Point(4)  = {63,   65,  0, lc};
Point(5)  = {200, 45,  0, lc};
Point(6)  = {59, 45,  0, lc};

Point(7)  = {0,   95,  0, lc};
Point(8)  = {69,   95,  0, lc};
Point(9)  = {200, 75,  0, lc};
Point(10)  = {65,   75,  0, lc};

Point(11)  = {0,   105, 0, lc};
Point(12)  = {71,   105, 0, lc};
Point(13)  = {200, 85, 0, lc};
Point(14)  = {67, 85, 0, lc};

Point(15)  = {0,  135, 0, lc};
Point(16)  = {77,  135, 0, lc};
Point(17) = {200, 115, 0, lc};
Point(18)  = {73,  115, 0, lc};

Point(19) = {0,   200, 0, lc};
Point(20) = {200, 200, 0, lc};

Point(100) = {30, 0, 0, lc};
Point(101) = {30, 65, 0, lc};
Point(102) = {30, 95, 0, lc};
Point(103) = {30, 105, 0, lc};
Point(104) = {30, 135, 0, lc};
Point(105) = {30, 200, 0, lc};

// Fault endpoints
Point(1001) = {50,  0,   0, lc};
Point(1002) = {90, 200, 0, lc};

// ==================================================
// OUTER BOUNDARY LINES
// ==================================================

// horizontal
Line(1) = {1,100};
Line(201) = {100,1001};
Line(23) = {1001,2};
Line(2) = {3,101};
Line(202) = {101,4};
Line(3) = {5,6};
Line(4) = {7,102};
Line(204) = {102,8};
Line(5) = {9,10};
Line(6) = {11,103};
Line(206) = {103,12};
Line(9)  = {13,14};
Line(10) = {15,104};
Line(210) = {104,16};
Line(11) = {17,18};
Line(12) = {19,105};
Line(212) = {105,1002};
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
Line(100) = {1001,6};
Line(101) = {6,4};
Line(102) = {4,10};
Line(103) = {10,14};
Line(104) = {14,8};
Line(105) = {8,12};
Line(106) = {12,18};
Line(107) = {18,16};
Line(108) = {16,1002};

// Plot line
Line(25) = {100,101};
Line(26) = {101,102};
Line(27) = {102,103};
Line(28) = {103,104};
Line(29) = {104,105};

// ==================================================
// LAYER SURFACES
// ==================================================

ll = newreg;
Curve Loop(ll) = {1,25,-2,-13};
Plane Surface(1) = {ll};

ll = newreg;
Curve Loop(ll) = {201,100,101,-202,-25};
Plane Surface(101) = {ll};

ll = newreg;
Curve Loop(ll) = {23,18,-3,-100};
Plane Surface(2) = {ll};

ll = newreg;
Curve Loop(ll) = {2,26,-4,-14};
Plane Surface(3) = {ll};

ll = newreg;
Curve Loop(ll) = {202,102,103,104,-204,-26};
Plane Surface(103) = {ll};

ll = newreg;
Curve Loop(ll) = {3,19,-5,-101,-102};
Plane Surface(4) = {ll};

ll = newreg;
Curve Loop(ll) = {4,27,-6,-15};
Plane Surface(5) = {ll};

ll = newreg;
Curve Loop(ll) = {204,105,-206,-27};
Plane Surface(105) = {ll};

ll = newreg;
Curve Loop(ll) = {5,20,-9,-103};
Plane Surface(6) = {ll};

ll = newreg;
Curve Loop(ll) = {6,28,-10,-16};
Plane Surface(7) = {ll};

ll = newreg;
Curve Loop(ll) = {206,106,107,-210,-28};
Plane Surface(107) = {ll};

ll = newreg;
Curve Loop(ll) = {9,21,-11,-106,-105,-104};
Plane Surface(8) = {ll};

ll = newreg;
Curve Loop(ll) = {10,29,-12,-17};
Plane Surface(9) = {ll};

ll = newreg;
Curve Loop(ll) = {210,108,-212,-29};
Plane Surface(109) = {ll};

ll = newreg;
Curve Loop(ll) = {11,22,-24,-108,-107};
Plane Surface(10) = {ll};

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
    5, 6,
    7, 8,
    9, 10,
    101, 103, 105, 107, 109
};

// -------------------------
// Geological layers
// -------------------------

// Layer 1 (bottom)
Physical Surface("layer_11") = {1,101};
Physical Surface("layer_12") = {2};

// Layer 2
Physical Surface("layer_21") = {3,103};
Physical Surface("layer_22") = {4};

// Layer 3
Physical Surface("layer_31") = {5,105};
Physical Surface("layer_32") = {6};

// Layer 4
Physical Surface("layer_41") = {7,107};
Physical Surface("layer_42") = {8};

// Layer 5 (top)
Physical Surface("layer_51") ={9,109};
Physical Surface("layer_52") = {10};

// -------------------------
// Fault
// -------------------------
Physical Curve("Fault") =
{
    100,101,102,103,104,106,107,108
};

Physical Curve("ResFault") =
{
    105
};

Physical Curve("PostProc") = {25,26,27,28,29};

// -------------------------
// External boundaries
// -------------------------

Physical Curve("bottom") =
{
    1,23,201 
};

Physical Curve("top") =
{
    12,24,212
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

// Point Boundary Conditions
Physical Point("dirY") = {1};
Physical Point("dirX") = {2};


// ==================================================
// MESH OPTIONS
// ==================================================

// Global size
Mesh.MeshSizeMin = lc/10;
Mesh.MeshSizeMax = lc;

// Distance field from the fault
Field[1] = Distance;
Field[1].CurvesList = {100,101,102,103,104,105,106,107,108};

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