#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <cmath>
#include <iostream>
#include <cstdio>
using namespace std;

char *textFileRead(const char *fn) {
	FILE *fp;
	char *content = NULL;

	int count=0;

	if(fn != NULL){
		fp = fopen(fn,"rt");
		if(fp != NULL){
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if(count > 0){
				content = new char[count+1];
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

static void printShaderInfoLog(GLuint obj){
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if(infologLength > 1){
		infoLog = new char[infologLength];
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		puts(infoLog);
		delete []infoLog;
	}
}

static void printProgramInfoLog(GLuint obj){
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if(infologLength > 1){
		infoLog = new char[infologLength];
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		puts(infoLog);
		delete []infoLog;
	}
}

GLuint loadShaders(){
	GLuint v,f;
	
	char *vs; v = glCreateShader(GL_VERTEX_SHADER);
	vs = textFileRead("std.vsh");	
	const char * vv = vs;	
	glShaderSource(v, 1, &vv,NULL);	
	delete []vs;
	glCompileShader(v);
	printShaderInfoLog(v);
	
	f = glCreateShader(GL_FRAGMENT_SHADER);
		
	char *fs = textFileRead("conv.fsh");
	const char *ff = fs;
	glShaderSource(f, 1, &ff,NULL);
	delete []fs;
	
	glCompileShader(f);
	printShaderInfoLog(f);
	GLuint program = glCreateProgram();
	glAttachShader(program,v);
	glAttachShader(program,f);
	glLinkProgram(program);
	printProgramInfoLog(program);
	
	return program;
}

int main(int argc, char **args){
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		cerr << "Error in SDL_Init." << endl;
		return -1;
	}
	
	IMG_Init(IMG_INIT_PNG);
	SDL_Surface *img = IMG_Load("input0.png");
	if(img == NULL){
		cerr << "Error loading image: " << SDL_GetError() << endl;
		return -1;
	}
	
	const int dScrWidth = img->w;
	const int dScrHeight = img->h;
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	SDL_Window *window = SDL_CreateWindow("Pepper's ghost", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, dScrWidth, dScrHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
	
	if(window == NULL){
		cerr << "Error in SDL_SetVideoMode: " << SDL_GetError() << endl;
		return -1;
	}
	
	SDL_GL_CreateContext(window);
	
	glewExperimental = true;
	GLenum err = glewInit();
	if(GLEW_OK != err){
		cerr << "Error in glewInit(): " << glewGetErrorString(err) << endl;
		return -1;
	}
	
	GLuint imgId;
	glGenTextures(1, &imgId);
	glBindTexture(GL_TEXTURE_2D, imgId);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,	0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
	SDL_FreeSurface(img);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.1f);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	
	GLuint program = loadShaders();
	
	glUseProgram(program);
	GLint t0_loc = glGetUniformLocation(program,"Texture0"); glUniform1i(t0_loc, 0);
	GLint delta_loc = glGetUniformLocation(program,"delta"); glUniform2f(delta_loc, 1.0/dScrWidth, 1.0/dScrHeight);
	
	int k[14]={0};
	int quit=0, frame=0, oldMs=0;
	float repet=14.5;
	float efac1=0.5;
	float efac2=0.25;
	float efac3=0.125;
	float efac4=0.06;
	float efac5=0.03;
	float gamma=2.2;
	bool disable=false;
	
	while(!quit){
	
		SDL_Event event;
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT) quit=1;
			if(event.type == SDL_KEYDOWN){
				if(event.key.keysym.sym == SDLK_ESCAPE) quit=1;
				if(event.key.keysym.sym == SDLK_q) k[0]=1;
				if(event.key.keysym.sym == SDLK_a) k[1]=1;
				if(event.key.keysym.sym == SDLK_w) k[2]=1;
				if(event.key.keysym.sym == SDLK_s) k[3]=1;
				if(event.key.keysym.sym == SDLK_e) k[4]=1;
				if(event.key.keysym.sym == SDLK_d) k[5]=1;
				if(event.key.keysym.sym == SDLK_r) k[6]=1;
				if(event.key.keysym.sym == SDLK_f) k[7]=1;				
				if(event.key.keysym.sym == SDLK_t) k[8]=1;
				if(event.key.keysym.sym == SDLK_g) k[9]=1;		
				if(event.key.keysym.sym == SDLK_y) k[10]=1;
				if(event.key.keysym.sym == SDLK_h) k[11]=1;			
				if(event.key.keysym.sym == SDLK_u) k[12]=1;		
				if(event.key.keysym.sym == SDLK_j) k[13]=1;		
				if(event.key.keysym.sym == SDLK_SPACE) disable=!disable;		
			}
			if(event.type == SDL_KEYUP){
				if(event.key.keysym.sym == SDLK_q) k[0]=0;
				if(event.key.keysym.sym == SDLK_a) k[1]=0;
				if(event.key.keysym.sym == SDLK_w) k[2]=0;
				if(event.key.keysym.sym == SDLK_s) k[3]=0;
				if(event.key.keysym.sym == SDLK_e) k[4]=0;
				if(event.key.keysym.sym == SDLK_d) k[5]=0;
				if(event.key.keysym.sym == SDLK_r) k[6]=0;
				if(event.key.keysym.sym == SDLK_f) k[7]=0;				
				if(event.key.keysym.sym == SDLK_t) k[8]=0;
				if(event.key.keysym.sym == SDLK_g) k[9]=0;			
				if(event.key.keysym.sym == SDLK_y) k[10]=0;
				if(event.key.keysym.sym == SDLK_h) k[11]=0;
				if(event.key.keysym.sym == SDLK_u) k[12]=0;		
				if(event.key.keysym.sym == SDLK_j) k[13]=0;						
			}
		}
		if(k[0]) repet+=0.1;
		if(k[1]) repet-=0.1;
		if(k[2]) efac1+=0.01;
		if(k[3]) efac1-=0.01;
		if(k[4]) efac2+=0.01;
		if(k[5]) efac2-=0.01;
		if(k[6]) efac3+=0.01;
		if(k[7]) efac3-=0.01;
		if(k[8]) efac4+=0.01;
		if(k[9]) efac4-=0.01;
		if(k[10]) efac5+=0.01;
		if(k[11]) efac5-=0.01;		
		if(k[12]) efac5+=0.1;
		if(k[13]) efac5-=0.1;			
		
		glViewport(0,0,dScrWidth,dScrHeight);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(program);
		GLint repet_loc = glGetUniformLocation(program,"repet"); 
		GLint efac1_loc = glGetUniformLocation(program,"efac1"); 
		GLint efac2_loc = glGetUniformLocation(program,"efac2"); 
		GLint efac3_loc = glGetUniformLocation(program,"efac3"); 
		GLint efac4_loc = glGetUniformLocation(program,"efac4"); 
		GLint efac5_loc = glGetUniformLocation(program,"efac5"); 
		GLint gamma_loc = glGetUniformLocation(program,"gamma"); 
		
		glUniform1f(repet_loc, repet);
		if(disable){
			glUniform1f(efac1_loc, 0.0f);
			glUniform1f(efac2_loc, 0.0f);
			glUniform1f(efac3_loc, 0.0f);
			glUniform1f(efac4_loc, 0.0f);
			glUniform1f(efac5_loc, 0.0f);
			glUniform1f(gamma_loc, gamma);
		}else{
			glUniform1f(efac1_loc, efac1);
			glUniform1f(efac2_loc, efac2);
			glUniform1f(efac3_loc, efac3);
			glUniform1f(efac4_loc, efac4);
			glUniform1f(efac5_loc, efac5);
			glUniform1f(gamma_loc, gamma);
		}
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, imgId);
				
		glBegin(GL_QUADS);
			glTexCoord2d(0, 1); glVertex3f( -1, +1, 0);
			glTexCoord2d(1, 1); glVertex3f( +1, +1, 0);
			glTexCoord2d(1, 0); glVertex3f( +1, -1, 0);
			glTexCoord2d(0, 0); glVertex3f( -1, -1, 0);		
		glEnd();
		
		SDL_GL_SwapWindow(window);
		SDL_Delay(2);
		
		{GLint err = glGetError();
		if(err!=0) cerr << err << endl;}
		
		frame++;
		if(frame==100){
			frame=0;
			int dt = SDL_GetTicks()-oldMs;
			oldMs = SDL_GetTicks();
			char bf[128];
			sprintf(bf, "%.2f FPS %f %f %f %f %f %f", 100000.0f/(float)dt, repet, efac1, efac2, efac3, efac4, efac5);
			SDL_SetWindowTitle(window, bf);
		}
	}
	
	SDL_Quit();
	return 0;
}
