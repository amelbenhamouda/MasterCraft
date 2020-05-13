#version 300 es
precision mediump float;

// Le Fragment Shader est destiné à traiter des fragments (ie. des pixels).
// Il fera son traitement à partir des valeurs aux sommets interpolées par
// la rasterizer lors de la production des fragments. Il fournit en sortie une
// couleur pour le pixel associé au fragment (OpenGL gère l'association fragment-pixel,
// il n'y a donc pas à spécifier les coordonnées du pixel).
in vec3 vPosition_vs;

in vec3 vNormal_vs;
in vec2 vTexCoords;
in float vOpacity;
in vec4 viewSpace;
//indique à OpenGL une valeur d'entrée du fragment shader
//elle à le même nom que la valeur de sortie du VS. C'est normal, et même obligatoire
//Les valeurs que récuperera notre fragment shaders ne seront pas exactement
//les mêmes que celles sortant du VS: il ne faut pas oublier que ces dernières sont
//interpolées sur toute la surface du triangle afin d'être affecté à chaque fragment.
out vec4 fFragColor; // couleur qui sera affectée à chaque fragment
//déclare la variable de sortie du FS destinée à contenir la couleur finale du pixel correspondant au fragment.
uniform sampler2D uTexture;

// matériaux de l'objet
uniform vec3 uKd;
uniform vec3 uKd2;
uniform vec3 uKs;
uniform vec3 uKs2;
uniform vec3 uColor;
uniform float uShininess;

//les informations sur la lumière au shader.
uniform vec3 uLightDir_vs; // _vs car en travail sur la view sphere
uniform vec3 uLightIntensity;
uniform vec3 uLightPos_vs; // position pour la lumiere ponctuelle
uniform vec3 uFrontvec;
uniform float uTorchEnable;
uniform float uShininess_lamp;
uniform vec3 uLightIntensity_lamp;

vec3 blinnPhong() {
	vec3 wo = normalize(-vPosition_vs); // half vector car la position caméra est l'origine du repère
	vec3 wi = normalize(uLightDir_vs);
	vec3 halfvector = normalize((wo + wi) / 2.0);
	float widotnormal = dot(wi,normalize(vNormal_vs));
	if (widotnormal < 0.0) {
		widotnormal = 0.0;
	}
	float halfdotnormal = dot(halfvector, normalize(vNormal_vs));
	if (halfdotnormal < 0.0) {
	 	halfdotnormal = 0.0;
	}
	return (uLightIntensity *(uKd * widotnormal + uKs * pow(halfdotnormal, uShininess)));
}

vec3 blinnPhong2() { // lumiere directionnelle  pour la "torche" du joueur
	vec3 wo = normalize(-vPosition_vs); // half vector car la position caméra est l'origine du repère
	vec3 wi = normalize(vec3(0, 0, 1));
	vec3 halfvector = normalize((wo + wi) / 2.0);
	float widotnormal = dot(wi,normalize(vNormal_vs));
	if (widotnormal < 0.0) {
		widotnormal = 0.0;
	}
	float halfdotnormal = dot(halfvector,normalize(vNormal_vs));
	if (halfdotnormal < 0.0) {
		halfdotnormal = 0.0; 
	}
	float d = distance(vec3(0, 0, 0), vPosition_vs) * 0.05;
	return (uLightIntensity_lamp / (d * d) * (uKd2 * widotnormal + uKs * pow(halfdotnormal, uShininess_lamp)));
}

vec3 blinnPhong3() { // lumiere torche
	vec3 wo = normalize(-vPosition_vs); // half vector car la position caméra est l'origine du repère
	vec3 wi = normalize(normalize(uLightPos_vs) - vPosition_vs);
	vec3 halfvector = normalize((wo + wi) / 2.0);
	float widotnormal = dot(wi, normalize(vNormal_vs));
	if (widotnormal < 0.0) {
		widotnormal = 0.0;
	}
	float halfdotnormal = dot(halfvector, normalize(vNormal_vs));
	if (halfdotnormal < 0.0) {
		halfdotnormal = 0.0;
	}
	float d = distance(normalize(uLightPos_vs), vPosition_vs) * 0.05;
	vec3 light = (uLightIntensity_lamp / (d * d) * (uKd * widotnormal + uKs * pow(halfdotnormal, uShininess_lamp)));
	light = vec3(min(light.x, 1.f), min(light.y, 1.f), min(light.z, 1.f));
	return light;
}

void main() {
    vec4 text3D = texture(uTexture, vTexCoords); // la texture/ les coordonnées
    if (uTorchEnable > 0.5) {
        vec3 lightDir = normalize(normalize(uLightPos_vs) - vPosition_vs);
        float theta = dot(lightDir, vec3(-0.2,0.2,1.0));
        if (theta > cos(radians(12.5f))) { // remember that we're working with angles as cosines instead of degrees so a '>' is used.
            fFragColor = vec4(vec3(text3D) + blinnPhong3() + blinnPhong(), 1.0);
        }
        else {
            fFragColor = vec4(vec3(text3D) + blinnPhong(), 1.0);
        }
    }
    else {
        fFragColor = vec4(vec3(text3D) + blinnPhong(), 1);
    }

    fFragColor = fFragColor * vec4(uColor, 0);
    if (vOpacity > 0.5) {
        fFragColor.a = 0.9;
    }
}
