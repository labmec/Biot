lc1 = 16;
//+
lc2 = 2;
//+
Point(1) = {0, 0, 0, lc1};
//+
Point(2) = {100, 0, 0, lc1};
//+
Point(3) = {100, 100, 0, lc1};
//+
Point(4) = {0, 100, 0, lc1};
//+
Point(5) = {20, 40, 0, lc2};
//+
Point(6) = {20, 50, 0, lc2};
//+
Point(7) = {80, 50, 0, lc2};
//+
Point(8) = {80, 40, 0, lc2};
//+
Line(1) = {6, 7};
//+
Line(2) = {7, 8};
//+
Line(3) = {8, 5};
//+
Line(4) = {5, 6};
//+
Line(5) = {1, 2};
//+
Line(6) = {2, 3};
//+
Line(7) = {3, 4};
//+
Line(8) = {4, 1};
//+
Curve Loop(1) = {1, 2, 3, 4, -5, -6, -7, -8};
//+
Curve Loop(2) = {1, 2, 3, 4};
//+
Plane Surface(1) = {1};
//+
Plane Surface(2) = {2};
//+
Physical Surface("domain", 1) = {1, 2};
//+
Physical Surface("far-field", 2) = {1};
//+
Physical Surface("pre-stress", 3) = {2};
//+
Physical Curve("left", 4) = {8};
//+
Physical Curve("right", 5) = {6};
//+
Physical Curve("bottom", 6) = {5};
//+
Physical Curve("top", 7) = {7};
//+
Physical Curve("res-left", 8) = {4};
//+
Physical Curve("res-right", 9) = {2};
//+
Physical Curve("res-bottom", 10) = {3};
//+
Physical Curve("res-top", 11) = {1};