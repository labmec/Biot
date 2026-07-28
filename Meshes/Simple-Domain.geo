SetFactory("OpenCASCADE");

// =====================================================
// Mesh sizes
// =====================================================

lcFar   = 80.0;
lcRes   = 10.0;
lcFault = 20.0;

// =====================================================
// Outer boundary (100 x 90)
// =====================================================

Point(1) = {0,   0, 0, lcFar};
Point(2) = {100, 0, 0, lcFar};
Point(3) = {100,90, 0, lcFar};
Point(4) = {0,  90, 0, lcFar};

Line(1) = {1,2};
Line(2) = {2,3};
Line(3) = {3,4};
Line(4) = {4,1};

Curve Loop(1) = {1,2,3,4};

// =====================================================
// Reservoir (50 x 30), centered
// =====================================================

Point(11) = {25,40,0,lcRes};
Point(12) = {75,40,0,lcRes};
Point(13) = {75,60,0,lcRes};
Point(14) = {25,60,0,lcRes};

Line(11) = {11,12};
Line(12) = {12,13};
Line(13) = {13,14};
Line(14) = {14,11};

Curve Loop(2) = {11,12,13,14};

// =====================================================
// Surfaces
// =====================================================

// Far-field (outer region excluding the reservoir)
Plane Surface(1) = {1,2};

// Reservoir
Plane Surface(2) = {2};

// =====================================================
// Fault (~70 degrees)
// =====================================================

// Bottom intersection
Point(101) = {33.5,0,0,lcFault};

// Top intersection
Point(102) = {66.5,90,0,lcFault};

Line(101) = {101,102};

// =====================================================
// Fragment surfaces with the fault
// =====================================================

BooleanFragments
{
    Surface{1,2};
    Delete;
}
{
    Curve{101};
    Delete;
}

// =====================================================
// Physical groups
// =====================================================

// Entire domain
Physical Surface("Domain") = Surface{:};

// Reservoir
reservoir[] = Surface In BoundingBox
{
24.9,29.9,-1,
75.1,60.1, 1
};

Physical Surface("Reservoir") = {reservoir[]};
Physical Surface("Far-Field") = {1,2};

// (Alternative if your Gmsh version does not support deleting from arrays,
// simply inspect the IDs in the GUI and replace below.)

// -----------------------------------------------------
// Boundary groups
// -----------------------------------------------------

eps = 1e-6;

bottom[] = Curve In BoundingBox{-eps,-eps,-1,100+eps,eps,1};
right[]  = Curve In BoundingBox{100-eps,-eps,-1,100+eps,90+eps,1};
top[]    = Curve In BoundingBox{-eps,90-eps,-1,100+eps,90+eps,1};
left[]   = Curve In BoundingBox{-eps,-eps,-1,eps,90+eps,1};

r_bottom[] = Curve In BoundingBox{25-eps,40-eps,-1,75+eps,40+eps,1};
r_right[]  = Curve In BoundingBox{75-eps,40-eps,-1,75+eps,60+eps,1};
r_top[]    = Curve In BoundingBox{25-eps,60-eps,-1,75+eps,60+eps,1};
r_left[]   = Curve In BoundingBox{25-eps,40-eps,-1,25+eps,60+eps,1};

Physical Curve("bottom") = {bottom[]};
Physical Curve("right")  = {right[]};
Physical Curve("top")    = {top[]};
Physical Curve("left")   = {left[]};

Physical Curve("bottom_res") = {r_bottom[]};
Physical Curve("right_res")  = {r_right[]};
Physical Curve("top_res")    = {r_top[]};
Physical Curve("left_res")   = {r_left[]};

// Fault
fault[] = Curve In BoundingBox
{
33.4,-0.1,-1,
66.6,90.1, 1
};

Physical Curve("Fault") = {fault[]};

// =====================================================
// Mesh
// =====================================================

Mesh.Algorithm = 6;
Mesh.CharacteristicLengthMin = 1.0;
Mesh.CharacteristicLengthMax = 10.0;