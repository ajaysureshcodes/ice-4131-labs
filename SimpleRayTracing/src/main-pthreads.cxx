/*

Copyright (c) 2020, Dr Franck P. Vidal (f.vidal@bangor.ac.uk),
http://www.fpvidal.net/
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the Bangor University nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


/**
********************************************************************************
*
*   @file       main.cxx
*
*   @brief      A simple ray-tracer without parallelism.
*
*   @version    1.0
*
*   @date       14/10/2020
*
*   @author     Dr Franck P. Vidal
*
*   License
*   BSD 3-Clause License.
*
*   For details on use and redistribution please refer
*   to http://opensource.org/licenses/BSD-3-Clause
*
*   Copyright
*   (c) by Dr Franck P. Vidal (f.vidal@bangor.ac.uk),
*   http://www.fpvidal.net/, Oct 2020, 2020, version 1.0, BSD 3-Clause License
*
********************************************************************************
*/


//******************************************************************************
//  Include
//******************************************************************************
#include <iostream>  // for cerr
#include <exception> // to catch exceptions
#include <algorithm> // for max
#include <cmath>     // for pow
#include <limits>    // for inf
#include <stdexcept> // for exceptions
#include <sstream>   // to format error messages
#include <string>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include<pthread.h>
#include<vector>
#include<mutex>


#ifndef __Vec3_h
#include "Vec3.h"
#endif

#ifndef __Ray_h
#include "Ray.h"
#endif

#ifndef __Ray_h
#include "Ray.h"
#endif

#ifndef __TriangleMesh_h
#include "TriangleMesh.h"
#endif

#ifndef __Material_h
#include "Material.h"
#endif

#ifndef __Image_h
#include "Image.h"
#endif


//******************************************************************************
//  Namespace
//******************************************************************************
using namespace std;


//******************************************************************************
//  Function declarations
//******************************************************************************
void showUsage(const std::string& aProgramName);

void processCmd(int argc, char** argv,
                string& aFileName,
                unsigned int& aWidth, unsigned int& aHeight,
                unsigned char& r, unsigned char& g, unsigned char& b, unsigned int& t);

Vec3 applyShading(const Light& aLight,
                  const Material& aMaterial,
                  const Vec3& aNormalVector,
                  const Vec3& aPosition,
                  const Vec3& aViewPosition);

void loadMeshes(const std::string& aFileName,
                vector<TriangleMesh>& aMeshSet);

TriangleMesh createBackground(const Vec3& anUpperBBoxCorner,
                              const Vec3& aLowerBBoxCorner);

void getBBox(const vector<TriangleMesh>& aMeshSet,
             Vec3& anUpperBBoxCorner,
             Vec3& aLowerBBoxCorner);

void renderLoop(Image& anOutputImage,
                const vector<TriangleMesh>& aTriangleMeshSet,
                const Vec3& aDetectorPosition,
                const Vec3& aRayOrigin,
                const Vec3& anUpVector,
                const Vec3& aRightVector,
                const Light& aLight);



void* renderPartialImage(void* arg) {
    RenderThreadData* data = static_cast<RenderThreadData*>(arg);
    renderLoop(*data->output_image, *data->p_mesh_set, data->detector_position, data->origin, 
               data->up, data->right, *data->light, data->start_row, data->end_row);
    return nullptr;
}
std::mutex zbuffer_mutex;

// Thread callback function
void* callback(void* params) {
    RenderThreadData* data = (RenderThreadData*)params;
    // Process each row assigned to this thread
    for (int y = data->startRow; y < data->endRow; ++y) {
        for (int x = 0; x < data->outputImage->width(); ++x) {
            // Calculate ray and perform shading
            // Lock z-buffer access if required
            zbuffer_mutex.lock();
            // Render logic here (compute color, update z-buffer)
            zbuffer_mutex.unlock();
        }
    }
    pthread_exit(0);
}

//******************************************************************************
//  Constant global variables
//******************************************************************************
const unsigned int g_default_image_width  = 2048;
const unsigned int g_default_image_height = 2048;

const Vec3 g_black(0, 0, 0);
const Vec3 g_white(1, 1, 1);

const Vec3 g_red(1, 0, 0);
const Vec3 g_green(0, 1, 0);
const Vec3 g_blue(0, 0, 1);

const Vec3 g_background_colour = g_black;

//Declare a structure to hold the thread data and pass it to threads
struct RenderThreadData {
    int startRow;
    int endRow;
    Image* outputImage;
    const std::vector<TriangleMesh>* triangleMeshSet;
    Vec3 detectorPosition;
    Vec3 rayOrigin;
    Vec3 upVector;
    Vec3 rightVector;
    Light light;
    float* zBuffer;
    float pixelSpacing[2];
};


//-----------------------------
int main(int argc, char** argv) {
    try {
        // Output file and image properties
        std::string output_file_name = "test.jpg";
        unsigned int image_width = g_default_image_width;
        unsigned int image_height = g_default_image_height;

        // Background color
        unsigned char r = 128, g = 128, b = 128;

        // Number of threads (can be modified)
        unsigned int num_threads = 4; // Set desired number of threads

        // Process command-line arguments
        processCmd(argc, argv, output_file_name, image_width, image_height, r, g, b, num_threads);

        // Load the polygon meshes
        std::vector<TriangleMesh> p_mesh_set;
        loadMeshes("./dragon.ply", p_mesh_set);

        // Set material of the first mesh
        Material material(0.2 * g_red, g_green, g_blue, 1);
        p_mesh_set[0].setMaterial(material);

        // Get scene bounding box
        Vec3 lower_bbox_corner, upper_bbox_corner;
        getBBox(p_mesh_set, upper_bbox_corner, lower_bbox_corner);

        // Initialize ray-tracing properties
        Vec3 range = upper_bbox_corner - lower_bbox_corner;
        Vec3 bbox_centre = lower_bbox_corner + range / 2.0;
        float diagonal = range.getLength();

        Vec3 up(0.0, 0.0, -1.0);
        Vec3 origin = bbox_centre - Vec3(diagonal * 1, 0, 0);
        Vec3 detector_position = bbox_centre + Vec3(diagonal * 0.6, 0, 0);
        Vec3 direction = detector_position - origin;
        direction.normalise();

        Image output_image(image_width, image_height, r, g, b);

        Vec3 light_position = origin + up * 100.0;
        Vec3 light_direction = bbox_centre - light_position;
        light_direction.normalise();
        Light light(g_white, light_direction, light_position);

        Vec3 right = direction.crossProduct(up);

        // Add background mesh
        p_mesh_set.push_back(createBackground(upper_bbox_corner, lower_bbox_corner));

        // Thread data array and thread handles
        std::vector<pthread_t> threads(num_threads);
        std::vector<RenderThreadData> thread_data(num_threads);

        unsigned int rows_per_thread = image_height / num_threads;
        
        // Launch threads
        for (unsigned int i = 0; i < num_threads; ++i) {
            thread_data[i] = {
                &output_image, &p_mesh_set, detector_position, origin, up, right, &light,
                i * rows_per_thread, (i == num_threads - 1) ? image_height : (i + 1) * rows_per_thread
            };
            pthread_create(&threads[i], nullptr, renderPartialImage, &thread_data[i]);
        }

        // Join threads
        for (unsigned int i = 0; i < num_threads; ++i) {
            pthread_join(threads[i], nullptr);
        }

        // Save the rendered image
        output_image.saveJPEGFile(output_file_name);
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::string& e) {
        std::cerr << "ERROR: " << e << std::endl;
        return 2;
    }
    catch (const char* e) {
        std::cerr << "ERROR: " << e << std::endl;
        return 3;
    }

    return 0;
}


//******************************************************************************
//  Function definitions
//******************************************************************************


//---------------------------------------------
void showUsage(const std::string& aProgramName)
//---------------------------------------------
{
    std::cerr << "Usage: " << aProgramName << " <option(s)>" << endl <<
        "Options:" << endl <<
        "\t-h,--help\t\t\tShow this help message" << endl <<
        "\t-t,--threads T\tSpecify the number of threads (default value: 4)" << endl << 
        "\t-s,--size IMG_WIDTH IMG_HEIGHT\tSpecify the image size in number of pixels (default values: 2048 2048)" << endl << 
        "\t-b,--background R G B\t\tSpecify the background colour in RGB, acceptable values are between 0 and 255 (inclusive) (default values: 128 128 128)" << endl << 
        "\t-j,--jpeg FILENAME\t\tName of the JPEG file (default value: test.jpg)" << endl << 
        std::endl;
}


//-------------------------------------------------------------------
void processCmd(int argc, char** argv,
                string& aFileName,
                unsigned int& aWidth, unsigned int& aHeight,
                unsigned char& r, unsigned char& g, unsigned char& b,
                unsigned int& t)
//-------------------------------------------------------------------
{
    // Process the command line
    int i = 1;
    while (i < argc)
    {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help")
        {
            showUsage(argv[0]);
            exit(EXIT_SUCCESS);
        }
        else if (arg == "-s" || arg == "--size")
        {
            ++i;
            if (i < argc)
            {
                aWidth = stoi(argv[i]);
            }
            else
            {
                showUsage(argv[0]);
                exit(EXIT_FAILURE);
            }
            
            ++i;
            if (i < argc)
            {
                aHeight = stoi(argv[i]);
            }
            else
            {
                showUsage(argv[0]);
                exit(EXIT_FAILURE);
            }
        }
        else if (arg == "-b" || arg == "--background")
        {
            ++i;
            if (i < argc)
            {
                r = stoi(argv[i]);
            }
            else
            {
                showUsage(argv[0]);
                exit(EXIT_FAILURE);
            }

            ++i;
            if (i < argc)
            {
                g = stoi(argv[i]);
            }
            else
            {
                showUsage(argv[0]);
                exit(EXIT_FAILURE);
            }
            
            ++i;
            if (i < argc)
            {
                b = stoi(argv[i]);
            }
            else
            {
                showUsage(argv[0]);
                exit(EXIT_FAILURE);
            }
        }
        else if (arg == "-j" || arg == "--jpeg")
        {                
            ++i;
            if (i < argc)
            {
                aFileName = argv[i];
            }
            else
            {
                showUsage(argv[0]);
                exit(EXIT_FAILURE);
            }
        }
        else if (arg == "-t" || arg == "--threads")
        {
            ++i;
            if (i < argc)
            {
                t = stoi(argv[i]);
            }
            else
            {
                showUsage(argv[0]);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            showUsage(argv[0]);
            exit(EXIT_FAILURE);
        }
        ++i;
    }
}



//------------------------------------------
Vec3 applyShading(const Light& aLight,
                  const Material& aMaterial,
                  const Vec3& aNormalVector,
                  const Vec3& aPosition,
                  const Vec3& aViewPosition)
//------------------------------------------
{
    Vec3 ambient, diffuse, specular;

    // ambient
    ambient = aLight.getColour() * aMaterial.getAmbient();

    // diffuse
    Vec3 lightDir = (aLight.getPosition() - aPosition);
    lightDir.normalize();
    float diff = std::max(std::abs(aNormalVector.dotProduct(lightDir)), 0.0f);
    diffuse = aLight.getColour() * (diff * aMaterial.getDiffuse());

    // specular
    Vec3 viewDir(aViewPosition - aPosition);
    viewDir.normalize();

    Vec3 reflectDir = reflect(-viewDir, aNormalVector);
    float spec = std::pow(std::max(dot(viewDir, reflectDir), 0.0f), aMaterial.getShininess());
    specular = aLight.getColour() * (spec * aMaterial.getSpecular());

    return ambient + diffuse + specular;
}


//---------------------------------------------
void loadMeshes(const std::string& aFileName,
                                vector<TriangleMesh>& aMeshSet)
//-----------------------------==--------------
{
    // Create an instance of the Importer class
    Assimp::Importer importer;

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile( aFileName,
            aiProcess_CalcTangentSpace       |
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_SortByPType);

    // If the import failed, report it
    if( !scene)
    {
        std::stringstream error_message;
        error_message << importer.GetErrorString() << ", in File " << __FILE__ <<
                ", in Function " << __FUNCTION__ <<
                ", at Line " << __LINE__;

        throw std::runtime_error(error_message.str());
    }

    // Now we can access the file's contents.
    if (scene->HasMeshes())
    {
        aMeshSet.clear();

        for (int mesh_id = 0; mesh_id < scene->mNumMeshes; ++mesh_id)
        {
            aiMesh* p_mesh = scene->mMeshes[mesh_id];
            TriangleMesh mesh;

            // This is a triangle mesh
            if (p_mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE)
            {
                aiMaterial* p_mat = scene->mMaterials[p_mesh->mMaterialIndex];

                aiColor3D ambient, diffuse, specular;
                float shininess;

                p_mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
                p_mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
                p_mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
                p_mat->Get(AI_MATKEY_SHININESS, shininess);

                Material material;
                material.setAmbient(Vec3(ambient.r, ambient.g, ambient.b));
                material.setDiffuse(Vec3(diffuse.r, diffuse.g, diffuse.b));
                material.setSpecular(Vec3(specular.r, specular.g, specular.b));
                material.setShininess(shininess);

                mesh.setMaterial(material);

                // Load the vertices
                std::vector<float> p_vertices;
                for (unsigned int vertex_id = 0; vertex_id < p_mesh->mNumVertices; ++vertex_id)
                {
                    p_vertices.push_back(p_mesh->mVertices[vertex_id].x);
                    p_vertices.push_back(p_mesh->mVertices[vertex_id].y);
                    p_vertices.push_back(p_mesh->mVertices[vertex_id].z);
                }

                // Load indices
                std::vector<unsigned int> p_index_set;
                for (unsigned int index_id = 0; index_id < p_mesh->mNumFaces; ++index_id)
                {
                    if (p_mesh->mFaces[index_id].mNumIndices == 3)
                    {
                        p_index_set.push_back(p_mesh->mFaces[index_id].mIndices[0]);
                        p_index_set.push_back(p_mesh->mFaces[index_id].mIndices[1]);
                        p_index_set.push_back(p_mesh->mFaces[index_id].mIndices[2]);
                    }
                }
                mesh.setGeometry(p_vertices, p_index_set);
            }
            aMeshSet.push_back(mesh);
        }
    }
}


//----------------------------------------------------------
TriangleMesh createBackground(const Vec3& anUpperBBoxCorner,
                                const Vec3& aLowerBBoxCorner)
//----------------------------------------------------------
{
    Vec3 range = anUpperBBoxCorner - aLowerBBoxCorner;

    std::vector<float> vertices = {
        anUpperBBoxCorner[0] + range[0] * 0.1f, aLowerBBoxCorner[1] - range[1] * 0.5f, aLowerBBoxCorner[2] - range[2] * 0.5f,
        anUpperBBoxCorner[0] + range[0] * 0.1f, anUpperBBoxCorner[1] + range[1] * 0.5f, aLowerBBoxCorner[2] - range[2] * 0.5f,
        anUpperBBoxCorner[0] + range[0] * 0.1f, anUpperBBoxCorner[1] + range[1] * 0.5f, anUpperBBoxCorner[2] + range[2] * 0.5f,
        anUpperBBoxCorner[0] + range[0] * 0.1f, aLowerBBoxCorner[1] - range[1] * 0.5f, anUpperBBoxCorner[2] + range[2] * 0.5f,
    };

    std::vector<float> text_coords = {
        0, 1, 0,
        1, 1, 0,
        1, 0, 0,
        0, 0, 0,
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3,
    };

    TriangleMesh background_mesh(vertices, indices, text_coords);
    Image background_texture("background.jpg");
    background_mesh.setTexture(background_texture);

    return (background_mesh);
}


//------------------------------------------------
void getBBox(const vector<TriangleMesh>& aMeshSet,
                         Vec3& anUpperBBoxCorner,
                         Vec3& aLowerBBoxCorner)
//------------------------------------------------
{
    float inf = std::numeric_limits<float>::infinity();

    aLowerBBoxCorner = Vec3( inf,  inf,  inf);
    anUpperBBoxCorner = Vec3(-inf, -inf, -inf);

    for (std::vector<TriangleMesh>::const_iterator ite = aMeshSet.begin();
            ite != aMeshSet.end();
            ++ite)
    {
        Vec3 mesh_lower_bbox_corner = ite->getLowerBBoxCorner();
        Vec3 mesh_upper_bbox_corner = ite->getUpperBBoxCorner();

        aLowerBBoxCorner[0] = std::min(aLowerBBoxCorner[0], mesh_lower_bbox_corner[0]);
        aLowerBBoxCorner[1] = std::min(aLowerBBoxCorner[1], mesh_lower_bbox_corner[1]);
        aLowerBBoxCorner[2] = std::min(aLowerBBoxCorner[2], mesh_lower_bbox_corner[2]);

        anUpperBBoxCorner[0] = std::max(anUpperBBoxCorner[0], mesh_upper_bbox_corner[0]);
        anUpperBBoxCorner[1] = std::max(anUpperBBoxCorner[1], mesh_upper_bbox_corner[1]);
        anUpperBBoxCorner[2] = std::max(anUpperBBoxCorner[2], mesh_upper_bbox_corner[2]);
    }
}

// Thread function to process a subset of rows
void* renderThreadFunc(void* arg) {
    RenderThreadData* data = static_cast<RenderThreadData*>(arg);
    int width = data->outputImage->getWidth();

    for (int row = data->startRow; row < data->endRow; ++row) {
        for (int col = 0; col < width; ++col) {
            // Calculate pixel offsets
            float v_offset = data->pixelSpacing[1] * (0.5 + row - data->outputImage->getHeight() / 2.0);
            float u_offset = data->pixelSpacing[0] * (0.5 + col - data->outputImage->getWidth() / 2.0);

            // Initialize ray direction
            Vec3 direction = data->detectorPosition + data->upVector * v_offset + data->rightVector * u_offset - data->rayOrigin;
            direction.normalise();
            Ray ray(data->rayOrigin, direction);

            const TriangleMesh* intersectedObject = nullptr;
            const Triangle* intersectedTriangle = nullptr;
            float minT = std::numeric_limits<float>::infinity();

            // Iterate over each mesh and each triangle
            for (const auto& mesh : *data->triangleMeshSet) {
                if (mesh.intersectBBox(ray)) {
                    for (unsigned int triId = 0; triId < mesh.getNumberOfTriangles(); ++triId) {
                        const Triangle& triangle = mesh.getTriangle(triId);
                        float t;
                        if (ray.intersect(triangle, t) && t < minT) {
                            minT = t;
                            intersectedObject = &mesh;
                            intersectedTriangle = &triangle;
                        }
                    }
                }
            }

            // Update the pixel color if an intersection was found
            if (intersectedObject && intersectedTriangle) {
                int idx = row * width + col;
                if (data->zBuffer[idx] > minT) {
                    data->zBuffer[idx] = minT;
                    Vec3 pointHit = ray.getOrigin() + minT * ray.getDirection();
                    Material material = intersectedObject->getMaterial();
                    Vec3 color = applyShading(data->light, material, intersectedTriangle->getNormal(), pointHit, ray.getOrigin());
                    data->outputImage->setPixel(col, row, static_cast<unsigned char>(color[0]), 
                                                       static_cast<unsigned char>(color[1]), 
                                                       static_cast<unsigned char>(color[2]));
                }
            }
        }
    }
    pthread_exit(nullptr);
}



//-------------------------------------------------------------
// Main render loop using pthreads
void renderLoop(Image& anOutputImage,
                const std::vector<TriangleMesh>& aTriangleMeshSet,
                const Vec3& aDetectorPosition,
                const Vec3& aRayOrigin,
                const Vec3& anUpVector,
                const Vec3& aRightVector,
                const Light& aLight) 
{
    int numThreads = 32;
    pthread_t threads[numThreads];
    RenderThreadData threadData[numThreads];
    float inf = std::numeric_limits<float>::infinity();
    int height = anOutputImage.getHeight();
    int width = anOutputImage.getWidth();
    
    // Bounding box calculations
    Vec3 upper_bbox_corner, lower_bbox_corner;
    getBBox(aTriangleMeshSet, upper_bbox_corner, lower_bbox_corner);
    Vec3 range = upper_bbox_corner - lower_bbox_corner;

    // Pixel spacing
    float res1 = range[2] / width;
    float res2 = range[1] / height;
    float pixel_spacing[] = {2 * std::max(res1, res2), 2 * std::max(res1, res2)};
    
    // Initialize z-buffer
    std::vector<float> z_buffer(width * height, inf);

    int rowsPerThread = height / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        // Set the range of rows for each thread
        threadData[i].startRow = i * rowsPerThread;
        threadData[i].endRow = (i == numThreads - 1) ? height : (i + 1) * rowsPerThread;
        
        // Set other thread data parameters
        threadData[i].outputImage = &anOutputImage;
        threadData[i].triangleMeshSet = &aTriangleMeshSet;
        threadData[i].detectorPosition = aDetectorPosition;
        threadData[i].rayOrigin = aRayOrigin;
        threadData[i].upVector = anUpVector;
        threadData[i].rightVector = aRightVector;
        threadData[i].light = aLight;
        threadData[i].zBuffer = z_buffer.data();
        threadData[i].pixelSpacing[0] = pixel_spacing[0];
        threadData[i].pixelSpacing[1] = pixel_spacing[1];
        
        // Create thread
        pthread_create(&threads[i], nullptr, renderThreadFunc, &threadData[i]);
    }

    // Join threads
    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }
}