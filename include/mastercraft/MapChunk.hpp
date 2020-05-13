#ifndef MAPCHUNK_H
#define MAPCHUNK_H
#include "Chunk.hpp"
#include <iostream>
#include <vector>

/*
* Class pour créer un cube avec 6 vertex par face,3x2 triangle
* Construit dans l'ordre face - dos - droite -gauche - haut - bas
*
*/
class MapChunk : public Chunk {
    public:
        /// Constructeur
        MapChunk(int width, int heigth, int column);
       
        /// Destructeur
        ~MapChunk();
       
        /// Nombre de chunk
        const int nb_chunk;
       
        /// Geter pour obtenir la map
        std::vector<Chunk*>  getMap() { return map_chunk; }
        
        /// Genère la carte à partir d'une carte de hauteur et de la color map associés
        void compute(std::vector<int> &heightmap, std::vector<int> &colormap, std::vector<glm::vec3> &Tree_pos);
       
        /// Génération spécifique pour les arbres
        void compute_tree(std::vector<int> &heightmap, std::vector<int> &colormap, std::vector<glm::vec3> &Tree_pos);
        
        /// Génération spécifique pour l'eau
        void compute_water(std::vector<int> &heightmap);
        
        /// Affichage du nombre de chunk et des hauteur/largeur/longueur
        friend std::ostream &operator<<(std::ostream &os, const MapChunk &_mc);
        
        /// Méthode de display utiliser pour la surchage de l'opérateur <<
        void  displaychunk(uint8_t x, uint8_t y, uint8_t h);
        
        /// retourne le nombre de ligne en x
        int get_nbw() const{ return nbw; }
       
        /// retourne le nombre de ligne en z
        int get_nbh() const{ return nbh; }
        
        /// retourne le nombre de ligne en y
        int get_nbc() const{ return nbc; }
        
        /// Geter pour obtenir le chunk à la position souhaité
        Chunk* get_map_chunk (uint8_t x, uint8_t y, uint8_t h) const { return map_chunk[h * nbw * nbh + y * nbh + x]; }
        
        /// get minwater
        int get_minwater() const { return minwater;}

    private:
        // The blocks data
        std::vector<Chunk*> map_chunk; /// La map
        int nbw; /// nombre de ligne en x
        int nbh; /// nombre de ligne en z
        int nbc; /// nombre de ligne en y
        int minwater; /// niveau pour l'eau
};
#endif // MAPCHUNK_H
