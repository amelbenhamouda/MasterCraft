#version 330 core

// Attributs de sommet
layout(location = 0) in vec3 aVertexPosition; // Position du sommet
layout(location = 1) in vec3 aVertexNormal; // Normale du sommet
layout(location = 2) in vec2 aVertexTexCoords; // Coordonnées de texture du sommet

uniform vec3 uTranslate;
uniform mat4 uVMatrix;
uniform mat4 uVmatrix;
uniform mat4 uPMatrix;
uniform bool uAnimated;
uniform float uPoswater;
uniform int uTime;


mat4 translate(vec3 txyz) {
    return mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(txyz[0], txyz[1], txyz[2], 1));
}

// Sorties du shader
out vec3 vPosition_vs; // Position du sommet transformé dans l'espace View
out vec3 vNormal_vs; // Normale du sommet transformé dans l'espace View
out vec2 vTexCoords; // Coordonnées de texture du sommet
out float vOpacity;
out vec4 viewSpace;

void main() {
    // Passage en coordonnées homogènes
    vec4 vertexPosition = vec4(aVertexPosition, 1); //1 car il s'agit des points
    vec4 vertexNormal = vec4(aVertexNormal, 0);     // 0 car il s'agit d'une normal
    float poswater = uPoswater;

    if (uAnimated) {
        vOpacity = 1.0;
        vec2 VertexTexCoords_move = aVertexTexCoords;
        VertexTexCoords_move[1] += poswater;
        vTexCoords = VertexTexCoords_move;
    }
    else {
        vOpacity = 0.0;
        vTexCoords = aVertexTexCoords;
    }

    mat4 MVMatrix = uVMatrix;
    mat4 NormalMatrix = transpose(inverse(MVMatrix));
    mat4 MVPMatrix = uPMatrix * MVMatrix;
    vPosition_vs = vec3(MVMatrix * vertexPosition);  // le vecteur toujours à droite de la matrice
    vNormal_vs = vec3(NormalMatrix * vertexNormal);
    viewSpace = uVmatrix * MVMatrix * vec4(aVertexPosition, 1);

    // Calcul de la position projetée
    gl_Position = MVPMatrix * vertexPosition; // MVP model view projection
}
