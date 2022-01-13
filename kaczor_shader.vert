#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

uniform mat4 mvp;
uniform mat4 invViewMatrix;
uniform vec3 bezier_pts [64];

out vec3 worldPos;
out vec3 normal;
out vec3 view;
out vec2 tex;
out vec3 acolor;

vec3 drawBrezier3(float t, vec3 B0, vec3 B1, vec3 B2, vec3 B3) {
    float one_minus_t = 1.0 - t;
    if (t == 0.0f) return B0;
    if (t >= 1.0f) return B3;

    vec3 B0_ = B0 * one_minus_t + B1 * t;
    vec3 B1_ = B1 * one_minus_t + B2 * t;
    vec3 B2_ = B2 * one_minus_t + B3 * t;

    vec3 B0_d_ = B0_ * one_minus_t + B1_ * t;
    vec3 B1_d_ = B1_ * one_minus_t + B2_ * t;

    return B0_d_ * one_minus_t + B1_d_ * t;
}

void main()
{
    

    vec3 p000 = bezier_pts[ 0];
	vec3 p010 = bezier_pts[ 1];
	vec3 p020 = bezier_pts[ 2];
	vec3 p030 = bezier_pts[ 3];
	vec3 p001 = bezier_pts[ 4];
	vec3 p011 = bezier_pts[ 5];
	vec3 p021 = bezier_pts[ 6];
	vec3 p031 = bezier_pts[ 7];
	vec3 p002 = bezier_pts[ 8];
	vec3 p012 = bezier_pts[ 9];
	vec3 p022 = bezier_pts[10];
	vec3 p032 = bezier_pts[11];
	vec3 p003 = bezier_pts[12];
	vec3 p013 = bezier_pts[13];
	vec3 p023 = bezier_pts[14];
	vec3 p033 = bezier_pts[15];
	vec3 p100 = bezier_pts[16 + 0];
	vec3 p110 = bezier_pts[16 + 1];
	vec3 p120 = bezier_pts[16 + 2];
	vec3 p130 = bezier_pts[16 + 3];
	vec3 p101 = bezier_pts[16 + 4];
	vec3 p111 = bezier_pts[16 + 5];
	vec3 p121 = bezier_pts[16 + 6];
	vec3 p131 = bezier_pts[16 + 7];
	vec3 p102 = bezier_pts[16 + 8];
	vec3 p112 = bezier_pts[16 + 9];
	vec3 p122 = bezier_pts[16 +10];
	vec3 p132 = bezier_pts[16 +11];
	vec3 p103 = bezier_pts[16 +12];
	vec3 p113 = bezier_pts[16 +13];
	vec3 p123 = bezier_pts[16 +14];
	vec3 p133 = bezier_pts[16 +15];
	vec3 p200 = bezier_pts[32 + 0];
	vec3 p210 = bezier_pts[32 + 1];
	vec3 p220 = bezier_pts[32 + 2];
	vec3 p230 = bezier_pts[32 + 3];
	vec3 p201 = bezier_pts[32 + 4];
	vec3 p211 = bezier_pts[32 + 5];
	vec3 p221 = bezier_pts[32 + 6];
	vec3 p231 = bezier_pts[32 + 7];
	vec3 p202 = bezier_pts[32 + 8];
	vec3 p212 = bezier_pts[32 + 9];
	vec3 p222 = bezier_pts[32 +10];
	vec3 p232 = bezier_pts[32 +11];
	vec3 p203 = bezier_pts[32 +12];
	vec3 p213 = bezier_pts[32 +13];
	vec3 p223 = bezier_pts[32 +14];
	vec3 p233 = bezier_pts[32 +15];
	vec3 p300 = bezier_pts[48 + 0];
	vec3 p310 = bezier_pts[48 + 1];
	vec3 p320 = bezier_pts[48 + 2];
	vec3 p330 = bezier_pts[48 + 3];
	vec3 p301 = bezier_pts[48 + 4];
	vec3 p311 = bezier_pts[48 + 5];
	vec3 p321 = bezier_pts[48 + 6];
	vec3 p331 = bezier_pts[48 + 7];
	vec3 p302 = bezier_pts[48 + 8];
	vec3 p312 = bezier_pts[48 + 9];
	vec3 p322 = bezier_pts[48 +10];
	vec3 p332 = bezier_pts[48 +11];
	vec3 p303 = bezier_pts[48 +12];
	vec3 p313 = bezier_pts[48 +13];
	vec3 p323 = bezier_pts[48 +14];
	vec3 p333 = bezier_pts[48 +15];

	vec3 bu00 = drawBrezier3(aPos.z,p000,p010,p020,p030);
	vec3 bu01 = drawBrezier3(aPos.z,p001,p011,p021,p031);
	vec3 bu02 = drawBrezier3(aPos.z,p002,p012,p022,p032);
	vec3 bu03 = drawBrezier3(aPos.z,p003,p013,p023,p033);
	vec3 bu10 = drawBrezier3(aPos.z,p100,p110,p120,p130);
	vec3 bu11 = drawBrezier3(aPos.z,p101,p111,p121,p131);
	vec3 bu12 = drawBrezier3(aPos.z,p102,p112,p122,p132);
	vec3 bu13 = drawBrezier3(aPos.z,p103,p113,p123,p133);
	vec3 bu20 = drawBrezier3(aPos.z,p200,p210,p220,p230);
	vec3 bu21 = drawBrezier3(aPos.z,p201,p211,p221,p231);
	vec3 bu22 = drawBrezier3(aPos.z,p202,p212,p222,p232);
	vec3 bu23 = drawBrezier3(aPos.z,p203,p213,p223,p233);
	vec3 bu30 = drawBrezier3(aPos.z,p300,p310,p320,p330);
	vec3 bu31 = drawBrezier3(aPos.z,p301,p311,p321,p331);
	vec3 bu32 = drawBrezier3(aPos.z,p302,p312,p322,p332);
	vec3 bu33 = drawBrezier3(aPos.z,p303,p313,p323,p333);

	vec3 bv0 = drawBrezier3(aPos.y,bu00,bu01,bu02,bu03);
	vec3 bv1 = drawBrezier3(aPos.y,bu10,bu11,bu12,bu13);
	vec3 bv2 = drawBrezier3(aPos.y,bu20,bu21,bu22,bu23);
	vec3 bv3 = drawBrezier3(aPos.y,bu30,bu31,bu32,bu33);

	vec3 pos = drawBrezier3(aPos.x,bv0,bv1,bv2,bv3);
	worldPos = pos;

	vec3 small = (aPos / 2.0f + vec3(0.25f,0.25f,0.25f));

	bu00 = drawBrezier3(small.z,p000,p010,p020,p030);
	bu01 = drawBrezier3(small.z,p001,p011,p021,p031);
	bu02 = drawBrezier3(small.z,p002,p012,p022,p032);
	bu03 = drawBrezier3(small.z,p003,p013,p023,p033);
	bu10 = drawBrezier3(small.z,p100,p110,p120,p130);
	bu11 = drawBrezier3(small.z,p101,p111,p121,p131);
	bu12 = drawBrezier3(small.z,p102,p112,p122,p132);
	bu13 = drawBrezier3(small.z,p103,p113,p123,p133);
	bu20 = drawBrezier3(small.z,p200,p210,p220,p230);
	bu21 = drawBrezier3(small.z,p201,p211,p221,p231);
	bu22 = drawBrezier3(small.z,p202,p212,p222,p232);
	bu23 = drawBrezier3(small.z,p203,p213,p223,p233);
	bu30 = drawBrezier3(small.z,p300,p310,p320,p330);
	bu31 = drawBrezier3(small.z,p301,p311,p321,p331);
	bu32 = drawBrezier3(small.z,p302,p312,p322,p332);
	bu33 = drawBrezier3(small.z,p303,p313,p323,p333);
	 bv0 = drawBrezier3(small.y,bu00,bu01,bu02,bu03);
	 bv1 = drawBrezier3(small.y,bu10,bu11,bu12,bu13);
	 bv2 = drawBrezier3(small.y,bu20,bu21,bu22,bu23);
	 bv3 = drawBrezier3(small.y,bu30,bu31,bu32,bu33);

	vec3 smallpos = drawBrezier3(small.x,bv0,bv1,bv2,bv3);

    normal = normalize(pos - smallpos);
    tex = aTex;

    vec3 camPos = (invViewMatrix* vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
	view = camPos - worldPos;

    gl_Position = mvp * vec4(pos, 1.0);

}