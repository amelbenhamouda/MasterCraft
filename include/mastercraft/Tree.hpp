#pragma once

#include <vector>

#include "../glimac/common.hpp"

namespace glimac {

    // Représente une sphère discrétisée centrée en (0, 0, 0) (dans son repère local)
    // Son axe vertical est (0, 1, 0) et ses axes transversaux sont (1, 0, 0) et (0, 0, 1)
    class Tree {
        // Alloue et construit les données (implantation dans le .cpp)
        void build(const int sizeCube);
        void tronc(std::vector<ShapeVertex> data, GLsizei count_vertex, float tol, const int sizeCube);
        void leavesFace(std::vector<ShapeVertex> data, GLsizei count_vertex, float tol, const int sizeCube);
        void leavesCote(std::vector<ShapeVertex> data, GLsizei count_vertex, float tol, const int sizeCube);

        public:
            // Constructeur: alloue le tableau de données et construit les attributs des vertex
            Tree(const int sizeCube) : m_nVertexCount(0) {
                build(sizeCube); // Construction (voir le .cpp)
            }

            // Renvoit le pointeur vers les données
            const ShapeVertex* getDataPointer() const {
                return &m_Vertices[0];
            }

            // Renvoit les positions
            const std::vector<glm::vec3> getDataPosition() const {
                return m_Position;
            }

            // Renvoit le nombre de vertex
            GLsizei getVertexCount() const {
                return m_nVertexCount;
            }

        private:
            std::vector<ShapeVertex> m_Vertices;
            std::vector<glm::vec3> m_Position;
            GLsizei m_nVertexCount; // Nombre de sommets
    };

}
