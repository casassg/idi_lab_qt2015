#version 330 core
in vec3 fcolor;
out vec4 FragColor;
void main () {

    FragColor = vec4(0.,1.,0.,1.);
    if (gl_FragCoord.x<400. && gl_FragCoord.y <400)
        FragColor = vec4(1.,1.,0.,1.);
    if(gl_FragCoord.x<400 && gl_FragCoord.y>=400)
        FragColor = vec4(1.,0.,0.,1.);
    if(gl_FragCoord.x>=400 && gl_FragCoord.y>=400)
        FragColor = vec4(0.,0.,1.,1.);

    FragColor = vec4(fcolor,1.);


}

