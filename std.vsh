void main(void){
	vec2 pos = sign(gl_Vertex.xy);
	gl_Position = vec4(pos, 0.0, 1.0);
	gl_TexCoord[0] = gl_MultiTexCoord0;
}