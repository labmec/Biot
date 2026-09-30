SetFactory("OpenCASCADE");

// =====================================================
// Mesh sizes
// =====================================================

lcFar   = 10.0;
lcRes   = 10.0;
lcFault = 10.0;

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

//Point(11) = {25,40,0,lcRes};
//Point(12) = {75,40,0,lcRes};
//Point(13) = {75,60,0,lcRes};
//Point(14) = {25,60,0,lcRes};

// =====================================================
// Surfaces
// =====================================================

// Far-field (outer region excluding the reservoir)
Plane Surface(1) = {1};

// =====================================================
// Fault (180 degrees)
// =====================================================

// Bottom intersection
Point(101) = {40.0,0.0,0,lcFault};

// Top intersection
Point(102) = {72.76,90.0,0,lcFault};

Line(101) = {101,102};

// =====================================================
// Fragment surfaces with the fault
// =====================================================

BooleanFragments
{
    Surface{1};
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

Physical Curve("bottom") = {bottom[]};
Physical Curve("right")  = {right[]};
Physical Curve("top")    = {top[]};
Physical Curve("left")   = {left[]};

// Fault
fault[] = Curve In BoundingBox
{
-0.1,40.,-1,
100.1,50., 1
};

Physical Curve("Fault") = {101};

// =====================================================
// Mesh
// =====================================================

Mesh.Algorithm = 6;
Mesh.CharacteristicLengthMin = 1.0;
Mesh.CharacteristicLengthMax = 4.0;//+
Show "*";
