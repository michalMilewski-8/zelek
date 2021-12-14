#version 400 core

layout( quads, equal_spacing,cw) in;

uniform mat4 mvp;

out vec4 f_color;

vec4 drawBrezier4(float t, vec4 B0, vec4 B1, vec4 B2, vec4 B3) {
    float one_minus_t = 1.0 - t;
    if (t == 0.0f) return B0;
    if (t >= 1.0f) return B3;

    vec4 B0_ = B0 * one_minus_t + B1 * t;
    vec4 B1_ = B1 * one_minus_t + B2 * t;
    vec4 B2_ = B2 * one_minus_t + B3 * t;

    vec4 B0_d_ = B0_ * one_minus_t + B1_ * t;
    vec4 B1_d_ = B1_ * one_minus_t + B2_ * t;

    return B0_d_ * one_minus_t + B1_d_ * t;
}


void main( )
{
	f_color = vec4(12/255.0f, 176/255.0f, 26/255.0f,1);

	vec4 p00 = gl_in[ 0].gl_Position;
	vec4 p10 = gl_in[ 1].gl_Position;
	vec4 p20 = gl_in[ 2].gl_Position;
	vec4 p30 = gl_in[ 3].gl_Position;
	vec4 p01 = gl_in[ 4].gl_Position;
	vec4 p11 = gl_in[ 5].gl_Position;
	vec4 p21 = gl_in[ 6].gl_Position;
	vec4 p31 = gl_in[ 7].gl_Position;
	vec4 p02 = gl_in[ 8].gl_Position;
	vec4 p12 = gl_in[ 9].gl_Position;
	vec4 p22 = gl_in[10].gl_Position;
	vec4 p32 = gl_in[11].gl_Position;
	vec4 p03 = gl_in[12].gl_Position;
	vec4 p13 = gl_in[13].gl_Position;
	vec4 p23 = gl_in[14].gl_Position;
	vec4 p33 = gl_in[15].gl_Position;

	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec4 bu0 = drawBrezier4(v,p00,p10,p20,p30);
	vec4 bu1 = drawBrezier4(v,p01,p11,p21,p31);
	vec4 bu2 = drawBrezier4(v,p02,p12,p22,p32);
	vec4 bu3 = drawBrezier4(v,p03,p13,p23,p33);


	vec4 position = drawBrezier4(u,bu0,bu1,bu2,bu3);
	gl_Position = mvp * position;

	//gl_Position =mvp * vec4(u,p01.x,v,1);
}