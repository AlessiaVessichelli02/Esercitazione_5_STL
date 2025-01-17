#include "Utils.hpp"
#include "PolygonalMesh.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

//Calcolo la distanza euclidea tra due vettori bidimensionali
double euclidean_distance(const Vector2d& v1, const Vector2d& v2){
    double dist = sqrt(pow((v1[0] - v2[0]),2) + pow((v1[1]-v2[1]),2));
    return dist;
}

//Calcolo l'area di una poligonale utilizzando la formula gaussiana
double polygonal_area(const vector <Vector2d>& vertices){
    double area = 0;
    unsigned int n = vertices.size();
    for(unsigned int i = 0; i < n-1; i++)
    {
        area += vertices[i][0] * vertices[i+1][1] - vertices[i+1][0] * vertices[i][1];
    }
    area += vertices[n-1][0] * vertices[0][1] - vertices[n-1][1] * vertices[0][0];
    area *= 0.5;
    return area;
}

//Trovo il massimo tra tre valori doppi
double find_max(const double& a, const double& b, const double& c){
    double max_value = max(max(a,b),c);
    return max_value;
}


namespace PolygonalLibrary {

bool ImportMesh(const string &filepath, PolygonalMesh& mesh)
{
    //Verifico l'importazione di Cell0D e se è andata a buon fine stampo i marker
    if(!ImportCell0Ds(filepath + "/Cell0Ds.csv", mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell0D marker:" << endl;
        if(mesh.Cell0DMarkers.size() == 0)
        {
            cout << "Non ci sono markers diversi da 0 per Cell0D" << endl;
        }
        else
        {
            for(auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++) //auto è un tipo
            {
                cout << "key:\t" << it -> first << "\t values:";
                for(const unsigned int id : it -> second)
                    cout << "\t" << id;
                cout << endl;
            }

        }

    }

    //Verifico l'importazione di Cell1D e se è andata a buon fine stampo i marker
    if(!ImportCell1Ds(filepath + "/Cell1Ds.csv", mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell1D marker:" << endl;
        if(mesh.Cell1DMarkers.size() == 0)
        {
            cout << "Non ci sono markers diversi da 0 for Cell1D" << endl;
        }
        else
        {
            for(auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
            {
                cout << "key:\t" << it -> first << "\t values:";
                for(const unsigned int id : it -> second)
                    cout << "\t" << id;

                cout << endl;
            }
        }

    }

    //Verifico l'importazione di Cell2D
    if(!ImportCell2Ds(filepath + "/Cell2Ds.csv", mesh))
    {
        return false;
    }
    else
    {
        //Test: gli spigoli dei poligonali hanno lunghezza diversa da zero
        double toll_1D = pow(10,-15);

        for(unsigned int c = 0; c < mesh.NumberCell2D; c++)
        {
            Vector2i vert = mesh.Cell1DVertices[c];
            Vector2d origin = mesh.Cell0DCoordinates[vert[0]];
            Vector2d end = mesh.Cell0DCoordinates[vert[1]];

            double distanza = euclidean_distance(origin, end);
            if (distanza < toll_1D)
            {
                cerr << "Il lato i cui vertici sono " << origin << " e " << end << " ha lunghezza zero." << endl;
            }
        }

        //Test: l'area dei poligonali è diversa da zero
        double epsilon_macchina = numeric_limits<double>::epsilon();
        double area_min = pow(toll_1D, 2)/4*sqrt(3);
        double toll_2D = find_max(epsilon_macchina, area_min, toll_1D);

        for(unsigned int c = 0; c < mesh.NumberCell2D; c++)
        {
            vector<Vector2d> vertices;
            for(unsigned i = 0; mesh.Cell2DNumVertices; i++)
            {
                unsigned int vert_index = mesh.Cell2DVertices[c][i];
                vertices.push_back(mesh.Cell0DCoordinates[vert_index]);
                if (polygonal_area(vertices) < toll_2D)
                {
                    cerr << "C'è almeno un poligono con area nulla" << endl;
                }
            }
        }
    }

    return true;

}

bool ImportCell0Ds(const string &filename, PolygonalMesh& mesh)
{
    //Apro il file
    ifstream file;
    file.open(filename);

    //Verifico che l'apertura vada a suo fine
    if(file.fail())
        return false;

    //Leggo il file
    list<string> listLines;
    string line;
    while(getline(file, line))
        listLines.push_back(line);

    file.close();

    listLines.pop_front();

    mesh.NumberCell0D = listLines.size();

    if(mesh.NumberCell0D == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DId.reserve(mesh.NumberCell0D);
    mesh.Cell0DCoordinates.reserve(mesh.NumberCell0D);

    for(const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        char punto_virgola;
        Vector2d coord;

        converter >> id >> punto_virgola >> marker >> punto_virgola >> coord(0) >> punto_virgola >> coord(1);

        mesh.Cell0DId.push_back(id);
        mesh.Cell0DCoordinates.push_back(coord);

        //Test sui marker
        if(marker != 0) //Escudo la chiave 0
        {
            auto ret = mesh.Cell0DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first) -> second.push_back(id);
        }
    }
    file.close();
    return true;
}

bool ImportCell1Ds(const string &filename, PolygonalMesh& mesh)
{
    //Apro il file
    ifstream file;
    file.open(filename);

    //Verifico che l'apertura vada a suo fine
    if(file.fail())
        return false;

    //Leggo il file
    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    listLines.pop_front();

    mesh.NumberCell1D = listLines.size();

    if (mesh.NumberCell1D == 0)
    {
        cerr << "Non c'è nessuna cella 1D" << endl;
        return false;
    }
    mesh.Cell1DId.reserve(mesh.NumberCell1D);
    mesh.Cell1DVertices.reserve(mesh.NumberCell1D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        char punto_virgola;
        Vector2i vertices;

        converter >>id >> punto_virgola >> marker >> punto_virgola >> vertices(0) >> punto_virgola >> vertices(1);

        mesh.Cell1DId.push_back(id);
        mesh.Cell1DVertices.push_back(vertices);

        if( marker != 0)
        {
            auto ret = mesh.Cell1DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }
    }

    file.close();

    return true;
}


bool ImportCell2Ds(const string &filename,
                   PolygonalMesh& mesh)
{
    //Apro il file
    ifstream file;
    file.open(filename);

    //Verifico che l'apertura vada a suo fine
    if(file.fail())
        return false;

    //Leggo il file
    list<string> listLines; //nella lista colleziono tutti gli elementi che legge
    string line;
    while (getline(file, line))
        listLines.push_back(line); //metto nella lista tutto ciò che leggo

    listLines.pop_front(); //pop_front() serve a togliere il ptimo elemento

    mesh.NumberCell2D = listLines.size();

    if (mesh.NumberCell2D == 0)
    {
        cerr << "Non c'è nessuna cella 2D" << endl;
        return false;
    }

    mesh.Cell2DId.reserve(mesh.NumberCell2D); //reserve modifica la capacità e non la dimensione
    mesh.Cell2DVertices.reserve(mesh.NumberCell2D);
    mesh.Cell2DEdges.reserve(mesh.NumberCell2D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int mark;
        unsigned int NumVertices;
        unsigned int NumEdges;

        vector<unsigned int> vertices;
        vector<unsigned int> edges;

        converter >>  id >> mark >> NumVertices;
        for(unsigned int i = 0; i < NumVertices; i++)
            converter >> vertices[i];

        converter >> NumEdges;
        for(unsigned int i = 0; i < NumEdges; i++)
            converter >> edges[i];

        mesh.Cell2DId.push_back(id);
        mesh.Cell2DVertices.push_back(vertices);
        mesh.Cell2DEdges.push_back(edges);
    }
    file.close();
    return true;
}
}

