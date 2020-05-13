#include "mastercraft/MapChunk.hpp"
#include <cmath>
#include <algorithm>

MapChunk::MapChunk(int width, int heigth, int column) : nb_chunk(width * heigth * column / std::pow(CHUNK_SIZE, 3)),
    nbw(width / CHUNK_SIZE), nbh(heigth / CHUNK_SIZE), nbc(column / CHUNK_SIZE) {
    Chunk *pdefault_chunk;
    int countchunk = 0;
    /// On construit la map avec des chunk par défault remplis de 0
    /// Pour chaque chunk on utilise setPosXYZ pour indiquer à l'objet chunk sa position dans la map
    for(int k = 0; k < nbc; k++) { // col
        for(int j = 0; j < nbh; j++) { // largeur
            for(int i = 0; i < nbw; i++) { // longueur
                pdefault_chunk = new Chunk();
                map_chunk.push_back(pdefault_chunk);
                map_chunk[k * nbw * nbh + j * nbh + i]->setPosXYZ(i, j, k, nbw, nbh, nbc);
                countchunk++;
            }
        }
    }
    //std::cout << "countchunk:" << countchunk << std::endl;
}

MapChunk::~MapChunk() {
    /// On delete la mémoire allouer pour chaque chunk dans la map
    for(auto &it: map_chunk) {
        delete it;
    }
}

std::ostream &operator<<(std::ostream &os, const MapChunk &_mc) {
    os << "Map with " <<_mc.nb_chunk << " chunk, W: " << _mc.nbw << ", H: " << _mc.nbh << ", Nb Columns: " << _mc.nbc;
}

void  MapChunk::displaychunk(uint8_t x, uint8_t y, uint8_t h) {
    assert(x < nbw && y < nbh && h < nbc);
    std::cout << *map_chunk[h * nbw * nbh + y * nbh + x];
}

void MapChunk::compute_tree(std::vector<int> &heightmap, std::vector<int> &colormap,std::vector<glm::vec3> &Tree_pos) {
    /// On impose que les deux cartes soit de la même taille
    assert (heightmap.size() == colormap.size());
    int line = 0;
    int nbblockarbre = 0;
    auto it = std::min_element(heightmap.begin(),heightmap.end());
    float minmap = *it;
    auto it2 = std::max_element(heightmap.begin(),heightmap.end());
    float maxmap = *it2;
    float factor = 0.5;
    /// La map étant un vecteur il faut calculer chaque emplacement ligne/collone/hauteur pour les placés correctement dans le vecteur
    minwater = int(minmap +(maxmap*factor));

    /// La map étant un vecteur il faut calculer chaque emplacement ligne/collone/hauteur pour les placés correctement dans le vecteur
    for (int i = 0; i < heightmap.size(); i++) {
        int col = i % (nbh * Chunk::CHUNK_SIZE);
        if (i != 0 && i % (nbw * Chunk::CHUNK_SIZE) == 0) {
            line++;
        }

        if (colormap[i] == 100 && (heightmap[i] + 5)< nbc * Chunk::CHUNK_SIZE ) {
            if (heightmap[i] >= minwater) { // on regarde si l'arbre est sous le niveau de l'eau au quel cas il n'est pas déssiner
                for (int j = 0 ; j < Tree_pos.size() ; j++) {
                    if (line > 3 && line < nbw * Chunk::CHUNK_SIZE - 3 && col > 3 && col < nbh * Chunk::CHUNK_SIZE - 3) {
                        int x_chunk = (line + (int)Tree_pos[j][0]) % Chunk::CHUNK_SIZE;
                        int y_chunk = (col+ (int)Tree_pos[j][2]) % Chunk::CHUNK_SIZE;
                        int k_map_0_tree =  heightmap[i] / Chunk::CHUNK_SIZE; // botom of the tree
                        int k_map = (heightmap[i]+ (int)Tree_pos[j][1]) / Chunk::CHUNK_SIZE; // implicite floor (heightmap / chunk_size)
                        int x_map = (line + (int)Tree_pos[j][0]) / Chunk::CHUNK_SIZE;          // pos x chunk in map
                        int y_map = (col + (int)Tree_pos[j][2]) / Chunk::CHUNK_SIZE;         // pos y chunk in map
                        int k_chunk = (heightmap[i] +  (int)Tree_pos[j][1])  % Chunk::CHUNK_SIZE;
                        if (j < 2) {
                            map_chunk[k_map * nbw * nbh + y_map * nbh + x_map]->create_block(x_chunk, y_chunk, k_chunk, colormap[i]);
                        } 
                        else {
                            map_chunk[k_map * nbw * nbh + y_map * nbh + x_map]->create_block(x_chunk, y_chunk, k_chunk, colormap[i] + 1);
                        }
                        nbblockarbre++;
                    }
                }
            } 
            else {
                int x_chunk = line % Chunk::CHUNK_SIZE;
                int y_chunk = col % Chunk::CHUNK_SIZE;
                int k_map = (heightmap[i] - 1) / Chunk::CHUNK_SIZE; // implicite floor (heightmap / chunk_size)
                int x_map = line / Chunk::CHUNK_SIZE;          // pos x chunk in map
                int y_map = col / Chunk::CHUNK_SIZE;         // pos y chunk in map
                int k_chunk = (heightmap[i] - 1) % Chunk::CHUNK_SIZE;
                map_chunk[k_map * nbw * nbh + y_map * nbh + x_map]->create_block(x_chunk, y_chunk, k_chunk, 1);
            }
        }
    }
}

void MapChunk::compute_water(std::vector<int> &heightmap) {
    int line = 0;
    int nbblockwater = 0;
    auto it = std::min_element(heightmap.begin(), heightmap.end());
    float minmap = *it;
    auto it2 = std::max_element(heightmap.begin(), heightmap.end());
    float maxmap = *it2;
    float factor = 0.5;
    /// La map étant un vecteur il faut calculer chaque emplacement ligne/collone/hauteur pour les placés correctement dans le vecteur
    for (int i = 0; i < heightmap.size(); i++) {
        int col = i % (nbw * Chunk::CHUNK_SIZE); // nbw== nb chunk so nbw* chunk_size == nb col
        if (i != 0 && i % (nbw * Chunk::CHUNK_SIZE) == 0) {
            line++;
        }

        if (heightmap[i] <= int(minmap + (maxmap * factor))) {
            int x_chunk = line % Chunk::CHUNK_SIZE;
            int y_chunk = col % Chunk::CHUNK_SIZE;
            int k_map = int(minmap + (maxmap * factor)) / Chunk::CHUNK_SIZE; // implicite floor (heightmap / chunk_size)
            int x_map = line / Chunk::CHUNK_SIZE;          // pos x chunk in map
            int y_map = col / Chunk::CHUNK_SIZE;         // pos y chunk in map
            int k_chunk = int(minmap + (maxmap * factor)) % Chunk::CHUNK_SIZE;
            if (map_chunk[k_map * nbw * nbh + y_map * nbh + x_map]->get_block(x_chunk, y_chunk, k_chunk) == 0) {
                map_chunk[k_map * nbw * nbh + y_map * nbh + x_map]->create_block(x_chunk, y_chunk, k_chunk, 50);
            }
            nbblockwater++;
            if (col == 0 || col == nbw * Chunk::CHUNK_SIZE - 1 || line == 0 || line == nbh *  Chunk::CHUNK_SIZE - 1) {
                int nbblock = int(minmap + (maxmap * factor)) - heightmap[i];
                for (int j = 0; j <= nbblock ; j++) {
                    int k_map = int(minmap + (maxmap * factor) - j) / Chunk::CHUNK_SIZE;
                    int k_chunk = int(minmap + (maxmap * factor) - j) % Chunk::CHUNK_SIZE;
                    map_chunk[k_map * nbw * nbh + y_map * nbh + x_map]->create_block(x_chunk, y_chunk, k_chunk, 51);
                }
            }
        }
    }
}

void MapChunk::compute(std::vector<int> &heightmap, std::vector<int> &colormap, std::vector<glm::vec3> &Tree_pos) {
    /// On impose que les deux cartes soit de la même taille
    assert (heightmap.size() == colormap.size());
    int line = 0;
    int nbblock = 0;
    /// La map étant un vecteur il faut calculer chaque emplacement ligne/collone/hauteur pour les placés correctement dans le vecteur
    for (int i = 0; i < heightmap.size(); i++) {
        int col = i % (nbw * Chunk::CHUNK_SIZE); // nbw== nb chunk so nbw* chunk_size == nb col
        if (i != 0 && i % (nbw * Chunk::CHUNK_SIZE) == 0) {
            line++;
        }
        int x_chunk = line % Chunk::CHUNK_SIZE;
        int y_chunk = col % Chunk::CHUNK_SIZE;
        int k_map = heightmap[i] / Chunk::CHUNK_SIZE; // implicite floor (heightmap / chunk_size)
        int x_map = line / Chunk::CHUNK_SIZE;          // pos x chunk in map
        int y_map = col / Chunk::CHUNK_SIZE;         // pos y chunk in map
        int k_chunk;
        /// Une fois qu'on à la position en x et z on regarde la hauteur à laquel on doit placer les éléments dans le chunk.
        for (int h_map = 0; h_map <= k_map; h_map++) {
            if (h_map != k_map) {
                /// On vérifie que se situe au dessous de la position la plus haute de la carte et si c'est le cas on remplis le chunk en collone
                /// en mettant le nombre de collone k_chunk à la taille max d'un chunk
                k_chunk = Chunk::CHUNK_SIZE;
            } 
            else {
                k_chunk = heightmap[i] % Chunk::CHUNK_SIZE;
            }
            for (int h_chunk = 0; h_chunk < k_chunk; h_chunk++) {
                if (colormap[i] == 100 && h_chunk != k_chunk - 1) {
                    map_chunk[h_map * nbw * nbh + y_map * nbh + x_map]->create_block(x_chunk, y_chunk, h_chunk, 2);
                } 
                else {
                    map_chunk[h_map * nbw * nbh + y_map * nbh + x_map]->create_block(x_chunk, y_chunk, h_chunk, colormap[i]);
                }
                nbblock++;
            }
        }
    }
    /// Pour chaque chunk on doit tester toutes les arête/face/coin/intérieur et attribuer leur voisins
    for (int k = 0; k < nbc; k++) {
        for (int j = 0; j < nbh; j++) {
            for (int i = 0; i < nbw; i++) { /// On test chaque arête
                if (i == 0 && k == 0 && j != 0 && j != nbh - 1) { // 1
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (i == 0 && j == 0 && k != 0 && k != nbc - 1) { // 2
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (i == 0 && k == nbc - 1 && j != 0 && j != nbh - 1) { // 3
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (i == 0 && j == nbh - 1 && k != 0 && k != nbc - 1) { // 4
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (i == nbw - 1 && k == 0 && j != 0 && j != nbh - 1) { // 5
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (i == nbw - 1 && j == 0 && k != 0 && k != nbc - 1) { // 6
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (i == nbw - 1 && k == nbc - 1 && j != 0 && j != nbh - 1) { // 7
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (i == nbw - 1 && j == nbh - 1 && k != 0 && k != nbc - 1) { // 8
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (k == 0&& j == 0&& i != 0 && i != nbw - 1) { // 9
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (k == 0 && j == nbh - 1 && i != 0 && i != nbw - 1) { /// 10
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (j == 0 && k == nbc - 1 && i != 0 && i != nbw - 1) { // 11
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (j == nbh - 1 && k == nbc - 1 && i != 0 && i != nbw - 1) { // 12
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                }
                /// On test chaque face
                else if (i == 0 && j != 0 && j != nbh - 1 && k != 0 && k != nbc - 1) { /// 1 face i = 0
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (i == nbw - 1 && j != 0 && j != nbh - 1 && k != 0 && k != nbc - 1) { /// 2 face i = nbw-1
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (j == 0 && i != 0 && i != nbw - 1 && k != 0 && k != nbc - 1) { /// 3 face j = 0
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (j == nbh - 1 && i != 0 && i != nbw - 1 && k != 0 && k != nbc - 1) { /// 4 face j = nbh-1
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (k == 0 && i != 0 && i != nbw - 1 && j != 0 && j != nbh - 1) { /// 5 face k = 0
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else if (k == nbc - 1 && i != 0 && i != nbw - 1 && j != 0 && j != nbh - 1) { /// 6 face k = nbw-1
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                }
                /// si aucun voisin n'est un bord (On est dans le centre de la map)
                else if (i != 0 && j != 0 && k != 0 && i != nbw - 1 && j != nbh - 1 && k != nbc - 1) {
                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                    map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                    map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);

                    map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                    map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                } 
                else {
                    /// Enfin on est sur un coin de la map
                    /// on test chaque coin
                    if (i == 0 && j == 0 && k == 0) { /// 1
                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                        map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                        map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                        map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);
                    } 
                    else if (i == 0 && j == nbh - 1 && k == 0) { /// 2
                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                        map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                        map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                        map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);
                    } 
                    else if (i == 0 && j == 0 && k == nbc - 1) { /// 3
                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                        map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                        map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                        map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                    } 
                    else if (i == 0 && j == nbh - 1 && k == nbc - 1) { /// 4
                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                        map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pright(map_chunk[k * nbw * nbh + j * nbh + (i + 1)]);
                        map_chunk[k * nbw * nbh + j * nbh + (i + 1)]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                        map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                    } 
                    else if (i == nbw - 1 && j == 0 && k == 0) { /// 5
                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                        map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                        map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                        map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);
                    } 
                    else if (i == nbw - 1 && j == nbh - 1 && k == 0) { /// 6
                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                        map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                        map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[(k + 1) * nbw * nbh + j * nbh + i]);
                        map_chunk[(k + 1) * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[k * nbw * nbh + j * nbh + i]);
                    } 
                    else if (i == nbw - 1 && j == 0 && k == nbc - 1) { /// 7
                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pback(map_chunk[k * nbw * nbh + (j + 1) * nbh + i]);
                        map_chunk[k * nbw * nbh + (j + 1) * nbh + i]->set_pface(map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                        map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                        map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                    } 
                    else if (i == nbw - 1 && j == nbh - 1 && k == nbc - 1) { /// 8
                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pface(map_chunk[k * nbw * nbh + (j - 1) * nbh + i]);
                        map_chunk[k * nbw * nbh + (j - 1) * nbh + i]->set_pback(map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pleft( map_chunk[k * nbw * nbh + j * nbh + (i - 1)]);
                        map_chunk[k * nbw * nbh + j * nbh + (i - 1)]->set_pright(map_chunk[k * nbw * nbh + j * nbh + i]);

                        map_chunk[k * nbw * nbh + j * nbh + i]->set_pbot(map_chunk[(k - 1) * nbw * nbh + j * nbh + i]);
                        map_chunk[(k - 1) * nbw * nbh + j * nbh + i]->set_ptop(map_chunk[k * nbw * nbh + j * nbh + i]);
                    }
                }
            }
        }
    }
    compute_tree(heightmap,colormap,Tree_pos);
    compute_water(heightmap);

    /// Maintenant que chaque voisin est attribué il faut tester tous les chunks
    /// pour savoir si il doivent être afficher et quel cube doit être afficher dans chaque chunk
    for (int k = 0; k < nbc; k++) {
        for (int j = 0; j < nbh; j++) {
            for (int i = 0; i < nbw; i++) {
                map_chunk[k * nbw * nbh + j * nbh + i]->Check_chunk();
            }
        }
    }
}
