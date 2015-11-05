# version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

out vec3 matambF;
out vec3 matdiffF;
out vec3 matspecF;
out float matshinF;


out vec3 normalSCO;
out vec4 vertSCO;






void main()
{	
    vertSCO = (view * TG * vec4 (vertex, 1.0));
    mat3 normalMatrix = inverse (transpose (mat3 (view * TG)));
    normalSCO =  normalize(normalMatrix * normal);

    gl_Position = proj  * vertSCO;

    //Passem parametres cap a fragment
    matambF = matamb;
    matdiffF = matdiff;
    matspecF = matspec;
    matshinF = matshin;
}
