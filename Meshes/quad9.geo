lc = 1.0;

// Geometry
Point(1) = {0.0, 0.0, 0.0, lc};
Point(2) = {1.0, 0.0, 0.0, lc};
Point(3) = {1.0, 1.0, 0.0, lc};
Point(4) = {0.0, 1.0, 0.0, lc};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};

Curve Loop(1) = {1, 2, 3, 4};
Plane Surface(1) = {1};

// --------------------------------------------------
// Physical groups
// --------------------------------------------------

Physical Curve("bottom", 6) = {1};
Physical Curve("right", 5)  = {2};
Physical Curve("top", 7)    = {3};
Physical Curve("left", 4)   = {4};

Physical Surface("domain", 1) = {1};

Physical Point("dirXY", 8) = {1};
Physical Point("dirY", 9)  = {2};

// --------------------------------------------------
// Structured 3x3 quadrilateral mesh
// --------------------------------------------------

// 3 elements per edge => 4 nodes
Transfinite Curve {1,2,3,4} = 4;

// Structured surface
Transfinite Surface {1};

// Convert structured triangles into quads
Recombine Surface {1};

// Generate mesh
//Mesh 2;
