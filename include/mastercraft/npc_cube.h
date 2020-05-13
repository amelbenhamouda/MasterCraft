#ifndef NPC_CUBE_H
#define NPC_CUBE_H
#include "glimac/common.hpp"
#include "mastercraft/Texture_block.h"
#include <vector>

using namespace glimac;
class npc_cube {
    public:
        npc_cube();
        npc_cube(const ShapeVertex *vertices, int countvertices, std::vector<int> &l_heightmap, int width_ground, int height_ground);
        void setVboVao_npc();
        void setPosXYZ(uint8_t x, uint8_t y);
        void draw_npc(const mastercraft::NpcText &npctex, const glm::mat4 &VMatrix, const glm::mat4 &ProjMatrix, const float size_cube, const GLuint &texture, 
                      const bool nuit, glm::vec4 &nigthandday, const bool torch, const glm::vec3 poscam);
        virtual ~npc_cube();

    private:
        GLfloat posW = 0;
        GLfloat posH = 0;
        GLfloat posC = 0;
        int minwater = 0;
        int posWmax;
        int posHmax;
        float timer_npc = 0;
        float tnpc;
        float traveltime = 0.1;
        float angle_npc;
        std::vector<std::pair<int, int>> pos_npc;
        int nbpos = 100;
        int pos_to_go = 1;
        GLuint vao,vbo;
        ShapeVertex *vertices_npc = NULL;
        std::vector<int> heightmap;
        int nbvertices;
};

#endif // NPC_CUBE_H
