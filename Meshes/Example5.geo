SetFactory("OpenCASCADE");

// ==================================================
// PARAMETERS
// ==================================================

lc = 20.0;

// ==================================================
// LAYER POINTS
// ==================================================

Point(1)  = {0,   0,   0, lc};
Point(2)  = {200, 0,   0, lc};

Point(3)  = {0,   65,  0, lc};
Point(4)  = {200, 65,  0, lc};

Point(5)  = {0,   95,  0, lc};
Point(6)  = {200, 95,  0, lc};

Point(7)  = {0,   105, 0, lc};
Point(8)  = {200, 105, 0, lc};

Point(9)  = {0,   135, 0, lc};
Point(10) = {200, 135, 0, lc};

Point(11) = {0,   200, 0, lc};
Point(12) = {200, 200, 0, lc};

// ==================================================
// OUTER BOUNDARY LINES
// ==================================================

// horizontal
Line(1) = {1,2};
Line(2) = {3,4};
Line(3) = {5,6};
Line(4) = {7,8};
Line(5) = {9,10};
Line(6) = {11,12};

// left boundary
Line(9)  = {1,3};
Line(10) = {3,5};
Line(11) = {5,7};
Line(12) = {7,9};
Line(13) = {9,11};

// right boundary
Line(16) = {2,4};
Line(17) = {4,6};
Line(18) = {6,8};
Line(19) = {8,10};
Line(20) = {10,12};

// ==================================================
// LAYER SURFACES
// ==================================================

Curve Loop(1) = {1,16,-2,-9};
Plane Surface(1) = {1};

Curve Loop(2) = {2,17,-3,-10};
Plane Surface(2) = {2};

Curve Loop(3) = {3,18,-4,-11};
Plane Surface(3) = {3};

Curve Loop(4) = {4,19,-5,-12};
Plane Surface(4) = {4};

Curve Loop(5) = {5,20,-6,-13};
Plane Surface(5) = {5};

// ==================================================
// FAULT GEOMETRY
// ==================================================

// Fault endpoints
Point(1001) = {70,  0,   0, lc};
Point(1002) = {110, 200, 0, lc};

// Fault line
Line(100) = {1001,1002};

// ==================================================
// SPLIT SURFACES WITH THE FAULT
// ==================================================

BooleanFragments{
    Surface{1,2,3,4,5};
    Delete;
}{
    Curve{100};
    Delete;
}

// ==================================================
// AUTOMATIC ENTITY COLLECTION
// ==================================================

// All resulting surfaces
allSurfaces[] = Surface "*";

// --------------------------------------------------
// Fault curves
// --------------------------------------------------

faultCurves[] = Curve In BoundingBox{
    39.9,  -0.1, -1,
    160.1, 200.1, 1
};

// --------------------------------------------------
// Boundary curves
// --------------------------------------------------

bottomCurves[] = Curve In BoundingBox{
    -0.1, -0.1, -1,
    200.1, 0.1, 1
};

topCurves[] = Curve In BoundingBox{
    -0.1, 199.9, -1,
    200.1, 200.1, 1
};

leftCurves[] = Curve In BoundingBox{
    -0.1, -0.1, -1,
    0.1, 200.1, 1
};

rightCurves[] = Curve In BoundingBox{
    199.9, -0.1, -1,
    200.1, 200.1, 1
};

// ==================================================
// PHYSICAL GROUPS
// ==================================================

Physical Surface("domain") = {allSurfaces[]};

Physical Curve("Fault") = {faultCurves[]};

Physical Curve("bottom") = {bottomCurves[]};
Physical Curve("top")    = {topCurves[]};
Physical Curve("left")   = {leftCurves[]};
Physical Curve("right")  = {rightCurves[]};
Physical Point("dirXY") = {1};

// ==================================================
// MESH OPTIONS
// ==================================================

// Frontal-Delaunay
Mesh.Algorithm = 6;

// Optimization
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;

// Finer mesh near fault tips
Characteristic Length{1001,1002} = lc;
Characteristic Length{3,8,11,14,17} = lc;

// ==================================================
// GENERATE MESH
// ==================================================

//Mesh 2;//+

// ==================================================
// PHYSICAL GROUPS
// ==================================================

Physical Surface("layer_1", 28) = {1, 2};
//+
Physical Surface("layer_2", 29) = {3, 4};
//+
Physical Surface("layer_3", 30) = {5, 6};
//+
Physical Surface("layer_4", 31) = {7, 8};
//+
Physical Surface("layer_5", 32) = {9, 10};
