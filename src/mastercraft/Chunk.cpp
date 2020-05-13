#include "mastercraft/Chunk.hpp"
#include <assert.h>
#include <numeric>
#include <algorithm>
#include <functional>

const GLuint VERTEX_ATTR_POSITION = 0;
const GLuint NORMAL_ATTR = 1;
const GLuint TEXTURE_ATTR_COORD = 2;

std::ostream &operator<<(std::ostream &os, const Chunk &_c) {
    for (uint8_t i = 0; i < _c.CHUNK_SIZE; i++) {
        for (uint8_t j = 0; j < _c.CHUNK_SIZE; j++) {
            for (uint8_t k = 0; k < _c.CHUNK_SIZE; k++) {
                os << unsigned(_c.m_pBlocks[i][j][k]) << " ";
            }
        }
    }
    os <<std::endl;
}

Chunk::Chunk() { // Create the blocks
    std::vector<bool> drawface {1, 1, 1, 1, 1, 1};
    for (uint8_t i = 0; i < CHUNK_SIZE; i++) {
        for (uint8_t j = 0; j < CHUNK_SIZE; j++) {
            for (uint8_t k = 0; k < CHUNK_SIZE; k++) {
                m_pBlocks[i][j][k] = 0;
                facetodraw.push_back(drawface);
            }
        }
    }
}

Chunk::~Chunk() { // Delete the blocks
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Chunk::create_block(uint8_t x, uint8_t y, uint8_t z, uint8_t data) {
    assert(x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE);
    m_pBlocks[x][z][y] = data;
}

void Chunk::update_block(uint8_t x, uint8_t y, uint8_t z, uint8_t data, const int i, const int j, const int k, glm::vec3 xyz, 
                         const std::vector<int> &l_heightmap, const GLsizei count_vertex, const GLsizei count_vertex_tree, const ShapeVertex *vertices, 
                         const float size_cube) {
    assert(x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE);
    m_pBlocks[x][z][y] = data;
    if (x == 0 && pleft != NULL) {
        pleft->Check_chunk();
        pleft->setVertices(i - 1, j, k, xyz, l_heightmap, count_vertex, count_vertex_tree, vertices, size_cube);
    }
    if (x == CHUNK_SIZE - 1 && pright != NULL) {
        pright->Check_chunk();
        pright->setVertices(i + 1, j, k, xyz, l_heightmap, count_vertex, count_vertex_tree, vertices, size_cube);
    }
    if (y == 0 && pface != NULL) {
        pface->Check_chunk();
        pface->setVertices(i, j - 1, k, xyz, l_heightmap, count_vertex, count_vertex_tree, vertices, size_cube);
    }
    if (y == CHUNK_SIZE - 1 && pback != NULL) {
        pback->Check_chunk();
        pback->setVertices(i, j + 1, k, xyz, l_heightmap, count_vertex, count_vertex_tree, vertices, size_cube);
    }
    if ( z == 0 && pbot != NULL) {
        pbot->Check_chunk();
        pbot->setVertices(i, j, k - 1, xyz, l_heightmap, count_vertex, count_vertex_tree, vertices, size_cube);
    }
    if (z == CHUNK_SIZE - 1 && ptop != NULL) {
        ptop->Check_chunk();
        ptop->setVertices(i, j, k + 1, xyz, l_heightmap, count_vertex, count_vertex_tree, vertices, size_cube);
    }
    Check_chunk();
    setVertices(i, j, k, xyz, l_heightmap, count_vertex, count_vertex_tree, vertices, size_cube);
}

uint8_t Chunk::get_block(uint8_t x, uint8_t y, uint8_t z) {
    assert(x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE);
    return m_pBlocks[x][z][y];
}

void Chunk::setVertices(const int i, const int j, const int k, glm::vec3 xyz, const std::vector<int> &l_heightmap,const GLsizei count_vertex,
                        const GLsizei count_vertex_tree, const ShapeVertex *vertices, const float size_cube) {
    std::vector<ShapeVertex> verticesChunk;
    std::vector<ShapeVertex> verticesUnderWater;
    std::vector<ShapeVertex> verticesWater;
    std::vector<std::vector<bool>> facetodraw = getfacetodraw();
    std::vector<bool> cubetodraw = getcubetodraw();
    int x = xyz[0];

    auto it = std::min_element(l_heightmap.begin(), l_heightmap.end());
    float minmap = *it;
    auto it2 = std::max_element(l_heightmap.begin(), l_heightmap.end());
    float maxmap = *it2;
    float factor = 0.5;
    // La map étant un vecteur il faut calculer chaque emplacement ligne/collone/hauteur pour les placés correctement dans le vecteur
    static int minwater = int(minmap +(maxmap*factor));
    //int minwater = MapChunk::minwater;
    float tex_y = 32.f / 1024.f;
    float tex_x = 32.f / 128.f;
    float tol_x = 0.05 * tex_x;
    float tol_y = 0.05 * tex_y;

    for (uint8_t x_chunk = 0; x_chunk < Chunk::CHUNK_SIZE; x_chunk++) {
        int tx = get_posW() + x_chunk;
        for (uint8_t y_chunk = 0; y_chunk < Chunk::CHUNK_SIZE; y_chunk++) {
            int tz = get_posH() + y_chunk;
            int indice = (i * Chunk::CHUNK_SIZE + x_chunk) * Chunk::CHUNK_SIZE * x + (j * Chunk::CHUNK_SIZE + y_chunk);
            for (uint8_t z_chunk = 0; z_chunk < Chunk::CHUNK_SIZE; z_chunk++) {
                if (cubetodraw[x_chunk * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE + y_chunk * Chunk::CHUNK_SIZE + z_chunk] == true) {
                    int idc_color = get_block(x_chunk, y_chunk, z_chunk);
                    int heigth_loc = k * Chunk::CHUNK_SIZE + z_chunk;
                    int ty = get_posC()+ z_chunk;
                    ShapeVertex verticesloc[count_vertex];
                    for (auto i = 0; i < count_vertex; i++) {
                        verticesloc[i].normal = vertices[i].normal;
                        verticesloc[i].position = vertices[i].position;
                        verticesloc[i].texCoords = vertices[i].texCoords;
                    }
                    for (int i_v = 0; i_v < count_vertex; i_v++) {
                        verticesloc[i_v].position[0] += tx * size_cube;
                        verticesloc[i_v].position[1] += ty * size_cube;
                        verticesloc[i_v].position[2] -= tz * size_cube;
                    }
                    if (idc_color == 1) { //sand
                        for (int i = 0; i < 6; i++) {
                            if (facetodraw[x_chunk * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE + y_chunk * Chunk::CHUNK_SIZE + z_chunk][i] == 1) {
                                for (int j = i * 6; j < i * 6 + 6; j++) {
                                    verticesloc[j].texCoords[0] *= tex_x - tol_x;
                                    verticesloc[j].texCoords[1] *= tex_y - tol_y;
                                    verticesloc[j].texCoords[1] += tex_y + tol_y * 0.5;
                                    verticesloc[j].texCoords[0] += tol_x*0.5;
                                    if (heigth_loc <= minwater){
                                        verticesUnderWater.push_back(verticesloc[j]);
                                    }
                                    else {
                                        verticesChunk.push_back(verticesloc[j]);
                                    }
                                }
                            }
                        }
                    }
                    else if (idc_color == 2) { // herbe
                        if (heigth_loc == l_heightmap[indice] - 1) {
                            if (facetodraw[x_chunk * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE + y_chunk * Chunk::CHUNK_SIZE + z_chunk][4] == 1) {
                                for (int j = 24; j < 30; j++) {
                                    verticesloc[j].texCoords[0] *= tex_x - tol_x;
                                    verticesloc[j].texCoords[1] *= tex_y - tol_y;
                                    verticesloc[j].texCoords[1] += tol_y * 0.5;
                                    verticesloc[j].texCoords[0] += tol_x * 0.5;
                                    if (heigth_loc <= minwater) {
                                        verticesUnderWater.push_back(verticesloc[j]);
                                    }
                                    else {
                                        verticesChunk.push_back(verticesloc[j]);
                                    }
                                }
                            }
                            if (facetodraw[x_chunk * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE + y_chunk * Chunk::CHUNK_SIZE + z_chunk][5] == 1) {
                                for (int j = 30; j < 36; j++) {
                                    verticesloc[j].texCoords[0] *= tex_x - tol_x;
                                    verticesloc[j].texCoords[1] *= tex_y - tol_y;
                                    verticesloc[j].texCoords[0] += 2 * tex_x + tol_x * 0.5;
                                    verticesloc[j].texCoords[1] += tol_y * 0.5;
                                    if (heigth_loc <= minwater) {
                                        verticesUnderWater.push_back(verticesloc[j]);
                                    }
                                    else {
                                        verticesChunk.push_back(verticesloc[j]);
                                    }
                                }
                            }
                            for (int i = 0; i < 4; i++) {
                                if (facetodraw[x_chunk * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE + y_chunk * Chunk::CHUNK_SIZE + z_chunk][i] == 1) {
                                    for (int j = i * 6; j < i * 6 + 6; j++) {
                                        verticesloc[j].texCoords[0] *= tex_x - tol_x;
                                        verticesloc[j].texCoords[1] *= tex_y - tol_y;
                                        verticesloc[j].texCoords[0] += tex_x + tol_x * 0.5;
                                        verticesloc[j].texCoords[1] += tol_y * 0.5;
                                        if (heigth_loc <= minwater) {
                                            verticesUnderWater.push_back(verticesloc[j]);
                                        }
                                        else {
                                            verticesChunk.push_back(verticesloc[j]);
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            for (int i = 0; i < 6; i++) {
                                if (facetodraw[x_chunk * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE + y_chunk * Chunk::CHUNK_SIZE + z_chunk][i] == 1) {
                                    for (int j = i * 6; j < i * 6 + 6; j++) {
                                        verticesloc[j].texCoords[0] *= tex_x - tol_x;
                                        verticesloc[j].texCoords[1] *= tex_y - tol_y;
                                        verticesloc[j].texCoords[0] += 2 * tex_x + tol_x * 0.5;
                                        verticesloc[j].texCoords[1] += tol_y * 0.5;
                                        if (heigth_loc<= minwater) {
                                            verticesUnderWater.push_back(verticesloc[j]);
                                        }
                                        else {
                                            verticesChunk.push_back(verticesloc[j]);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (idc_color == 3) { //cailloux
                        for (int i = 0; i < 6; i++) {
                            if (facetodraw[x_chunk * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE + y_chunk * Chunk::CHUNK_SIZE + z_chunk][i] == 1) {
                                for (int j = i * 6; j < i * 6 + 6; j++) {
                                    verticesloc[j].texCoords[0] *= tex_x- tol_x;
                                    verticesloc[j].texCoords[1] *= tex_y - tol_y;
                                    verticesloc[j].texCoords[0] += tol_x * 0.5;
                                    verticesloc[j].texCoords[1] += 2 * tex_y + tol_y * 0.5;
                                    if (heigth_loc<= minwater) {
                                        verticesUnderWater.push_back(verticesloc[j]);
                                    }
                                    else {
                                        verticesChunk.push_back(verticesloc[j]);
                                    }
                                }
                            }
                        }
                    }
                    else if (idc_color == 100) { // arbre
                        int begin_tree = count_vertex ;
                        for (int i = 0; i < 6; i++) {
                            if (facetodraw[x_chunk * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE + y_chunk * Chunk::CHUNK_SIZE + z_chunk][i] == 1) {
                                for (int j = i * 6; j < i * 6 + 6; j++) {
                                    verticesloc[j].texCoords[0] *= tex_x - tol_x;
                                    verticesloc[j].texCoords[1] *= tex_y - tol_y;
                                    verticesloc[j].texCoords[0] += 2 * tex_x + tol_x * 0.5;
                                    verticesloc[j].texCoords[1] += tex_y + tol_y * 0.5;
                                    verticesChunk.push_back(verticesloc[j]);
                                }
                            }
                        }
                    }
                    else if (idc_color == 101) {
                        int begin_tree = count_vertex + 72;
                        for(int i = 0; i < 6; i++) {
                            if (facetodraw[x_chunk * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE + y_chunk * Chunk::CHUNK_SIZE + z_chunk][i] == 1) {
                                for (int j = i * 6; j < i * 6 + 6; j++) {
                                    verticesloc[j].texCoords[0] *= tex_x - tol_x;
                                    verticesloc[j].texCoords[1] *= tex_y - tol_y;
                                    verticesloc[j].texCoords[0] += tex_x + tol_x * 0.5;
                                    verticesloc[j].texCoords[1] += tex_y + tol_y * 0.5;
                                    verticesChunk.push_back(verticesloc[j]);
                                }
                            }
                        }
                    }
                    else if (idc_color == 50) { // water
                        for (int i = 24; i < 30; i++) {
                            verticesloc[i].texCoords[0] *= tex_x- tol_x;
                            verticesloc[i].texCoords[1] *= tex_y - tol_y;
                            verticesloc[i].texCoords[0] += 3 * tex_x + tol_x * 0.5;
                            verticesloc[i].texCoords[1] += tol_y * 0.5;
                            verticesWater.push_back(verticesloc[i]);
                        }
                    }
                    else if (idc_color == 51) {
                        for (int i = 0; i < 36; i++) {
                            verticesloc[i].texCoords[0] *= tex_x- tol_x;
                            verticesloc[i].texCoords[1] *= tex_y - tol_y;
                            verticesloc[i].texCoords[0] += 3 * tex_x + tol_x * 0.5;
                            verticesloc[i].texCoords[1] += tol_y * 0.5;
                            verticesWater.push_back(verticesloc[i]);
                        }
                    }
                } // if
            }//end z
        }//end y
    }//end x

    nbvertices = verticesChunk.size();
    nbvertices_under_water = verticesUnderWater.size();
    nbvertices_water = verticesWater.size();
    verticesChunk.insert(verticesChunk.end(), verticesUnderWater.begin(), verticesUnderWater.end());
    verticesChunk.insert(verticesChunk.end(), verticesWater.begin(), verticesWater.end());
    setVaoandVbo(verticesChunk);
}

void Chunk::setVaoandVbo(const std::vector<ShapeVertex> vertices) {
    /// Bind VBO
	glGenBuffers(1, &vbo);
	// Binding d'un VBO sur la cible GL_ARRAY_BUFFER:
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    ShapeVertex vertices_chunk[vertices.size()];
	for (int i = 0; i < vertices.size(); i++) {
        vertices_chunk[i] = vertices[i];
	}

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof (ShapeVertex), vertices_chunk, GL_STATIC_DRAW); // Envoi des données
	//Après avoir modifié le VBO, on le débind de la cible pour éviter de le remodifier par erreur

	glBindBuffer(GL_ARRAY_BUFFER, 0); // debind
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao); 

	glEnableVertexAttribArray(VERTEX_ATTR_POSITION); //0
    glEnableVertexAttribArray(NORMAL_ATTR);//1
    glEnableVertexAttribArray(TEXTURE_ATTR_COORD);//2

   	glBindBuffer(GL_ARRAY_BUFFER, vbo);
   	// la vao représente le maillage et doit ^etre bindé sur vbo

   	glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof (ShapeVertex), (const GLvoid*)(offsetof (ShapeVertex, position)));
    glVertexAttribPointer(NORMAL_ATTR, 3, GL_FLOAT, GL_FALSE, sizeof (ShapeVertex),  (const GLvoid*)(offsetof (ShapeVertex, normal)));
    glVertexAttribPointer(TEXTURE_ATTR_COORD, 2, GL_FLOAT, GL_FALSE, sizeof (ShapeVertex), (const GLvoid*)(offsetof (ShapeVertex, texCoords)));
   	glBindBuffer(GL_ARRAY_BUFFER, 0);
   	glBindVertexArray(0);
}

void Chunk::render_chunk(const mastercraft::BlockText &btext, glm::mat4 &VMatrix, const glm::mat4 &ProjMatrix,
                         const int size_cube, const float poswater, const glm::vec3 &currentColor) {
    glBindVertexArray(vao);
    glUniformMatrix4fv(btext.uVMatrix, 1, GL_FALSE, glm::value_ptr(VMatrix));
    glUniformMatrix4fv(btext.uPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix));
    glUniform1i(btext.uTexture, 0);
    glUniform3fv(btext.uColor, 1, glm::value_ptr(currentColor));
    glDrawArrays(GL_TRIANGLES, 0, nbvertices);
    glUniform3fv(btext.uColor, 1, glm::value_ptr(glm::vec3(0.3, 0.6, 0.95) * currentColor));
    glDrawArrays(GL_TRIANGLES, nbvertices, nbvertices_under_water);
    glUniform3fv(btext.uColor, 1, glm::value_ptr(currentColor));
    glBindVertexArray(0);
}

void Chunk::render_water(const mastercraft::BlockText &btext, glm::mat4 &VMatrix, const glm::mat4 &ProjMatrix, const int size_cube, const float poswater, 
                         const glm::vec3 &currentColor) {
    glBindVertexArray(vao);
    glDisable(GL_CULL_FACE);
    glUniform1i(btext.uAnimated, true);
    glUniform1f(btext.uPoswater, poswater);
    glUniform3fv(btext.uColor, 1, glm::value_ptr(glm::vec3(0.3, 0.6, 0.95) * currentColor));
    glDrawArrays(GL_TRIANGLES, nbvertices + nbvertices_under_water, nbvertices + nbvertices_under_water + nbvertices_water);
    glUniform1i(btext.uAnimated, false);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBindVertexArray(0);
}

void Chunk::Check_chunk() {
    bool draw = false;
    std::vector<bool> cubedraw;
    std::vector<bool> drawface {1, 1, 1, 1, 1, 1};
    std::vector<std::vector<bool>> init_facetodrawloc;
    for (uint8_t i = 0; i < CHUNK_SIZE; i++) {
        for (uint8_t j = 0; j < CHUNK_SIZE; j++) {
            for (uint8_t k = 0; k < CHUNK_SIZE; k++) {
                init_facetodrawloc.push_back(drawface);
            }
        }
    }
    std::vector<int> cube_to_pass = {0, 50};
    std::function<bool(int)> testdraw = [&](int i) {
        return (std::find (cube_to_pass.begin(), cube_to_pass.end(), i ) != cube_to_pass.end());
    };

    facetodraw = init_facetodrawloc;
    for (uint8_t i = 0; i < CHUNK_SIZE; i++) {
        for (uint8_t j = 0; j < CHUNK_SIZE; j++) {
            for (uint8_t k = 0; k < CHUNK_SIZE; k++) {
                if (m_pBlocks[i][k][j] != 0) {
                    draw = true;
                    // on test chaque arrête
                    if (i == 0 && k == 0 && j != 0 && j != CHUNK_SIZE - 1) { // 1
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (pleft != NULL) {
                            if (!testdraw(pleft->get_block(CHUNK_SIZE - 1, j, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (pbot != NULL) {
                            if (!testdraw(pbot->get_block(i, j, CHUNK_SIZE - 1))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    else if (i == 0 && j == 0 && k != 0 && k != CHUNK_SIZE - 1) { // 2
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (pleft != NULL) {
                            if (!testdraw(pleft->get_block(CHUNK_SIZE - 1, j, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (pface != NULL) {
                            if (!testdraw(pface->get_block(i, CHUNK_SIZE - 1, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; // face
                            }
                        }
                    }
                    else if (i == 0 && k == CHUNK_SIZE - 1 && j != 0 && j != CHUNK_SIZE - 1) { // 3
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (pleft != NULL) {
                            if (!testdraw(pleft->get_block(CHUNK_SIZE - 1, j, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                            }
                        }
                        if (ptop != NULL) {
                            if (!testdraw(ptop->get_block(i, j, 0) )) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])){
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    else if (i == 0 && j == CHUNK_SIZE - 1 && k != 0 && k != CHUNK_SIZE - 1) { // 4
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (pleft != NULL) {
                            if (!testdraw(pleft->get_block(CHUNK_SIZE - 1, j, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (pback != NULL) {
                            if (!testdraw(pback->get_block(i, 0, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    // 5
                    else if (i == CHUNK_SIZE - 1 && k == 0 && j != 0 && j != CHUNK_SIZE - 1) {
                        if (pright != NULL) {
                            if (!testdraw(pright->get_block(0, j, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                            }
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (pbot != NULL) {
                            if (!testdraw(pbot->get_block(i, j, CHUNK_SIZE - 1))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    else if (i == CHUNK_SIZE - 1 && j == 0 && k != 0 && k != CHUNK_SIZE - 1) { // 6
                        if (pright != NULL) {
                            if (!testdraw(pright->get_block(0, j, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                            }
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (pface != NULL) {
                            if (!testdraw(pface->get_block(i, CHUNK_SIZE - 1, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; // gauche
                            }
                        }
                    }
                    else if (i == CHUNK_SIZE - 1 && k == CHUNK_SIZE - 1 && j != 0 && j != CHUNK_SIZE - 1) { // 7
                        if (pright != NULL) {
                            if (!testdraw(pright->get_block(0, j, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                            }
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (ptop != NULL) {
                            if (!testdraw(ptop->get_block(i, j, 0))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // top
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    else if (i == CHUNK_SIZE - 1 && j == CHUNK_SIZE - 1 && k != 0 && k != CHUNK_SIZE - 1) { // 8
                        if (pright != NULL) {
                            if (!testdraw(pright->get_block(0, j, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                            }
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (pback != NULL) {
                            if (!testdraw(pback->get_block(i, 0, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // gauche
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    else if (k == 0 && j == 0 && i != 0 && i != CHUNK_SIZE - 1) { // 9
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (pbot != NULL) {
                            if (!testdraw(pbot->get_block(i, j, CHUNK_SIZE - 1))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1] )) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (pface != NULL) {
                            if (!testdraw(pface->get_block(i, CHUNK_SIZE - 1, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; // gauche
                            }
                        }
                    }
                    else if (k == 0 && j == CHUNK_SIZE - 1 && i != 0 && i != CHUNK_SIZE - 1) { // 10
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (pbot != NULL) {
                            if (!testdraw(pbot->get_block(i, j, CHUNK_SIZE - 1))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                            }
                        }
                        if (pback != NULL) {
                            if (!testdraw(pback->get_block(i, 0, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    else if (j == 0 && k == CHUNK_SIZE - 1 && i != 0 && i != CHUNK_SIZE - 1) { // 11
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (ptop != NULL) {
                            if (!testdraw(ptop->get_block(i, j, 0))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // bas
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (pface != NULL) {
                            if (!testdraw(pface->get_block(i, CHUNK_SIZE - 1, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; // gauche
                            }
                        }
                    }
                    else if (j == CHUNK_SIZE - 1 && k == CHUNK_SIZE - 1 && i != 0 && i != CHUNK_SIZE - 1) { // 12
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (ptop != NULL) {
                            if (!testdraw(ptop->get_block(i, j, 0))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // bas
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (pback != NULL) {
                            if (!testdraw(pback->get_block(i, 0, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // gauche
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    // On test chaque face
                    if (i == 0 && j != 0 && j != CHUNK_SIZE - 1 && k != 0 && k != CHUNK_SIZE - 1) {  //1 face i = 0
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (pleft != NULL) {
                            if (!testdraw(pleft->get_block(CHUNK_SIZE - 1, j, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    else if (i == CHUNK_SIZE - 1 && j != 0 && j != CHUNK_SIZE - 1 && k != 0 && k != CHUNK_SIZE - 1) { //2 face i = nbw-1
                        if (pright != NULL) {
                            if (!testdraw(pright->get_block(0, j, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                            }
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    else if (j == 0 && i != 0 && i != CHUNK_SIZE - 1 && k != 0 && k != CHUNK_SIZE - 1) { //3 face j = 0
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (pface != NULL) {
                            if (!testdraw(pface->get_block(i, CHUNK_SIZE - 1, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; // gauche
                            }
                        }
                    }
                    else if (j == CHUNK_SIZE - 1 && i != 0 && i != CHUNK_SIZE - 1 && k != 0 && k != CHUNK_SIZE - 1) { //4 face j = nbh-1
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (pback != NULL) {
                            if (!testdraw(pback->get_block(i, 0, k))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    else if (k == 0 && i != 0 && i != CHUNK_SIZE - 1 && j != 0 && j != CHUNK_SIZE - 1) { //5 face k = 0
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (pbot != NULL) {
                            if (!testdraw(pbot->get_block(i, j, CHUNK_SIZE - 1))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    else if (k == CHUNK_SIZE - 1 && i != 0 && i != CHUNK_SIZE - 1 && j != 0 && j != CHUNK_SIZE - 1) { //6 face k = nbc-1
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (ptop != NULL) {
                            if (!testdraw(ptop->get_block(i, j, 0))) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                            }
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    if (i != 0 && j != 0 && k != 0 && i != CHUNK_SIZE - 1 && j != CHUNK_SIZE - 1&& k != CHUNK_SIZE - 1) { // si aucun voisin n'est un bord
                        if (!testdraw(m_pBlocks[i + 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                        }
                        if (!testdraw(m_pBlocks[i - 1][k][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                        }
                        if (!testdraw(m_pBlocks[i][k + 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                        }
                        if (!testdraw(m_pBlocks[i][k - 1][j])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                        }
                        if (!testdraw(m_pBlocks[i][k][j + 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                        }
                        if (!testdraw(m_pBlocks[i][k][j - 1])) {
                            facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                        }
                    }
                    else { // On est sur un coin de la map
                        // on test chaque coin
                        if (i == 0 && j == 0 && k == 0) {  //1
                            if (!testdraw(m_pBlocks[i + 1][k][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                            }
                            if (pleft != NULL) {
                                if (!testdraw(pleft->get_block(CHUNK_SIZE - 1, j, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                                }
                            }
                            if (!testdraw(m_pBlocks[i][k + 1][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                            }
                            if (pbot != NULL) {
                                if (!testdraw(pbot->get_block(i, j, CHUNK_SIZE - 1))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                                }
                            }
                            if (!testdraw(m_pBlocks[i][k][j + 1])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                            }
                            if (pface != NULL) {
                                if (!testdraw(pface->get_block(i, CHUNK_SIZE - 1, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; // face
                                }
                            }
                        }
                        else if (i == 0 && j == CHUNK_SIZE - 1 && k== 0) { //2
                            if (!testdraw(m_pBlocks[i + 1][k][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                            }
                            if (pleft != NULL) {
                                if (!testdraw(pleft->get_block(CHUNK_SIZE - 1, j, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                                }
                            }
                            if (!testdraw(m_pBlocks[i][k + 1][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                            }
                            if (pbot != NULL) {
                                if (!testdraw(pbot->get_block(i, j, CHUNK_SIZE - 1))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                                }
                            }
                            if (pback != NULL) {
                                if (!testdraw(pback->get_block(i, 0, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                                }
                            }
                            if (!testdraw(m_pBlocks[i][k][j - 1])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                            }
                        }
                        else if (i == 0 && j == 0 && k == CHUNK_SIZE - 1) { //3
                            if (!testdraw(m_pBlocks[i + 1][k][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                            }
                            if (pleft != NULL) {
                                if (!testdraw(pleft->get_block(CHUNK_SIZE - 1, j, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                                }
                            }
                            if (ptop != NULL) {
                                if (!testdraw(ptop->get_block(i, j, 0))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // bas
                                }
                            }
                            if (!testdraw(m_pBlocks[i][k - 1][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                            }
                            if (!testdraw(m_pBlocks[i][k][j + 1])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                            }
                            if (pface != NULL) {
                                if (!testdraw(pface->get_block(i, CHUNK_SIZE - 1, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; // face
                                }
                            }
                        }
                        else if (i == 0 && j == CHUNK_SIZE - 1 && k == CHUNK_SIZE - 1) { //4
                            if (!testdraw(m_pBlocks[i + 1][k][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                            }
                            if (pleft != NULL) {
                                if (!testdraw(pleft->get_block(CHUNK_SIZE - 1, j, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                                }
                            }
                            if (ptop != NULL) {
                                if (!testdraw(ptop->get_block(i, j, 0))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // bas
                                }
                            }
                            if (!testdraw(m_pBlocks[i][k - 1][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                            }
                            if (pback != NULL) {
                                if (!testdraw(pback->get_block(i, 0, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                                }
                            }
                            if (!testdraw(m_pBlocks[i][k][j - 1])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                            }
                        }
                        else if (i == CHUNK_SIZE - 1 && j == 0 && k== 0) { //5
                            if (pright != NULL) {
                                if (!testdraw(pright->get_block(0, j, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                                }
                            }
                            if (!testdraw(m_pBlocks[i - 1][k][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                            }
                            if (!testdraw(m_pBlocks[i][k + 1][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                            }
                            if (pbot != NULL) {
                                if (!testdraw(pbot->get_block(i, j, CHUNK_SIZE - 1))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                                }
                            }
                            if (!testdraw(m_pBlocks[i][k][j + 1])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                            }
                            if (pface != NULL) {
                                if (!testdraw(pface->get_block(i, CHUNK_SIZE - 1, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; // face
                                }
                            }
                        }
                        else if (i == CHUNK_SIZE - 1 && j == CHUNK_SIZE - 1 && k== 0) { //6
                            if (pright != NULL) {
                                if (!testdraw(pright->get_block(0, j, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                                }
                            }
                            if (!testdraw(m_pBlocks[i - 1][k][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                            }
                            if (!testdraw(m_pBlocks[i][k + 1][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                            }
                            if (pbot != NULL) {
                                if (!testdraw(pbot->get_block(i, j, CHUNK_SIZE - 1))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                                }
                            }
                            if (pback != NULL) {
                                if (!testdraw(pback->get_block(i, 0, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                                }
                            }
                            if (!testdraw(m_pBlocks[i][k][j - 1])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                            }
                        }
                        else if (i == CHUNK_SIZE - 1 && j == 0 && k == CHUNK_SIZE - 1) { //7
                            if (pright != NULL) {
                                if (!testdraw(pright->get_block(0, j, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                                }
                            }
                            if (!testdraw(m_pBlocks[i - 1][k][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                            }
                            if (ptop != NULL) {
                                if (!testdraw(ptop->get_block(i, j, 0))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                                }
                            }
                            if (!testdraw(m_pBlocks[i][k - 1][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                            }
                            if (!testdraw(m_pBlocks[i][k][j + 1])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                            }
                            if (pface != NULL) {
                                if (!testdraw(pface->get_block(i, CHUNK_SIZE - 1, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; // face
                                }
                            }
                        }
                        else if (i == CHUNK_SIZE - 1 && j == CHUNK_SIZE - 1 && k == CHUNK_SIZE - 1) { //8
                            if (pright != NULL) {
                                if (!testdraw(pright->get_block(0, j, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][2] = 0; // droite
                                }
                            }
                            if (!testdraw(m_pBlocks[i - 1][k][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][3] = 0; // gauche
                            }
                            if (ptop != NULL) {
                                if (!testdraw(ptop->get_block(i, j, 0))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][4] = 0; // haut
                                }
                            }
                            if (!testdraw(m_pBlocks[i][k - 1][j])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][5] = 0; // bas
                            }
                            if (pback != NULL) {
                                if (!testdraw(pback->get_block(i, 0, k))) {
                                    facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][1] = 0; // dos
                                }
                            }
                            if (!testdraw(m_pBlocks[i][k][j - 1])) {
                                facetodraw[i * CHUNK_SIZE * CHUNK_SIZE + j * CHUNK_SIZE + k][0] = 0; //face
                            }
                        }
                    }// fin else sur les 8 coins
                }// fin if mblock != 0
                /// On regarde le nombre de face à dessiner
                if (std::accumulate(facetodraw[i * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE + j * Chunk::CHUNK_SIZE + k].begin(),
                                    facetodraw[i * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE + j * Chunk::CHUNK_SIZE + k].end(), 0) == 0
                     && m_pBlocks[i][k][j] != 50)  {
                    /// On ajoute un 0 dans le vecteur draw de cube si les 6 faces ne doivent pas être déssiner
                    cubedraw.push_back(0);
                }
                else { /// On ajoute 1 si au moins 1 éléments est à déssiner
                    cubedraw.push_back(1);
                }
            }
        }
    }
    cubeisdrawable = cubedraw;
    drawable = draw;
}

void Chunk::setPosXYZ(uint16_t w,uint16_t h, uint16_t c,uint16_t _nbw,uint16_t _nbh,uint16_t _nbc) {
    posW = w * uint16_t(CHUNK_SIZE);
    posH = h * uint16_t(CHUNK_SIZE);
    posC = c * uint16_t(CHUNK_SIZE);
    nbw = _nbw;
    nbh = _nbh;
    nbc = _nbh;
}

void Chunk::print_neighbors() {
    if (pleft != NULL) { std::cout << "pleft :" << pleft<< std::endl;}
    else { std::cout << "pleft : NULL"<< std::endl; }

    if (pright != NULL) { std::cout << "pright :" <<pright<< std::endl;}
    else { std::cout << "pright : NULL"<< std::endl; }

    if (pbot != NULL) { std::cout << "pbot :" <<pbot<< std::endl;}
    else { std::cout << "pbot : NULL"<< std::endl; }

    if (ptop != NULL) { std::cout << "ptop :" <<ptop<< std::endl;}
    else { std::cout << "ptop : NULL"<< std::endl; }

    if (pface != NULL) { std::cout << "pface :" <<pface<< std::endl;}
    else { std::cout << "pface : NULL"<< std::endl; }

    if (pback != NULL) { std::cout << "pback :" <<pback<< std::endl;}
    else { std::cout << "pback : NULL"<< std::endl; }
}
