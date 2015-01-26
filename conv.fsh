uniform sampler2D Texture0;
uniform vec2 delta;
uniform float repet;
uniform float efac1,efac2,efac3,efac4,efac5,gamma;

void main(void){   
	vec4 acc = vec4(0);
	
	acc += pow(texture2D(Texture0, gl_TexCoord[0].st+vec2(0, -1.0)*repet*delta*0), vec4(gamma));
	acc -= pow(texture2D(Texture0, gl_TexCoord[0].st+vec2(0, -1.0)*repet*delta*1), vec4(gamma))*efac1;
	acc += pow(texture2D(Texture0, gl_TexCoord[0].st+vec2(0, -1.0)*repet*delta*2), vec4(gamma))*efac2;
	acc -= pow(texture2D(Texture0, gl_TexCoord[0].st+vec2(0, -1.0)*repet*delta*3), vec4(gamma))*efac3;
	acc += pow(texture2D(Texture0, gl_TexCoord[0].st+vec2(0, -1.0)*repet*delta*4), vec4(gamma))*efac4;
	acc -= pow(texture2D(Texture0, gl_TexCoord[0].st+vec2(0, -1.0)*repet*delta*5), vec4(gamma))*efac5;
	
	gl_FragData[0].rgba = pow(acc, vec4(1.0 / gamma));  
}
