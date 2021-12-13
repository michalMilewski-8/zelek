#version 330 core

in vec3 wpos;

out vec4 FragColor;

void main()
{
        vec2 coord = wpos.xz;
        vec2 derivative = fwidth(coord);
        vec2 grid = abs(fract(coord - 0.5f) - 0.5f) / derivative;
        float line = min(grid.x, grid.y);
        vec4 color = vec4(0.4f, 0.4f, 0.4f, 1.0f - min(line, 1.0f));
        float minz = min(derivative.y, 1.0f);
        float minx = min(derivative.x, 1.0f);
        if(wpos.x > -minx && wpos.x <minx)
            color.z = 1.0f;
        if(wpos.z > -minz && wpos.z <minz)
            color.x = 1.0f;
        FragColor = color;
}