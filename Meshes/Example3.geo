SetFactory("OpenCASCADE");

// ==================================================
// PARAMETERS
// ==================================================

lc = 10.0;

// ==================================================
// LAYER POINTS
// ==================================================

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
Line(7) = {13,14};
Line(8) = {16,15};

// left boundary
Line(9)  = {1,3};
Line(10) = {3,5};
Line(11) = {5,7};
Line(12) = {7,9};
Line(13) = {9,11};
Line(14) = {11,13};
Line(15) = {13,16};

// right boundary
Line(16) = {2,4};
Line(17) = {4,6};
Line(18) = {6,8};
Line(19) = {8,10};
Line(20) = {10,12};
Line(21) = {12,14};
Line(22) = {14,15};

// ==================================================
// INITIAL LAYER SURFACES
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

Curve Loop(6) = {6,21,-7,-14};
Plane Surface(6) = {6};

Curve Loop(7) = {7,22,-8,-15};
Plane Surface(7) = {7};

// ==================================================
// FAULT GEOMETRY
// ==================================================

// fault endpoints
Point(1001) = {30,  0,  0, lc};
Point(1002) = {70, 90, 0, lc};

// fault line
Line(100) = {1001,1002};

// ==================================================
// SPLIT SURFACES USING THE FAULT
// ==================================================

// IMPORTANT:
// This fragments all surfaces using the fault.
// The fault becomes an actual geometric interface.

BooleanFragments{
    Surface{1,2,3,4,5,6,7};
    Delete;
}{
    Curve{100};
    Delete;
}

// ==================================================
// PHYSICAL GROUPS
// ==================================================

// After BooleanFragments, Gmsh creates new surfaces.
// We collect them automatically.

// All surfaces
allSurfaces[] = Surface "*";

// Fault curves
faultCurves[] = Curve In BoundingBox{
    19.9, -0.1, -1,
    80.1, 90.1,  1
};

// --------------------------------------------------
// Boundary curves
// --------------------------------------------------

bottomCurves[] = Curve In BoundingBox{
    -0.1, -0.1, -1,
    100.1, 0.1, 1
};

topCurves[] = Curve In BoundingBox{
    -0.1, 89.9, -1,
    100.1, 90.1, 1
};

leftCurves[] = Curve In BoundingBox{
    -0.1, -0.1, -1,
    0.1, 90.1, 1
};

rightCurves[] = Curve In BoundingBox{
    99.9, -0.1, -1,
    100.1, 90.1, 1
};

// --------------------------------------------------
// Physical groups
// --------------------------------------------------

Physical Surface("domain") = {allSurfaces[]};

Physical Curve("Fault") = {faultCurves[]};

Physical Curve("bottom") = {bottomCurves[]};
Physical Curve("top")    = {topCurves[]};
Physical Curve("left")   = {leftCurves[]};
Physical Curve("right")  = {rightCurves[]};

// ==================================================
// INDIVIDUAL LAYER IDENTIFICATION
// ==================================================

// You may need to inspect the generated surface IDs
// in Gmsh GUI after fragmentation.
//
// Example:
//
// Physical Surface("layer_1") = { ... };
// Physical Surface("layer_2") = { ... };
//
// because BooleanFragments creates new IDs automatically.

// ==================================================
// MESH OPTIONS
// ==================================================

Mesh.Algorithm = 6;

Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;

// finer mesh near fault tips
Characteristic Length{1001,1002} = lc;
Characteristic Length{3,8,11,14,17,20} = lc;

// ==================================================
// GENERATE 2D MESH
// ==================================================

//Mesh 2;//+

// ==================================================
// PHYSICAL GROUPS
// ==================================================

Physical Surface("layer_1", 38) = {1, 2};
//+
Physical Surface("layer_2", 39) = {3, 4};
//+
Physical Surface("layer_3", 40) = {5, 6};
//+
Physical Surface("layer_4", 41) = {7, 8};
//+
Physical Surface("layer_5", 42) = {9, 10};
//+
Physical Surface("layer_6", 43) = {11, 12};
//+
Physical Surface("layer_7", 44) = {13, 14};
