#include "sugi.h"
#include "palette.h"


const char *vert_shader_src[] = {
  "\
  #version 130\n\
  void main() {\
    gl_Position = ftransform();\
    gl_TexCoord[0] = gl_MultiTexCoord0;\
  }\
  "
};


const char *frag_shader_src[] = {
  "\
  #version 130\n\
  uniform sampler2D text_in;\
  void main() {\
    vec4 c  = texture2D(text_in, gl_TexCoord[0].st);\ 
    float r = int(c.a * 2550) + 1;\ 
    \
    if (r >= 0)   {c = vec4("SUGI_COLOR00");}\
    if (r >= 10)  {c = vec4("SUGI_COLOR01");}\
    if (r >= 20)  {c = vec4("SUGI_COLOR02");}\
    if (r >= 30)  {c = vec4("SUGI_COLOR03");}\
    if (r >= 40)  {c = vec4("SUGI_COLOR04");}\
    if (r >= 50)  {c = vec4("SUGI_COLOR05");}\
    if (r >= 60)  {c = vec4("SUGI_COLOR06");}\
    if (r >= 70)  {c = vec4("SUGI_COLOR07");}\
    if (r >= 80)  {c = vec4("SUGI_COLOR08");}\
    if (r >= 90)  {c = vec4("SUGI_COLOR09");}\
    if (r >= 100) {c = vec4("SUGI_COLOR10");}\
    if (r >= 110) {c = vec4("SUGI_COLOR11");}\
    if (r >= 120) {c = vec4("SUGI_COLOR12");}\
    if (r >= 130) {c = vec4("SUGI_COLOR13");}\
    if (r >= 140) {c = vec4("SUGI_COLOR14");}\
    if (r >= 150) {c = vec4("SUGI_COLOR15");}\
    \
    gl_FragColor = c;\
  }"
};


