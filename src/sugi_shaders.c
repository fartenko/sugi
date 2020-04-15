#include "sugi.h"
#include "sugi_palette.h"


const char *vert_shader_src[0x1ffff] = {
	"\
	#version 130\n\
	\
	void main() {\
		gl_Position = ftransform();\
		gl_TexCoord[0] = gl_MultiTexCoord0;\
	}\
	"
};


const char *frag_shader_src[0x1ffff] = {
	"\
	#version 130\n\
	\
	uniform sampler2D text_in;\
	\
	vec4 PALETTE[16] = vec4[](\
		vec4("SUGI_COLOR00"),\
		vec4("SUGI_COLOR01"),\
		vec4("SUGI_COLOR02"),\
		vec4("SUGI_COLOR03"),\
		vec4("SUGI_COLOR04"),\
		vec4("SUGI_COLOR05"),\
		vec4("SUGI_COLOR06"),\
		vec4("SUGI_COLOR07"),\
		vec4("SUGI_COLOR08"),\
		vec4("SUGI_COLOR09"),\
		vec4("SUGI_COLOR10"),\
		vec4("SUGI_COLOR11"),\
		vec4("SUGI_COLOR12"),\
		vec4("SUGI_COLOR13"),\
		vec4("SUGI_COLOR14"),\
		vec4("SUGI_COLOR15")\
	);\
	\
	void main() {\
		vec4 c = texture2D(text_in, gl_TexCoord[0].st);\
		int a = int((c.a * 255.0) + 0.5);\
		c = PALETTE[a];\
		gl_FragColor = c;\
	}\
	"
};
