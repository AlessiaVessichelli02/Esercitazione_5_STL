#pragma once
#include <iostream>
#include "PolygonalMesh.hpp"


using namespace std;

namespace PolygonalLibrary{
/// import the polygonal mesh and test if the mesh is correct
/// mesh: a PolygonalMesh struct
/// return the result of the reading, true if is correct,false otherwise
bool ImportMesh(const string &filepath , PolygonalMesh& mesh);

/// import the Cell0D properties from Cell1.csv file
/// mesh: a PolygovalMesh struct
/// return the result of the reading, true if is correct, false otherwise
bool ImportCell0Ds(const string &filepath, PolygonalMesh& mesh);

/// import the Cell1D properties from Cell1.csv file
/// mesh: a PolygonalMesh struct
/// return the result of the reading, true if is correct, else otherwise
bool ImportCell1Ds(const string &filepath, PolygonalMesh& mesh);

/// import the Cell2D properties from Cell2D.csv file
/// mesh: a PolygonalMesh struct
/// return the result of the reading, true if is correct, false otherwise
bool ImportCell2Ds(const string &filepath, PolygonalMesh& mesh);
}

double euclidean_distance(const Vector2d& v1, const Vector2d& v2);

double polygonal_area(const vector <Vector2d>& vertices);

double find_max(const double& a, const double& b, const double& c);


