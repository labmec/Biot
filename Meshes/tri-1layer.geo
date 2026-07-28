// Gmsh project
SetFactory("OpenCASCADE");

lc = 0.3;
lc2 = 0.2;

// ======================================================
// Geometry
// ======================================================

Point(1) = {0.0, 0.0, 0.0, lc};
Point(2) = {1.0, 0.0, 0.0, lc};
Point(3) = {1.0, 1.0, 0.0, lc};
Point(4) = {0.0, 1.0, 0.0, lc};

Line(1) = {1,2};
Line(2) = {2,3};
Line(3) = {3,4};
Line(4) = {4,1};

Curve Loop(1) = {1,2,3,4};
Plane Surface(1) = {1};

// ======================================================
// Embedded 1D element (Fault)
// ======================================================

// End points of the fault
Point(10) = {0.25, 0.20, 0.0, lc2};
Point(11) = {0.75, 0.80, 0.0, lc2};

// Fault line
Line(10) = {10,11};

// Embed the line into the surface
Curve{10} In Surface{1};

// ======================================================
// Physical groups
// ======================================================

Physical Surface("layer_1", 1) = {1};

Physical Curve("bottom", 6) = {1};
Physical Curve("right", 5)  = {2};
Physical Curve("top", 7)    = {3};
Physical Curve("left", 4)   = {4};

Physical Curve("Fault", 10) = {10};

Physical Point("dirXY", 8) = {1};
Physical Point("dirY", 9)  = {2};

// ======================================================
// Mesh options
// ======================================================

// Frontal-Delaunay
Mesh.Algorithm = 6;

// Improve quality
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;

// Generate triangular mesh
// Mesh 2;