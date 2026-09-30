SetFactory("OpenCASCADE");

// ==================================================
// PARAMETERS
// ==================================================

lc = 100.0;


// ==================================================
// LAYER POINTS
// ==================================================

Point(1)  = {0,      0,   0, lc};
Point(2)  = {1000,   0,   0, lc};

Point(3)  = {0,   305,  0, lc};
Point(4)  = {511.02,   305,  0, lc};

Point(5)  = {1000, 355,  0, lc};
Point(6)  = {529.22,   355,  0, lc};

Point(7)  = {0,   505,  0, lc};
Point(8)  = {583.82,   505,  0, lc};

Point(9)  = {1000, 555,  0, lc};
Point(10) = {602.02,   555,  0, lc};

Point(11) = {0,   570,  0, lc};
Point(12) = {607.48,   570,  0, lc};

Point(13) = {1000, 620,  0, lc};
Point(14) = {625.68,   620,  0, lc};

Point(15) = {0,   605,  0, lc};
Point(16) = {620.22,   605,  0, lc};

Point(17) = {1000, 655,  0, lc};
Point(18) = {638.42,   655,  0, lc};

Point(19) = {0,   615,  0, lc};
Point(20) = {623.86,   615,  0, lc};

Point(21) = {1000, 665,  0, lc};
Point(22) = {642.06,   665,  0, lc};

Point(23) = {0,   620,  0, lc};
Point(24) = {625.68,   620,  0, lc};

Point(25) = {1000, 670,  0, lc};
Point(26) = {643.88,   670,  0, lc};

Point(27) = {0,   900,  0, lc};
Point(28) = {1000, 900,  0, lc};


// ==================================================
// FAULT ENDPOINTS
// ==================================================

Point(1001) = {400,   0,   0, lc};
Point(1002) = {727.6, 900,  0, lc};


// ==================================================
// OUTER BOUNDARY LINES
// ==================================================

// -------------------------
// Bottom
// -------------------------

Line(1) = {1,1001};
Line(2) = {1001,2};


// -------------------------
// Horizontal geological interfaces
// -------------------------

Line(3)  = {3,4};
Line(4)  = {5,6};

Line(5)  = {7,8};
Line(6)  = {9,10};

Line(7)  = {11,12};
Line(8)  = {13,14};

Line(9)  = {15,16};
Line(10) = {17,18};

Line(11) = {19,20};
Line(12) = {21,22};

Line(13) = {23,24};
Line(14) = {25,26};


// -------------------------
// Top
// -------------------------

Line(15) = {27,1002};
Line(16) = {1002,28};


// -------------------------
// Left boundary
// -------------------------

Line(17) = {1,3};
Line(18) = {3,7};
Line(19) = {7,11};
Line(20) = {11,15};
Line(21) = {15,19};
Line(22) = {19,23};
Line(23) = {23,27};


// -------------------------
// Right boundary
// -------------------------

Line(24) = {2,5};
Line(25) = {5,9};
Line(26) = {9,13};
Line(27) = {13,17};
Line(28) = {17,21};
Line(29) = {21,25};
Line(30) = {25,28};


// ==================================================
// FAULT GEOMETRY
// ==================================================
//
// The fault is a straight line from
//
//     (400,0)
//          to
//     (727.6,900)
//
// The points are ordered according to increasing y.
//
// IMPORTANT:
// Point 24 has the same coordinates as Point 14.
// Therefore the geological interface uses Point 14
// and Point 24 is not used in the topology.
//


// Fault segments

Line(100) = {1001,4};
Line(101) = {4,6};
Line(102) = {6,8};
Line(103) = {8,10};
Line(104) = {10,12};
Line(105) = {12,16};
Line(106) = {16,20};
Line(107) = {20,14};
Line(108) = {14,18};
Line(109) = {18,22};
Line(110) = {22,26};
Line(111) = {26,1002};


// ==================================================
// CORRECTION TO HORIZONTAL INTERFACE
// ==================================================
//
// Point 24 = Point 14 geometrically.
// Use Point 14 for the interface at y = 620.
//

Line(13) = {23,14};


// ==================================================
// LAYER SURFACES
// ==================================================
//
// The geometry produces 14 elementary surfaces:
//
//       Layer 1 : 1,  2
//       Layer 2 : 3,  4
//       Layer 3 : 5,  6
//       Layer 4 : 7,  8
//       Layer 5 : 9, 10
//       Layer 6 : 11,12
//       Layer 7 : 13,14
//
// The first surface of each pair is on the left
// side of the fault and the second on the right.
//


// --------------------------------------------------
// Layer 1
// --------------------------------------------------

// Left
ll = newreg;
Curve Loop(ll) = {-1,17,3,-100};
Plane Surface(1) = {ll};

// Right
ll = newreg;
Curve Loop(ll) = {-2,100,101,-4,-24};
Plane Surface(2) = {ll};


// --------------------------------------------------
// Layer 2
// --------------------------------------------------

// Left
ll = newreg;
Curve Loop(ll) = {-3,18,5,-102,-101};
Plane Surface(3) = {ll};

// Right
ll = newreg;
Curve Loop(ll) = {4,102,103,-6,-25};
Plane Surface(4) = {ll};


// --------------------------------------------------
// Layer 3
// --------------------------------------------------

// Left
ll = newreg;
Curve Loop(ll) = {-5,19,7,-104,-103};
Plane Surface(5) = {ll};

// Right
ll = newreg;
Curve Loop(ll) = {6,104,105,106,107,-8,-26};
Plane Surface(6) = {ll};


// --------------------------------------------------
// Layer 4
// --------------------------------------------------

// Left
ll = newreg;
Curve Loop(ll) = {-7,20,9,-105};
Plane Surface(7) = {ll};

// Right
ll = newreg;
Curve Loop(ll) = {8,108,-10,-27};
Plane Surface(8) = {ll};


// --------------------------------------------------
// Layer 5
// --------------------------------------------------

// Left
ll = newreg;
Curve Loop(ll) = {-9,21,11,-106};
Plane Surface(9) = {ll};

// Right
ll = newreg;
Curve Loop(ll) = {10,109,-12,-28};
Plane Surface(10) = {ll};


// --------------------------------------------------
// Layer 6
// --------------------------------------------------

// Left
ll = newreg;
Curve Loop(ll) = {-11,22,13,-107};
Plane Surface(11) = {ll};

// Right
ll = newreg;
Curve Loop(ll) = {12,110,-14,-29};
Plane Surface(12) = {ll};


// --------------------------------------------------
// Layer 7
// --------------------------------------------------

// Left
ll = newreg;
Curve Loop(ll) = {-13,23,15,-111,-110,-109,-108};
Plane Surface(13) = {ll};

// Right
ll = newreg;
Curve Loop(ll) = {14,111,16,-30};
Plane Surface(14) = {ll};


// ==================================================
// PHYSICAL GROUPS
// ==================================================


// --------------------------------------------------
// Entire domain
// --------------------------------------------------

Physical Surface("domain") =
{
    1, 2,
    3, 4,
    5, 6,
    7, 8,
    9, 10,
    11, 12,
    13, 14
};


// --------------------------------------------------
// Geological layers
// --------------------------------------------------

Physical Surface("layer_11") = {1};

Physical Surface("layer_12") = {2};

Physical Surface("layer_21") = {3};

Physical Surface("layer_22") = {4};

Physical Surface("layer_31") = {5};

Physical Surface("layer_32") = {6};

Physical Surface("layer_41") = {7};

Physical Surface("layer_42") = {8};

Physical Surface("layer_51") = {9};

Physical Surface("layer_52") = {10};

Physical Surface("layer_61") = {11};

Physical Surface("layer_62") = {12};

Physical Surface("layer_71") = {13};

Physical Surface("layer_72") = {14};



// ==================================================
// FAULT
// ==================================================

Physical Curve("Fault") =
{
    100,101,102,103,104,
    105,106,107,108,109,110,111
};


// --------------------------------------------------
// Reservoir fault
// --------------------------------------------------

Physical Curve("ResFault") =
{
    101, 102, 103
};


// ==================================================
// POST-PROCESSING CURVES
// ==================================================

Physical Curve("PostProc") =
{
    25,26,27,28,29
};


// ==================================================
// EXTERNAL BOUNDARIES
// ==================================================

// Bottom
Physical Curve("bottom") =
{
    1,2
};


// Top
Physical Curve("top") =
{
    15,16
};


// Left
Physical Curve("left") =
{
    17,18,19,20,21,22,23
};


// Right
Physical Curve("right") =
{
    24,25,26,27,28,29,30
};


// ==================================================
// GEOLOGICAL INTERFACES
// ==================================================

Physical Curve("interface_1") =
{
    3,4
};

Physical Curve("interface_2") =
{
    5,6
};

Physical Curve("interface_3") =
{
    7,8
};

Physical Curve("interface_4") =
{
    9,10
};

Physical Curve("interface_5") =
{
    11,12
};

Physical Curve("interface_6") =
{
    13,14
};


// ==================================================
// POINT BOUNDARY CONDITIONS
// ==================================================

Physical Point("dirY") = {1};

Physical Point("dirX") = {2};


// ==================================================
// MESH OPTIONS
// ==================================================

Mesh.MeshSizeMin = lc/10;
Mesh.MeshSizeMax = lc;


// ==================================================
// DISTANCE FIELD FROM THE FAULT
// ==================================================

Field[1] = Distance;

Field[1].CurvesList =
{
    100,101,102,103,104,
    105,106,107,108,109,110,111
};


// ==================================================
// FAULT REFINEMENT
// ==================================================

Field[2] = Threshold;

Field[2].InField = 1;

Field[2].SizeMin = lc/4;
Field[2].SizeMax = lc;

Field[2].DistMin = 5;
Field[2].DistMax = 25;


// ==================================================
// FAULT-TIP REFINEMENT
// ==================================================

Field[3] = Distance;

Field[3].PointsList =
{
    1001,1002
};


Field[4] = Threshold;

Field[4].InField = 3;

Field[4].SizeMin = lc/12;
Field[4].SizeMax = lc/4;

Field[4].DistMin = 2;
Field[4].DistMax = 10;


// ==================================================
// COMBINE REFINEMENT FIELDS
// ==================================================

Field[5] = Min;

Field[5].FieldsList =
{
    2,4
};

Background Field = 5;


// ==================================================
// MESH ALGORITHM
// ==================================================

Mesh.Algorithm = 6;