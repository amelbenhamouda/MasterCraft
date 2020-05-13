#version 300 es
precision mediump float;

// Attributs de sommet
layout(location = 0) in vec3 aVertexPosition; // Position du sommet
layout(location = 1) in vec3 aVertexNormal; // Normale du sommet
layout(location = 2) in vec2 aVertexTexCoords; // Coordonnées de texture du sommet

uniform vec3 uTranslate;
uniform mat4 uRotate;
uniform mat4 uVMatrix;
uniform mat4 uPMatrix;
uniform float uSize_cube;

mat4 translate(vec3 txyz){
    return mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(txyz[0], txyz[1], txyz[2], 1));
}

// Sorties du shader
out vec3 vPosition_vs; // Position du sommet transformé dans l'espace View
out vec3 vNormal_vs; // Normale du sommet transformé dans l'espace View
out vec2 vTexCoords; // Coordonnées de texture du sommet

void main() {
    // Passage en coordonnées homogènes
    vec4 vertexPosition = vec4(aVertexPosition, 1); //1 car il s'agit des points
    vec4 vertexNormal = vec4(aVertexNormal, 0);     // 0 car il s'agit d'une normal

    // Calcul des valeurs de sortie
    vTexCoords = aVertexTexCoords;
    // les deux translate sont la pour ramener le personnage en (0,y,0)
    // pour faire la rotation par le centre du personnage
    float size_cube = uSize_cube * 0.5;
    mat4 MVMatrix = uVMatrix * translate(uTranslate) * translate(vec3(size_cube, 0, -size_cube)) * uRotate * translate(vec3(-size_cube, 0, size_cube));
    mat4 NormalMatrix = transpose(inverse(MVMatrix));
    mat4 MVPMatrix = uPMatrix * MVMatrix;
    vPosition_vs = vec3(MVMatrix * vertexPosition);  // le vecteur toujours à droite de la matrice
    vNormal_vs = vec3(NormalMatrix * vertexNormal);

    // Calcul de la position projetée
    gl_Position = MVPMatrix * vertexPosition; // MVP model view projection
}
