 #version 330 core
 layout (location = 0) in vec3 aPos;
 layout (location = 1) in vec3 aNor;
 
 out vec3 ourNorm;
 out vec3 FragPos;
 
 uniform mat4 model;
 uniform mat4 trmodel;
 uniform mat4 vp;
 uniform mat4 mvp;
 
 uniform float x_size;
 uniform float y_size;
 uniform float z_size;
 uniform float x_delta;
 uniform float y_delta;
 
 
 void main()
 {

     vec3 pos = aPos;

     vec3 Normal = aNor;

     ourNorm = mat3(trmodel) * Normal;
     FragPos = vec3(model * vec4(pos, 1.0));;

     gl_Position =  vp * vec4(FragPos, 1.0);
 }
