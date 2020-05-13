#ifndef CHUNK_H
#define CHUNK_H
#include <stdint.h>
#include <iostream>
#include <assert.h>
#include <GL/glew.h>
#include <vector>
#include "glimac/common.hpp"
#include "mastercraft/Texture_block.h"

using namespace glimac;

struct BlockText;
class Chunk {
    public:
        /// Constructeur
        Chunk();
        
        /// Destructeur
        ~Chunk();
        
        /// Création d'un block sans vérification
        void create_block(uint8_t x, uint8_t y, uint8_t z, uint8_t data);
        
        /// Update d'un block avec vérification du chunk
        void update_block(uint8_t x, uint8_t y, uint8_t z, uint8_t data, const int i, const int j, const int k, glm::vec3 xyz, 
                          const std::vector<int> &l_heightmap, const GLsizei count_vertex, const GLsizei count_vertex_tree, const ShapeVertex *vertices, 
                          const float size_cube);
       
        /// Geter du block dans le chunk
        uint8_t get_block(uint8_t x, uint8_t y, uint8_t z);

        /// Taille d'un Chunk
        static const uint8_t CHUNK_SIZE = 16;
        
        /// Surcharge opérateur <<
        friend std::ostream &operator<<(std::ostream &os, const Chunk &_c);
        
        /// Vérification dans le chunk si on doit dessiner un cube et le nombre de face que l'on doit déssiner
        void Check_chunk();
        
        /// Booléen vérifiant si un chunk doit être déssiner ou non
        bool isdrawable() const { return drawable; }
        
        /// Seter de la position xyz du chunk dans une map
        void setPosXYZ(uint16_t x, uint16_t y, uint16_t z, uint16_t _nbw, uint16_t _nbh, uint16_t _nbc);
       
        /// Affiche les voisins d'un chunk
        void print_neighbors();
        
        /// Getter des position en x y et z
        uint16_t get_posW() { return posW; }
        uint16_t get_posH() { return posH; }
        uint16_t get_posC() { return posC; }
        
        /// Renvoi un vecteur avec les faces à afficher
        std::vector<std::vector<bool>> getfacetodraw() { return facetodraw; }
        
        /// Renvoi un vecteur de même taille que le vecteur des faces indiquant si un cube doit être déssiner
        std::vector<bool> getcubetodraw() { return cubeisdrawable; }
        
        ///Vbo et Vao du chunk
        void setVaoandVbo(const std::vector<ShapeVertex> vertices);
       
        ///Seter of vertices ijk position of chunk xyz nb chunk for each axis
        void setVertices(const int i, const int j, const int k, glm::vec3 xyz, const std::vector<int> &l_heightmap, const GLsizei count_vertex,
                         const GLsizei count_vertex_tree, const ShapeVertex *vertices, const float size_cube);
       
        /// render chunk
        void render_chunk(const mastercraft::BlockText &btext, glm::mat4 &VMatrix, const glm::mat4 &ProjMatrix, const int size_cube, const float poswater,
                          const glm::vec3 &currentColor);
        
        /// render water
        void render_water(const mastercraft::BlockText &btext, glm::mat4 &VMatrix, const glm::mat4 &ProjMatrix, const int size_cube, const float poswater,
                          const glm::vec3 &currentColor);
       
        /// Setter des voisin du chunk
        void set_pleft(Chunk* _chunk) { pleft = _chunk; }
        void set_pright(Chunk* _chunk) { pright = _chunk; }
        void set_pbot(Chunk* _chunk) { pbot = _chunk; }
        void set_ptop(Chunk* _chunk) { ptop = _chunk; }
        void set_pface(Chunk* _chunk) { pface = _chunk; }
        void set_pback(Chunk* _chunk) { pback = _chunk; };

    private:
        /// The blocks data
        /// vecteur avec les faces à afficher
        std::vector<std::vector<bool>> facetodraw;
        /// vecteur de même taille que le vecteur des faces indiquant si un cube doit être déssiner
        std::vector<bool> cubeisdrawable;
        uint8_t m_pBlocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
        uint16_t posW = 0;
        uint16_t posH = 0;
        uint16_t posC = 0;
        uint16_t nbw = 0;
        uint16_t nbh = 0;
        uint16_t nbc = 0;
        /// Voisin du chunk
        Chunk* pleft = NULL;
        Chunk* pright = NULL;
        Chunk* pbot = NULL;
        Chunk* ptop = NULL;
        Chunk* pface = NULL;
        Chunk* pback = NULL;
        ///vao et vbo
        GLuint vao,vbo;
        /// vertice of chunk
        int nbvertices;
        int nbvertices_water;
        int nbvertices_under_water;
        /// booléen déterminant si on dessine ou non un chunk
        bool drawable = false;
        int poswater = 0;
};

#endif // CHUNK_H
