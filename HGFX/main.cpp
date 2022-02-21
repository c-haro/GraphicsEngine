
#include <glad/glad.h> 
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"
#include <SDL_ttf.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include <map>
#include FT_FREETYPE_H 


//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


unsigned int VAO[2], VBO[2], EBO;

void ProcessEvent(SDL_Event e);
//The window we'll be rendering to
SDL_Window* gWindow = NULL;


bool quit = false;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;


// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

std::map<char, Character> Characters;

void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color)
{
	// activate corresponding render state	
	s.use();
	glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO[1]);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void processInput(const Uint8* state)
{

	//stepz += ((int)state[SDL_SCANCODE_W] * step_value) + -((int)state[SDL_SCANCODE_S] * step_value);
	//stepx += ((int)state[SDL_SCANCODE_A] * step_value) + -((int)state[SDL_SCANCODE_D] * step_value);

	if ((int)state[SDL_SCANCODE_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if ((int)state[SDL_SCANCODE_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if ((int)state[SDL_SCANCODE_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if ((int)state[SDL_SCANCODE_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if ((int)state[SDL_SCANCODE_SPACE])
		camera.ProcessKeyboard(JUMP, deltaTime);
}

void FreetypeLoad() {
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return ;
	}

	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}


	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}


	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}


int main(int argc, char* args[])
{

	

	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;

	//OpenGL context
	SDL_GLContext gContext;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			return -1;
		}
		else
		{
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				return -1;
			}
			else
			{
				//Initialize GLAD
				if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
				{
					printf("Error initializing GLAD!\n");
					return -1;
				}

				////identity matrix 
				//glm::mat4 trans = glm::mat4(1.0f);
				////rotate 
				//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
				////then scale 
				//trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

				FreetypeLoad();

				glEnable(GL_DEPTH_TEST);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


				Shader ourShader("vert.glsl", "frag.glsl");

				Shader textShader("TextV.glsl", "TextF.glsl");

				float vertices[] = {
					-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
				};

				glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
				};
				//unsigned int indices[] = {
				//	0, 1, 3, // first triangle
				//	1, 2, 3  // second triangle
				//};

				glGenBuffers(1, &EBO);
				glGenBuffers(2, VBO);
				glGenVertexArrays(2, VAO);

				// 1. bind Vertex Array Object
				glBindVertexArray(VAO[0]);

				glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/
				// 1. then set the vertex attributes pointers
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);
				//color
				/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(1);*/
				//texture
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(1);

				SDL_SetRelativeMouseMode(SDL_TRUE);

				//Text Buffer
				// 1. bind Vertex Array Object
				glBindVertexArray(VAO[1]);
				glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);




				glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);



				unsigned int texture1, texture2;

				glGenTextures(1, &texture1);
				glBindTexture(GL_TEXTURE_2D, texture1);
				//texture wraping params
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				//texture filtering params
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				// load image, create texture and generate mipmaps
				int width, height, nrChannels;
				stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
				unsigned char* data = stbi_load("images/container.jpg", &width, &height, &nrChannels, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);




				glGenTextures(1, &texture2);
				glBindTexture(GL_TEXTURE_2D, texture2);
				//texture wraping params
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				//texture filtering params
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				data = stbi_load("images/awesomeface.png", &width, &height, &nrChannels, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
				glGenerateMipmap(GL_TEXTURE_2D);



				ourShader.use();

				ourShader.setInt("texture1", 0);

				ourShader.setInt("texture2", 1);

				float stepz = -3.0f, stepx = 0.5f;

				

				//bool incz = false, decz = false;
				//Get the keystates

				// pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
	// -----------------------------------------------------------------------------------------------------------
				glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
				ourShader.setMat4("proj", projection);

				const Uint8* state = SDL_GetKeyboardState(NULL);
				const float step_value = 0.1f;

				
				while (!quit)
				{
					Uint64 start = SDL_GetPerformanceCounter();	
					float currentFrame = (float)(SDL_GetTicks() / 1000.0f);
					deltaTime = currentFrame - lastFrame;
					lastFrame = currentFrame;

					
					
					SDL_Event e;
					while (SDL_PollEvent(&e))
					{
						ProcessEvent(e);
					}
					processInput(state);

					glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texture1);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, texture2);


					//Render
					ourShader.use();
					

					// pass projection matrix to shader (note that in this case it could change every frame)
					glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
					ourShader.setMat4("projection", projection);

					// camera/view transformation
					glm::mat4 view = camera.GetViewMatrix();
					ourShader.setMat4("view", view);

					

					



					//Transformations
					/*glm::mat4 model = glm::mat4(1.0f);
					model = glm::rotate(model, (float)(SDL_GetTicks() / 1000.0f) * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));*/


					//glm::mat4 view = glm::mat4(1.0f);

					//Camera
					//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

					////Camera Target
					//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
					//glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
					////RIght axis
					//glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
					//glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
					////Camera Up
					//glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

					//const float radius = 10.0f;
					//float camX = sin(SDL_GetTicks()/1000.0f) * radius;
					//float camZ = cos(SDL_GetTicks()/1000.0f) * radius;
					//glm::mat4 view;
					//view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
					//Pos , Target, Up
					//glm::mat4 view;

					

					// note that we're translating the scene in the reverse direction of where we want to move
					
					



					//glm::mat4 trans = glm::mat4(1.0f);

					//trans = glm::translate(trans, glm::vec3(stepx, stepy, 0.0f));
					//trans = glm::rotate(trans, (float)(SDL_GetTicks() / 1000.0f), glm::vec3(0.0f, 0.0f, 1.0f));

					//ourShader.setMat("transform", trans);


					//ourShader.setMat4("view", view);
					



					glBindVertexArray(VAO[0]);
					for (unsigned int i = 0; i < 10; i++)
					{
						glm::mat4 model = glm::mat4(1.0f);
						model = glm::translate(model, cubePositions[i]);
						float angle = 20.0f * i;
						model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
						ourShader.setMat4("model", model);

						glDrawArrays(GL_TRIANGLES, 0, 36);
					}


					//std::cout << "time:" << deltaTime << std::endl;
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::scale(model, glm::vec3(5, .01, 5));
					model = glm::translate(model, glm::vec3(0, -10, 0));
					ourShader.setMat4("model", model);
					glDrawArrays(GL_TRIANGLES, 0, 36);



					Uint64 end = SDL_GetPerformanceCounter();

					float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
					int fixed = (int)(1 / elapsed);

					std::cout << "Current FPS: " << (int)(1 / elapsed) << std::endl;
					
					//Text Rendering
					textShader.use();
					projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT);
					textShader.setMat4("projection", projection);
					RenderText(textShader, "FPS: " + std::to_string(fixed), 0.0f, (float)SCREEN_HEIGHT-45, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
					RenderText(textShader, "HELLO", 0.0f, (float)SCREEN_HEIGHT /2, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));



					SDL_GL_SwapWindow(gWindow);

					
					
				}
				SDL_Quit();
			}
		}
	}
	return 0;
}


void ProcessEvent(SDL_Event e)
{


	if (e.type == SDL_QUIT)
		quit = true;

	if (e.type == SDL_KEYDOWN)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			quit = true;
			break;
			/*case SDLK_w:
				incz = !(incz);
				break;*/
				/*case SDLK_s:

					decz = !(decz);
					break;*/
					/*case SDLK_d:
						if (stepx <= 1.0f)
							stepx += 0.1f;
						ourShader.setFloat("stepx", stepx);
						break;
					case SDLK_a:
						if (stepx >= -1.0f)
							stepx -= 0.1f;

						ourShader.setFloat("stepx", stepx);
						break;*/
		default:
			break;
		}

	}

	if (e.type == SDL_KEYUP)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_ESCAPE:

			break;
			/*case SDLK_w:
				incz = !(incz);
				break;*/
				/*case SDLK_s:
					decz = !(decz);
					break;*/
		case SDLK_d:

			break;
		case SDLK_a:

			break;
		default:
			break;
		}
	}


	if (e.type == SDL_WINDOWEVENT)
	{
		if (e.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			int w, h;
			SDL_GetWindowSize(gWindow, &w, &h);

			//std::cout << "width: " << w << " height: " << h << std::endl;
			glViewport(0, 0, w, h);
		}
	}

	if (e.type == SDL_MOUSEMOTION)
	{

		double xposIn = e.motion.xrel;
		double yposIn = -e.motion.yrel;

		//std::cout << "x: " << e.motion.xrel << " y: " << e.motion.yrel << std::endl;
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		//if (firstMouse)
		//{
		//	lastX = xpos;
		//	lastY = ypos;
		//	firstMouse = false;
		//}

		//float xoffset = xpos - lastX;
		//float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		//lastX = xpos;
		//lastY = ypos;
		camera.ProcessMouseMovement(xpos, ypos, false);
		//camera.ProcessMouseMovement(xoffset, yoffset,false);
	}

}

