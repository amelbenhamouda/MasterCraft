#include <glimac/SDLWindowManager.hpp>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <GL/glew.h>
#include <iostream>
#include <glimac/glm.hpp>
#include "glimac/Image.hpp"
#include "glimac/Sphere.hpp"
#include "mastercraft/Cube.hpp"
#include "mastercraft/npc_cube.h"
#include "mastercraft/Tree.hpp"
#include "mastercraft/Texture_block.h"
#include "glm/gtc/random.hpp"
#include "glimac/FreeflyCamera.hpp"
#include "../src/glimac/stb_image.h"
#include <list>
#include <set>
#include "mastercraft/Chunk.hpp"
#include "mastercraft/MapChunk.hpp"
#include "mastercraft/Texture.hpp"
#include <functional>
#include <algorithm>
#include <vector>

using namespace glimac;
using namespace mastercraft;

const std::string TEXTURE_DIR = "../assets/textures";
const GLuint VERTEX_ATTR_POSITION = 0;
const GLuint VERTEX_ATTR_NORMAL = 1;
const GLuint VERTEX_ATTR_TEXT = 2;

struct Vertex2DUV {
    glm::vec2 position;
    glm::vec2 texture_pos;
    Vertex2DUV() {};
    Vertex2DUV(glm::vec2 _vecpos, glm::vec2 _vectext): position(_vecpos), texture_pos(_vectext) {};
};

glm::mat3 translate(float tx, float ty) {
    return glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(tx, ty, 1));
}

glm::mat3 scale(float sx, float sy) {
    return glm::mat3(glm::vec3(sx, 0, 0), glm::vec3(0, sy, 0), glm::vec3(0, 0, 1));
}

glm::mat3 rotat(float alpha) {
    alpha = glm::radians(alpha);
    return glm::mat3(glm::vec3(cos(alpha), sin(alpha), 0), glm::vec3(-sin(alpha), cos(alpha), 0), glm::vec3(0, 0, 1));
}

unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}

void activeskybox(const Skytext &skytext, const GLuint &cubemapTexture, float distRendu,
    float ratio_h_w, glm::mat4 VMatrix, const GLuint &texturebotom, const int &width, const int &height, const float size_cube, const int &column) {
    Texture tex;
    glm::mat4 MVMatrix, ProjMatrix, MVPMatrix, NormalMatrix;
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    skytext.m_Program.use();
    // ... définir la matrice de vue et projection
    ProjMatrix = glm::perspective(glm::radians(70.f), ratio_h_w, 0.1f, distRendu);
    MVMatrix = glm::mat4(glm::mat3(VMatrix)); // Remove translation from the MV
    MVPMatrix = ProjMatrix * MVMatrix;
    glUniformMatrix4fv(skytext.uMVP, 1, GL_FALSE, glm::value_ptr(MVPMatrix));

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
    //debindage de la texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

std::vector<int> colorMap(const std::vector<int> v) {
    std::vector<int> res;
    std::set<int> second (std::make_move_iterator(v.begin()), std::make_move_iterator(v.end()));
    std::set<int>::iterator it = second.begin();
    for(; it != second.end(); it++ ) {
        res.push_back(*it);
    }
    return res;
}

void activeChunks(const mastercraft::BlockText &btext, const mastercraft::SunText &suntext, const GLuint *texture, const FreeflyCamera &cam,
                  const std::vector<int> heigtmap, const GLsizei &count_vertex, const glm::mat4 &ProjMatrix, const MapChunk &mapchunk, 
                  const std::vector<int> lstColor, const float &size_cube,bool &nuit, const int &realtime, glm::vec4 &nigthandday, bool event_time,
                  int heigthmap_width,int heigthmap_height,bool torch,int &poswater, float distRendu) {
    suntext.m_Program.use();
	glActiveTexture(GL_TEXTURE0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glm::mat4  VMatrix /*,MVMatrix, NormalMatrix,MVPMatrix*/;
    int x = mapchunk.get_nbw(); // width
    int y = mapchunk.get_nbh(); // column
    int z = mapchunk.get_nbc(); // heigth
    VMatrix = cam.getViewMatrix();

    glm::vec3 currentColor;
    nigthandday = glm::rotate(glm::mat4(1), float(realtime) / 300 , glm::vec3(0, 0, 0.5)) * glm::vec4(1, 0, 1, 0);
    ///sun
    float dist = distRendu * 0.9;
    float event_sign = (event_time == 0 ? 1 : -1);
    GLfloat scale_cube = distRendu * 0.1 * size_cube;

    if (nigthandday.y * event_sign < 0) {
        glBindTexture(GL_TEXTURE_2D, texture[9]);
        glUniform1i(suntext.uTexture, 0);
        glUniform1f(suntext.uSize_cube, scale_cube);
        glUniform3fv(suntext.uColor, 1, glm::value_ptr(glm::vec3(0.9, 0.9, 1)));
        glUniform3f(suntext.uPosSun, -nigthandday.x * dist * event_sign, -nigthandday.y * dist * event_sign, -float(heigthmap_height) * size_cube / 2.f);
        glUniformMatrix4fv(suntext.uVMatrix, 1, GL_FALSE, glm::value_ptr(VMatrix));
        glUniformMatrix4fv(suntext.uPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindTexture(GL_TEXTURE_2D, 0);
    } 
    else {
        glUniform1f(suntext.uSize_cube, scale_cube);
        glUniform3fv(suntext.uColor, 1, glm::value_ptr(glm::vec3(1, 1, 1)));
        glUniform3f(suntext.uPosSun, nigthandday.x * dist * event_sign, nigthandday.y * dist*event_sign, -float(heigthmap_height) * size_cube / 2.f);
        glUniformMatrix4fv(suntext.uVMatrix, 1, GL_FALSE, glm::value_ptr(VMatrix));
        glUniformMatrix4fv(suntext.uPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);

    glm::vec4 lumdir = VMatrix * nigthandday;
    btext.m_Program.use();
    if (nigthandday.y * event_sign < 0) {
        glUniform3f(btext.uLightDir_vs, -lumdir.x * event_sign , -lumdir.y * event_sign , -lumdir.z * event_sign);
        nuit = true;
        currentColor = glm::vec3(0.2, 0.2, 0.25);
        glUniform3fv(btext.uColor, 1, glm::value_ptr(currentColor));
        glUniform3f(btext.uLightIntensity, 0.4, 0.4, 1);
        glUniform3f(btext.uKd, 0.8, 0.8, 0.8); // diffuse
        glUniform3f(btext.uKs, 0.2, 0.2, 0.2);
        glUniform1f(btext.uShininess, 128);
        if (torch) {
            glm::vec3 campos = cam.getPosition();
            glUniform3f(btext.uLightPos_vs, campos[0], campos[1], campos[2]);
            glUniform1f(btext.uTorchEnable, 1);
            glUniform3f(btext.uKd2, 0.8, 0.8, 0.8);
            glUniform3f(btext.uKs2, 0.2, 0.2, 0.2);
            glUniform3f(btext.uLightIntensity_lamp, 1, 1, 0);
            glUniform1f(btext.uShininess_lamp, 128);
        }
        else {
            glUniform1f(btext.uTorchEnable, 0);
        }
    }
    else {
        glUniform3f(btext.uLightDir_vs, lumdir.x * event_sign, lumdir.y * event_sign, lumdir.z*event_sign);
        nuit = false;
        currentColor = glm::vec3(1, 1, 1);
        glUniform3fv(btext.uColor, 1, glm::value_ptr(currentColor));
        glUniform3f(btext.uKd, 0.5, 0.5, 0.5); // diffuse
        glUniform3f(btext.uKs, 0, 0, 0);
        glUniform3f(btext.uLightIntensity, 0.5, 0.5, 0.5);
        glUniform1f(btext.uShininess, 128);
        glUniform1f(btext.uTorchEnable, 0);
    }

    glm::vec3 pos_cam = cam.getPosition();
    pos_cam[0] = std::round(pos_cam[0] / (Chunk::CHUNK_SIZE * size_cube));
    pos_cam[1] = std::floor(pos_cam[1] / (Chunk::CHUNK_SIZE * size_cube));
    pos_cam[2] = -std::round(pos_cam[2] / (Chunk::CHUNK_SIZE * size_cube));

    int nbchunk = 15;
    int nbchunk_h = 15;
    VMatrix = cam.getViewMatrix();
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    ///Timer pour l'eau
    if (realtime % 5 == 0) {
        poswater += 1;
        if (poswater == 1024) {
            poswater = 0;
        }
    }
    for (int i = pos_cam[0] - nbchunk; i <= pos_cam[0] + nbchunk; i++) {
        if (i < 0) i = 0;
        for (int j = pos_cam[2] - nbchunk; j <= pos_cam[2] + nbchunk; j++) {
            if (j < 0) j = 0;
            for (int k = pos_cam[1] - nbchunk_h; k <= pos_cam[1] + nbchunk_h; k++) {
                if (k < 0) {
                    k = 0;
                }
                if (i < 0 || i > x - 1 || j < 0 || j > y - 1 || k < 0 || k > z - 1) {
                    break;
                }
                if (mapchunk.get_map_chunk(i, j, k)->isdrawable() == true) {
                    mapchunk.get_map_chunk(i, j, k)->render_chunk(btext, VMatrix, ProjMatrix, size_cube, (float)poswater / 1024.f, currentColor);
                }
            }
        }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = pos_cam[0] - nbchunk; i <= pos_cam[0] + nbchunk; i++) {
        if (i < 0) {
            i = 0;
        }
        for (int j = pos_cam[2] - nbchunk; j <= pos_cam[2] + nbchunk; j++) {
            if (j < 0) {
                j = 0;
            }
            for (int k = pos_cam[1] - nbchunk_h; k <= pos_cam[1] + nbchunk_h; k++) {
                if (k < 0) {
                    k = 0;
                }
                if (i < 0 || i > x - 1 || j < 0 || j > y - 1 || k < 0 || k > z - 1) {
                    break;
                }
                if (mapchunk.get_map_chunk(i, j, k)->isdrawable() == true) {
                    mapchunk.get_map_chunk(i, j, k)->render_water(btext, VMatrix, ProjMatrix, size_cube, (float)poswater / 1024.f, currentColor);
                }
            }
        }
    }
    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_CULL_FACE);
}

std::vector<int> recupDataPicture(std::string picture, int* width, int* heigth, int* n) {
    unsigned char *mapdata = stbi_load(picture.c_str(), width, heigth, n, 1);
    if (!mapdata) {
        std::cerr << "loading image " << picture.c_str() << " error: " << stbi_failure_reason() << std::endl;
    }
    std::vector<int> lst;
    int size_data = (*width) * (*heigth);
    for (int i = 0; i < size_data; i++) {
        lst.push_back(mapdata[i] * 1);
    }
    return lst;
}

std::vector<glm::vec3> recupDataPictureColor(std::string picture, int* width, int* heigth, int* n) {
    unsigned char *data = stbi_load(picture.c_str(), width, heigth, n, 4);
    if (!data) {
        std::cerr << "loading image " << picture << " error: " << stbi_failure_reason() << std::endl;
    }
    unsigned int size = (*width) * (*heigth);
    auto scale = 1;
    std::vector<glm::vec3> ptr;
    for (auto i = 0; i < size; ++i) {
        auto offset = 4 * i;
        ptr.push_back(glm::vec3(data[offset] * scale, data[offset + 1] * scale, data[offset + 2] * scale));
    }
    stbi_image_free(data);
    return ptr;
}

std::vector<int> colorMap2( std::vector<glm::vec3> v) {
    std::vector<int> res;
    for (std::vector<glm::vec3>::iterator it = v.begin() ; it != v.end(); ++it) {
        if (*it == glm::vec3(200., 155., 40.)) { // Sable
            res.push_back(1);
        }
        else if (*it == glm::vec3(34., 177., 76.)) { // Herbe
            res.push_back(2);
        }
        else if (*it == glm::vec3(127., 127., 127.)) { // Cailloux
            res.push_back(3);
        }
        else { // Objet
            res.push_back(100);
        }
    }
    return res;
}

void copyvertex(const ShapeVertex *vertices, ShapeVertex *vercopy,int count_vertex) {
    for (auto i = 0; i < count_vertex; i++) {
        vercopy[i].normal = vertices[i].normal;
        vercopy[i].position = vertices[i].position;
        vercopy[i].texCoords = vertices[i].texCoords;
    }
}

int main(int argc, char** argv) {
	#ifdef _WIN32
	    freopen("CON", "w", stdout);
	    freopen("CON", "w", stderr);
	#endif // WIN32
    GLint test[4];
    glGetIntegerv(GL_VIEWPORT,test);
    Texture tex;
    int width_windows = 800;
    int height_windows = 800;
    float ratio_h_w = (float)width_windows / (float)height_windows;
    // Initialize SDL and open a window
    SDLWindowManager windowManager(width_windows, height_windows, "GLImac"); // fen^etre 800x600
    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    // initialise la bibliothèque GLEW.
    // Cette bibliothèque charge l'ensemble des fonctions OpenGL3.
    // Sans elle on peut considerer que l'on a accès qu'aux fonctions OpenGL2.

    if (GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

	// On affiche ensuite des informations sur OpenGL afin de vérifier que l'on a la bonne version:
    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    /*********************************
    // code initialisation des données
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/
    ////////////////////////////IMAGE ET SHADERS//////////////////////////////////
    std::unique_ptr<Image> Grass_side = tex.loadTexture("../assets/textures/GrassBlockSide.png");
    std::unique_ptr<Image> Grass_bot = tex.loadTexture("../assets/textures/Dirt.png");
    std::unique_ptr<Image> Grass_top = tex.loadTexture("../assets/textures/GrassBlock.png");
    std::unique_ptr<Image> Sand = tex.loadTexture("../assets/textures/Sand.png");
    std::unique_ptr<Image> Sea = tex.loadTexture("../assets/textures/Wool.png");
    std::unique_ptr<Image> Rock = tex.loadTexture("../assets/textures/Stone.png");
    std::unique_ptr<Image> Water = tex.loadTexture("../assets/textures/Water.bmp");
    std::unique_ptr<Image> Atlaschunk = tex.loadTexture("../assets/textures/AtlasChunk.png");
    std::unique_ptr<Image> Atlasmob = tex.loadTexture("../assets/textures/AtlasMob.png");
    std::unique_ptr<Image> Moon = tex.loadTexture("../assets/textures/Moon.png");

    GLuint textures[10], textureCloud, textureNigth; // elem texture
    tex.firstBindTexture(Grass_side, &textures[0]); // grass side
    tex.firstBindTexture(Grass_bot, &textures[1]); // grass bot
    tex.firstBindTexture(Grass_top, &textures[2]); // grass top
    tex.firstBindTexture(Sand, &textures[3]); // sand
    tex.firstBindTexture(Sea, &textures[4]); // sea
    tex.firstBindTexture(Rock, &textures[5]); // rock
    tex.firstBindTexture(Water, &textures[6]); // water
    tex.firstBindTexture(Atlaschunk, &textures[7]); // AtlastChunk
    tex.firstBindTexture(Atlasmob, &textures[8]); // Atlastmob
    tex.firstBindTexture(Moon, &textures[9]); // Moon

    // Texture jour Skybox
    std::vector<std::string> facesDay {
        TEXTURE_DIR + "/day/right.png",
        TEXTURE_DIR + "/day/left.png",
        TEXTURE_DIR + "/day/top.png",
        TEXTURE_DIR + "/day/bottom.png",
        TEXTURE_DIR + "/day/front.png",
        TEXTURE_DIR + "/day/back.png"
    };

    // Texture nuit Skybox
    std::vector<std::string> facesNigth {
        TEXTURE_DIR + "/nigth/right.png",
        TEXTURE_DIR + "/nigth/left.png",
        TEXTURE_DIR + "/nigth/top.png",
        TEXTURE_DIR + "/nigth/bottom.png",
        TEXTURE_DIR + "/nigth/front.png",
        TEXTURE_DIR + "/nigth/back.png"
    };
    textureCloud = loadCubemap(facesDay);
    textureNigth = loadCubemap(facesNigth);

    int heigthmap_width, heigthmap_heigth, heigthmap_n;
    std::vector<int> l_heightmap;
    int colormap_width, colormap_heigth, colormap_n;
    std::vector<glm::vec3> lst;
    int Column_skies, Column_map ;
    std::cout << "Choose map: 1 = 128x128x128 , 2 = 512x512x128" << std::endl;
    int mapchoice;
    float distRendu;
    std::cin >> mapchoice;
    while (mapchoice != 1 && mapchoice !=2){
        std::cout << "PLEASE choose map 1 or 2 : "<< std::endl;
        std::cout << "Choose map: 1 = 128x128x128 , 2 = 512x512x128" << std::endl;
        std::cin >> mapchoice;
    }
    if (mapchoice == 1) {
        // HeightMap
        l_heightmap = recupDataPicture("../assets/map/128x128/map128-heigthmap.bmp", &heigthmap_width, &heigthmap_heigth, &heigthmap_n);
        // ColorMap
        lst = recupDataPictureColor("../assets/map/128x128/map128-color-arbre.bmp", &colormap_width, &colormap_heigth, &colormap_n);
        Column_skies = 128;
        Column_map = 0.2 * Column_skies ;
        distRendu = 2500.0f;
    }
    else if (mapchoice == 2) {
        // HeightMap
        l_heightmap = recupDataPicture("../assets/map/512x512/map512-heigthmap.bmp", &heigthmap_width, &heigthmap_heigth, &heigthmap_n);
        // ColorMap
        lst = recupDataPictureColor("../assets/map/512x512/map512-color-arbre.bmp", &colormap_width, &colormap_heigth, &colormap_n);
        Column_skies = 128;
        Column_map = 0.7 * Column_skies ;
        distRendu = 6000.0f;
    }

    /////////////////////////////////////SHADERS////////////////////////////////////
    // Ici on appels les shaders
    FilePath applicationPath(argv[0]);
    mastercraft::BlockText blocktext(applicationPath);
    mastercraft::Skytext skytex(applicationPath);
    mastercraft::NpcText npctex(applicationPath);
    mastercraft::SunText suntext(applicationPath);
    glEnable(GL_DEPTH_TEST);
    /*permet d'activer le test de profondeur
    du GPU. Sans cet appel de fonction, certains triangles non visible
    viendraient recouvrir des triangles situés devant.*/

    // création de la caméra
    glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f), ratio_h_w, 0.1f, distRendu);
    glm::mat4 identity = glm::mat4(1.0);

    FreeflyCamera camera;
    ///////////////////Cube/////////////////////////////////
    float size_cube = 1;
    Cube cube(size_cube);
    Tree tree(size_cube);
    Cube cubeSkybox(size_cube);

    GLsizei count_vertex_skybox = cubeSkybox.getVertexCount();
    const ShapeVertex*  Datapointeur_skybox = cubeSkybox.getDataPointer();

    GLsizei count_vertex_tree = tree.getVertexCount();
    const ShapeVertex*  Datapointeur_tree = tree.getDataPointer();

    GLsizei count_vertex = cube.getVertexCount();
    const ShapeVertex*  Datapointeur = cube.getDataPointer();

    ShapeVertex vertices[count_vertex + count_vertex_tree + count_vertex_skybox];
    for (auto i = 0; i < count_vertex; i++) { // Cube
        vertices[i] = *Datapointeur;
        Datapointeur++;
    }

    for (auto i = 0; i < count_vertex_tree; i++) { // Tree
        vertices[i + count_vertex] = *Datapointeur_tree;
        Datapointeur_tree++;
    }
    std::vector<glm::vec3> Tree_pos = tree.getDataPosition();

    ShapeVertex verticesSkybox[count_vertex_skybox];
    for (auto i = 0; i < count_vertex_skybox; i++) { // Skybox
        verticesSkybox[i] = *Datapointeur_skybox;
        verticesSkybox[i].position.x -= 0.5;
        verticesSkybox[i].position.y -= 0.5;
        verticesSkybox[i].position.z += 0.5;
        Datapointeur_skybox++;
    }

    if (heigthmap_width % Chunk::CHUNK_SIZE != 0 || heigthmap_heigth % Chunk::CHUNK_SIZE != 0) {
        std::cout << "ERROR : HeigtMap must be a multiple of N=" << Chunk::CHUNK_SIZE << " in width and heigth, minimum size is NxN " << std::endl;
        return 0;
    }

    Chunk chunk1;

    int width_ground = heigthmap_width;
    int height_ground = heigthmap_heigth;
    MapChunk mapchunk = MapChunk(width_ground, height_ground, Column_skies);

    for (auto &it : l_heightmap) {
        it = it * Column_map / 255;
    }
    std::vector<int> l_colormap = colorMap2(lst);
    std::vector<int> lstColor = colorMap(l_colormap);
    mapchunk.compute(l_heightmap, l_colormap,Tree_pos);

    int dchunk = 0;
    int ndchunk = 0;
    for (int i = 0; i < mapchunk.get_nbw(); ++i) {
        for (int j = 0; j <  mapchunk.get_nbh(); ++j) {
            for (int k = 0; k <  mapchunk.get_nbc(); ++k) {
                if (mapchunk.get_map_chunk(i,j,k)->isdrawable() == true) dchunk++;
                else ndchunk++;
            }
        }
    }

    int x = mapchunk.get_nbw(); // width
    int y = mapchunk.get_nbh(); // heigth
    int z = mapchunk.get_nbc(); // column
    glm::vec3 xyz {x, y, z};

    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < z ; k++) {
                if (mapchunk.get_map_chunk(i, j, k)->isdrawable() == true) {
                    mapchunk.get_map_chunk(i, j, k)->setVertices(i, j, k, xyz, l_heightmap, count_vertex, count_vertex_tree, vertices, size_cube);
                }
            }
        }
    }

    /// Bind VBO for skybox
    GLuint vbo;
    glGenBuffers(1, &vbo);
    // Binding d'un VBO sur la cible GL_ARRAY_BUFFER:
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, count_vertex_skybox * sizeof(ShapeVertex), verticesSkybox, GL_STATIC_DRAW); // Envoi des données
    //Après avoir modifié le VBO, on le débind de la cible pour éviter de le remodifier par erreur
    glBindBuffer(GL_ARRAY_BUFFER, 0); // debind
    /// Bind VAO for skybox
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL); //1
    glEnableVertexAttribArray(VERTEX_ATTR_TEXT); //2
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)(offsetof(ShapeVertex, position)));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)(offsetof(ShapeVertex, normal)));
    glVertexAttribPointer(VERTEX_ATTR_TEXT, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)(offsetof(ShapeVertex, texCoords)));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
	/// Fin bind skybox

    float defaultspeed = 0.5;
    float speedcam = 0.5;
    float boost_speed = 0.5;

    bool Key_e_state = false;
    bool nuit = false;
    glm::vec4 nigthandday;
    GLuint textskyboxe;
    glm::vec3 current_pos = camera.getPosition();
    bool player_mode = false;
    bool Key_F5_state = false;

    ///time for nigth and day
    bool event_time = false;
    int realtime = 0;
    bool torch = false;
    bool torch_flag = false;
    //variable destruction / ajout block
    int xMouse, yMouse;
    GLfloat depth;
    float dist;
    bool blockchange = false;
    /// npc cube
    int nb_npc;
    if (mapchoice == 1) {
        nb_npc = 10;
    }
    else if (mapchoice == 2) {
        nb_npc = 100;
    }

    npc_cube npcs[nb_npc];
    srand(time(NULL));
    int poswater = 0; //water
    for (int i = 0; i < nb_npc; i++) {
        npcs[i] = npc_cube(vertices, count_vertex, l_heightmap, width_ground, height_ground);
    }

    bool done = false;
    glm::ivec2 lastmousePos;
    SDL_GetRelativeMouseState(NULL, NULL);
    bool flag = false;
    /// lambda func for immortal block (construction / destruction)
    std::vector<int> immortal_block = {51,50};
    std::function<bool(int)> testdraw = [&](int i) {
        return (std::find (immortal_block.begin(), immortal_block.end(), i ) != immortal_block.end());
    };

    int minwater = mapchunk.get_minwater();
    // La map étant un vecteur il faut calculer chaque emplacement ligne/collone/hauteur pour les placés correctement dans le vecteur

    while(!done) { // boucle tant que done == false
        // Event loop:
        SDL_Event e;
        while (windowManager.pollEvent(e)) {  //Boucle d'evenement
            if (e.type == SDL_QUIT || windowManager.isKeyPressed(SDLK_ESCAPE)) {
                done = true; // Leave the loop after this iteration
            }
            if (e.type == SDL_VIDEORESIZE) {
                width_windows = e.resize.w;
                height_windows = e.resize.h;
                glViewport(0, 0, (GLsizei) width_windows, (GLsizei) height_windows);
                ratio_h_w = (float)width_windows / (float)height_windows;
                ProjMatrix = glm::perspective(glm::radians(70.f), ratio_h_w, 0.1f, distRendu);
            }
        }
        //gestion caméra
        if (windowManager.isMouseButtonPressed(SDL_BUTTON_LEFT) == true) {
            SDL_GetRelativeMouseState(&lastmousePos.x, &lastmousePos.y);
            if (flag == true) {
                camera.rotateLeft(lastmousePos.x / 5);
                camera.rotateUp(lastmousePos.y / 5);
            }
            flag = true;
        }
        else {
            flag = false;
        }
        if (windowManager.isKeyPressed(SDLK_LCTRL) == true) {
            speedcam = defaultspeed * boost_speed;
        }
        else {
            speedcam = defaultspeed;
        }
        /* Pour avoir camera comme perso qui quitte pas la map */
        if (windowManager.isKeyPressed(SDLK_F5) == true ) {
            if (!Key_F5_state) {
                player_mode = !player_mode;
            }
            Key_F5_state = true;
        }
        else {
            Key_F5_state = false;
        }
        if (player_mode) {
            defaultspeed = 0.2;
            boost_speed = 2;
            current_pos = camera.getPosition();
            if (current_pos[0] <= 0.f) {
                current_pos[0] = 0.1;
            }
            if (-current_pos[2] <= 0.f) {
                current_pos[2] = -0.1;
            }
            if (current_pos[0] >= (float)heigthmap_width) {
                current_pos[0] = heigthmap_width - 0.1;
            }
            if (-current_pos[2] >= (float)heigthmap_heigth) {
                current_pos[2] = -(heigthmap_heigth - 0.1);
            }
            current_pos[1] = l_heightmap[std::floor(current_pos[0]) * heigthmap_width + std::floor(-current_pos[2])] + 3 * size_cube;
            camera.setPosition(current_pos);
        }
        else {
            defaultspeed = 0.5;
            boost_speed = 5;
        }

		#ifdef _WIN32
	        if (windowManager.isKeyPressed(SDLK_w) == true) {
	            camera.moveFront(speedcam);
	        }
	        if (windowManager.isKeyPressed(SDLK_a) == true) {
	            camera.moveLeft(speedcam);
	        }
		#else
	        if (windowManager.isKeyPressed(SDLK_z) == true) {
	            camera.moveFront(speedcam);
	        }
	        if (windowManager.isKeyPressed(SDLK_q) == true) {
	            camera.moveLeft(speedcam);
	        }
		#endif // WIN32

        if (windowManager.isKeyPressed(SDLK_s) == true) {
            camera.moveFront(-speedcam);
        }

        if (windowManager.isKeyPressed(SDLK_d) == true) {
            camera.moveLeft(-speedcam);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //////////////////////////////// TIMER

        if (windowManager.isKeyPressed(SDLK_e)) {
            if (!Key_e_state) {
                realtime = 0;
                nuit = !nuit;
                event_time = !event_time;
            }
            Key_e_state = true;
        }
        else {
            Key_e_state = false;
        }
        if (nuit == false ) {
            textskyboxe = textureCloud;
        }
        else {
            textskyboxe = textureNigth;
        }
        glm::mat4  VMatrix = camera.getViewMatrix();
        //////////////////////////////

        glBindVertexArray(vao);
        activeskybox(skytex, textskyboxe, distRendu, ratio_h_w, VMatrix, textures[6], width_ground, height_ground, size_cube, Column_skies);
        glBindVertexArray(0);
        /// npc draw
        for (int i = 0; i < nb_npc ; i++) {
            npcs[i].draw_npc(npctex, VMatrix, ProjMatrix, size_cube, textures[8], nuit, nigthandday, torch, camera.getPosition());
        }
        glBindVertexArray(vao);
        activeChunks(blocktext, suntext, textures, camera, l_heightmap, count_vertex, ProjMatrix, mapchunk, lstColor,
                     size_cube, nuit, realtime, nigthandday, event_time, heigthmap_width, heigthmap_heigth, torch, poswater,distRendu);
        glBindVertexArray(0);
        if (windowManager.isKeyPressed(SDLK_r) == true || windowManager.isKeyPressed(SDLK_t) == true) {
            if (blockchange == false) {
                glm::mat4 VMatrix = camera.getViewMatrix();
                SDL_GetMouseState(&xMouse,&yMouse);
                glReadPixels(xMouse, height_windows - yMouse - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
                glm::vec4 viewport = glm::vec4(0, 0, width_windows, height_windows);
                glm::vec3 wincoord = glm::vec3(xMouse, height_windows - yMouse - 1, depth);
                glm::vec3 objcoord = glm::unProject(wincoord, VMatrix, ProjMatrix, viewport); // projection inverse pour récupéré les coordonnées
                int chunkX = objcoord.x / Chunk::CHUNK_SIZE;
                int chunkY = objcoord.y / Chunk::CHUNK_SIZE;
                int chunkZ = objcoord.z / Chunk::CHUNK_SIZE;
                int posX = (int)objcoord.x % Chunk::CHUNK_SIZE;
                int posY = (int)objcoord.y % Chunk::CHUNK_SIZE;
                int posZ = (int)objcoord.z % Chunk::CHUNK_SIZE;
                dist = glm::distance(camera.getPosition(), objcoord);

                if (!testdraw(mapchunk.get_map_chunk(chunkX, -chunkZ, chunkY)->get_block(posX, -posZ, posY))) {
                    if (windowManager.isKeyPressed(SDLK_r) == true) {
                        int data = 0;
                        if (dist < 6 && std::floor(objcoord.y) > 3 && std::floor(objcoord.y) < Column_skies - 2) {
                            int id =std::floor(objcoord.x) * heigthmap_width + std::floor(-objcoord.z);
                            l_heightmap[id] -= 1;
                            mapchunk.get_map_chunk(chunkX, -chunkZ, chunkY)->update_block(posX, -posZ, posY, data, chunkX, -chunkZ, chunkY, xyz,
                                l_heightmap, count_vertex, count_vertex_tree, vertices, size_cube);
                            if (l_heightmap[id] == minwater){
                                mapchunk.get_map_chunk(chunkX, -chunkZ, chunkY)->update_block(posX, -posZ, posY, 50, chunkX, -chunkZ, chunkY, xyz,
                                    l_heightmap, count_vertex, count_vertex_tree, vertices, size_cube);
                            }
                        }
                    }
                    if (windowManager.isKeyPressed(SDLK_t) == true) {
                        int data = lstColor[2];
                        if (dist < 6 && std::floor(objcoord.y) > 2 && std::floor(objcoord.y) < Column_skies - 3) {
                            chunkY = (objcoord.y + 1) / Chunk::CHUNK_SIZE;
                            posY = (int)(objcoord.y + 1) % Chunk::CHUNK_SIZE;
                            mapchunk.get_map_chunk(chunkX, -chunkZ, chunkY)->update_block(posX, -posZ, posY, data, chunkX, -chunkZ, chunkY, xyz, l_heightmap,
                                count_vertex, count_vertex_tree, vertices, size_cube);
                            if (objcoord.y + 1 > l_heightmap[std::floor(objcoord.x) * heigthmap_width + std::floor(-objcoord.z)]) {
                                l_heightmap[std::floor(objcoord.x) * heigthmap_width + std::floor(-objcoord.z)] +=
                                    std::floor(objcoord.y + 1 - l_heightmap[std::floor(objcoord.x) * heigthmap_width + std::floor(-objcoord.z)]);
                            }
                        }
                    }
                    blockchange = true;
                } 
                else {
                    blockchange = false;
                }
            }
        }
        else {
            blockchange = false;
        }

        if (windowManager.isKeyPressed(SDLK_f) == true && nuit == true) {
           	if (!torch_flag) {
                torch = !torch;
                torch_flag = true;
            }
        }
        else{
            torch_flag = false;
        }

        // Update the display
        realtime += 1;
        // Enfin, l'appel à la méthode windowManager.swapBuffers(); met à jour l'affichage de la fenêtre en pratiquant le double buffering
        windowManager.swapBuffers(); 
    }

    glDeleteTextures(1, textures);
    glDeleteTextures(1, &textureCloud);
    glDeleteTextures(1, &textureNigth);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return EXIT_SUCCESS;
}
