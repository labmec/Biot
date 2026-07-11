// Gmsh project created on Fri Jul  5 14:35:05 2024
SetFactory("OpenCASCADE");
//+
lc = 0.5;
//+
Point(1) = {0.0, 0, 0, lc};
//+
Point(2) = {1.0, 0, 0, lc};
//+
Point(3) = {1.0, 1.0, 0, lc};
//+
Point(4) = {0.0, 1.0, 0, lc};
//+
Line(1) = {1, 2};
//+
Line(2) = {2, 3};
//+
Line(3) = {3, 4};
//+
Line(4) = {4, 1};
//+
Curve Loop(1) = {2, 3, 4, 1};
//+
Plane Surface(1) = {1};
//+
Physical Curve("bottom", 6) = {1};
//+
Physical Curve("right", 5) = {2};
//+
Physical Curve("top", 7) = {3};
//+
Physical Curve("left", 4) = {4};
//+
Physical Surface("domain", 1) = {1};
//+
Physical Point("dirXY", 8) = {1};

Physical Point("dirY", 9) = {2};
