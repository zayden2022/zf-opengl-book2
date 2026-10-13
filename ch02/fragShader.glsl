#version 430
out vec4 color;
void main(void)
{
    // gl_FragCoord is an input variable that contains the window relative coordinate
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/gl_FragCoord.xhtml
    if (gl_FragCoord.x < 295)
        color = vec4(1.0, 0.0, 0.0, 1.0);
    else
        color = vec4(0.0, 0.0, 1.0, 1.0);
}
