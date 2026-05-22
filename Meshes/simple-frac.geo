cl__1 = 20;
lc2 = 35;
Point(1) = {57, 24, 0, lc2};
Point(5) = {33, 70, 0, lc2};
Point(12) = {0, 0, 0, cl__1};
Point(13) = {0, 100, 0, cl__1};
Point(14) = {100, 100, 0, cl__1};
Point(15) = {100, 0, 0, cl__1};


Line(12) = {12, 13};
Line(13) = {13, 14};
Line(14) = {14, 15};
Line(15) = {15, 12};
Line(16) = {5,1};
Line(17) = {1,5};

Curve Loop(11) = {16,17};
Plane Surface(11) = {11};
Physical Curve("Fault",9)={16,17};
Physical Point("FracEnds",10)={1,5};

Curve Loop(1000) = { 16,17, -15, -14, -13, -12};

Plane Surface(1000) = {1000};
Physical Surface("domain", 1) = {1000};
//+
Physical Curve("right", 4) = {14};
Physical Curve("left", 5) = {12};
Physical Curve("top", 6) = {13};
Physical Curve("bottom", 7) = {15};
Coherence;
