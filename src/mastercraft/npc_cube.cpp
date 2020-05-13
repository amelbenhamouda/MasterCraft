#include "mastercraft/npc_cube.h"
#include <algorithm>

#ifndef M_PI
    #define M_PI 3.1415926535
#endif // M_PI

const GLuint VERTEX_ATTR_POSITION = 0;
const GLuint NORMAL_ATTR = 1;
const GLuint TEXTURE_ATTR_COORD = 2;

npc_cube::npc_cube() {}

npc_cube::~npc_cube() {}

npc_cube::npc_cube(const ShapeVertex *vertices, int countvertices, std::vector<int> &l_heightmap, int width_ground, int height_ground) : 
                                                                        heightmap(l_heightmap), posWmax(width_ground), posHmax(height_ground) {
    ShapeVertex tabloc[countvertices];
    for (int i = 0; i < 6; i++) {
        tabloc[i]= vertices[i];
        tabloc[i].texCoords[0] *= 0.5;
    }
    for (int i = 6; i < countvertices; i++) {
        tabloc[i]= vertices[i];
        tabloc[i].texCoords[0] *= 0.5;
        tabloc[i].texCoords[0] += 0.5;
    }
    vertices_npc = tabloc;
    nbvertices = countvertices;
    setVboVao_npc();
    ///generate random position in map
    int  rposX;
    int  rposZ;
    for (int i = 0; i < nbpos; i++) {
        rposX = rand() % width_ground + 1;
        rposZ = rand() % height_ground + 1;
        pos_npc.push_back(std::make_pair(rposX, rposZ));
    }
    posW = (GLfloat)pos_npc[0].first;
    posH = (GLfloat)pos_npc[0].second;
    double distTravel = sqrt(std::pow(posW - pos_npc[1].first, 2) + std::pow(posH-pos_npc[1].second, 2));
    tnpc = traveltime / distTravel;

    double Ux = pos_npc[1].first - posW;
    double Uy = pos_npc[1].second - posH;
    double Vx = 10;
    double Vy = 0;
    double distUV = sqrt(std::pow((posW + Vx) - pos_npc[1].first, 2) + std::pow((posH + Vy) - pos_npc[1].second, 2));
    double normU = sqrt(pow(Ux, 2) + pow(Uy, 2));
    double normV = sqrt(pow(Vx, 2) + pow(Vy, 2));
    int det = Ux * Vy - Uy * Vx;
    double angleRad = 0.5 * (pow(normU, 2) + pow(normV, 2) - pow(distUV, 2));
    angleRad = acos(angleRad / (normU * normV));
    angle_npc = copysign(angleRad, det);
    auto it = std::min_element(heightmap.begin(),heightmap.end());
    float minmap = *it;
    auto it2 = std::max_element(heightmap.begin(),heightmap.end());
    float maxmap = *it2;
    float factor = 0.5;
    /// La map étant un vecteur il faut calculer chaque emplacement ligne/collone/hauteur pour les placés correctement dans le vecteur
    minwater = int(minmap +(maxmap*factor));
}

void npc_cube::setVboVao_npc() {
    /// Bind VBO
    glGenBuffers(1, &vbo);
    // Binding d'un VBO sur la cible GL_ARRAY_BUFFER:
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, nbvertices * sizeof(ShapeVertex), vertices_npc, GL_STATIC_DRAW); // Envoi des données
    //Après avoir modifié le VBO, on le débind de la cible pour éviter de le remodifier par erreur

    glBindBuffer(GL_ARRAY_BUFFER, 0); // debind
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); // glBindVertexArray(GLuint array)

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION); //0
    glEnableVertexAttribArray(NORMAL_ATTR); //1
    glEnableVertexAttribArray(TEXTURE_ATTR_COORD); //2

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // la vao représente le maillage et doit ^etre bindé sur vbo
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)(offsetof(ShapeVertex, position)));  
    glVertexAttribPointer(NORMAL_ATTR, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)(offsetof(ShapeVertex, normal)));
    glVertexAttribPointer(TEXTURE_ATTR_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)(offsetof(ShapeVertex, texCoords))); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void npc_cube::draw_npc(const mastercraft::NpcText &npctex, const glm::mat4 &MVMatrix, const glm::mat4 &ProjMatrix, const float size_cube,
                        const GLuint &texture, const bool nuit, glm::vec4 &nigthandday,const bool torch,const glm::vec3 poscam) {   
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, texture);
    npctex.m_Program.use();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glUniform1i(npctex.uTexture, 0);
    glm::vec4 lumdir = MVMatrix * nigthandday;
    glUniform3f(npctex.uLightDir_vs, lumdir.x, lumdir.y, lumdir.z);
    glm::vec3 currentcolor;
    if (nuit) {
        currentcolor = glm::vec3(0.5, 0.5, 0.5 );
        glUniform3fv(npctex.uColor, 1, glm::value_ptr(currentcolor));
        glUniform3f(npctex.uKd, 0.5, 0.5, 0.5); // diffuse
        glUniform3f(npctex.uKs, 0, 0, 0);
        glUniform3f(npctex.uLightIntensity, 0.5, 0.5, 0.5);
        glUniform1f(npctex.uShininess, 32);
        if (torch) {
            glUniform1f(npctex.uTorchEnable, 1);
            glUniform3f(npctex.uLightPos_vs, poscam[0], poscam[1], poscam[2]);
            glUniform3f(npctex.uKd2, 0.6, 0.6, 0.6);
            glUniform3f(npctex.uKs2, 0.8, 0.8, 0.8);
            glUniform3f(npctex.uLightIntensity_lamp, 1, 1, 0);
            glUniform1f(npctex.uShininess_lamp, 32);
        } 
        else {
            glUniform1f(npctex.uTorchEnable, 0);
        }
    } 
    else {
        currentcolor = glm::vec3(1, 1, 1);
        glUniform3fv(npctex.uColor, 1, glm::value_ptr(currentcolor));
        glUniform3f(npctex.uKd, 0.5, 0.5, 0.5); // diffuse
        glUniform3f(npctex.uKs, 0, 0, 0);
        glUniform3f(npctex.uLightIntensity, 0.7, 0.7, 0.7);
        glUniform1f(npctex.uShininess, 128);
        glUniform1f(npctex.uTorchEnable, 0);
    }
    float tx = timer_npc * (float)pos_npc[pos_to_go].first + (1.0 - timer_npc) * posW;
    float tz = timer_npc * (float)pos_npc[pos_to_go].second + (1.0 - timer_npc) * posH;
    float ty = heightmap[std::round(tx) * posWmax + std::round(tz)];

    if (ty < minwater ) {
        glUniform3fv(npctex.uColor, 1, glm::value_ptr(glm::vec3(0.3, 0.6, 0.95) * currentcolor));
    }
    glm::mat4 rot = glm::rotate(glm::mat4(1), (float)M_PI / 2.f - angle_npc, glm::vec3(0, 1, 0));
    glUniform1f(npctex.uSize_cube, (GLfloat)size_cube);
    glUniformMatrix4fv(npctex.uRotate, 1, GL_FALSE, glm::value_ptr(rot));
    glUniform3fv(npctex.uTranslate, 1, glm::value_ptr(glm::vec3(tx * size_cube, ty * size_cube, -tz * size_cube)));
    glUniformMatrix4fv(npctex.uVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
    glUniformMatrix4fv(npctex.uPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix));
    glDrawArrays(GL_TRIANGLES, 0, nbvertices);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_CULL_FACE);
    timer_npc += tnpc;
    if (timer_npc > 1.0) {
        timer_npc = 0;
        int lastpos = pos_to_go;
        posW = pos_npc[lastpos].first;
        posH = pos_npc[lastpos].second;
        pos_to_go++;
        if (pos_to_go == nbpos) {
            pos_to_go = 0;
        }
        double distTravel = sqrt(std::pow(posW - pos_npc[pos_to_go].first, 2) + std::pow(posH - pos_npc[pos_to_go].second, 2));
        tnpc = traveltime / distTravel;

        double Ux = pos_npc[pos_to_go].first - posW;
        double Uy = pos_npc[pos_to_go].second - posH;
        double Vx = 10;
        double Vy = 0;
        double distUV = sqrt(std::pow((posW + Vx) - pos_npc[pos_to_go].first, 2) + std::pow((posH + Vy) - pos_npc[pos_to_go].second, 2));
        double normU = sqrt(pow(Ux, 2) + pow(Uy, 2));
        double normV = sqrt(pow(Vx, 2) + pow(Vy, 2));
        int det = Ux * Vy - Uy * Vx;
        double angleRad = 0.5 * (pow(normU, 2) + pow(normV, 2) - pow(distUV, 2));
        angleRad = acos(angleRad / (normU * normV));
        angle_npc = copysign(angleRad, det);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void npc_cube::setPosXYZ(uint8_t x, uint8_t y) {};
