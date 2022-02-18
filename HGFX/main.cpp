
#include <glad/glad.h> 
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;




int main(int argc, char* args[])
{

	//The window we'll be rendering to
	SDL_Window* gWindow = NULL;

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



				glEnable(GL_DEPTH_TEST);


				Shader ourShader("vert.glsl", "frag.glsl");

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

				unsigned int VAO[2], VBO[2], EBO;
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

				float SDtime = (float)(SDL_GetTicks() / 100000.0f);
				bool quit = false;

				bool incz = false, decz = false;
				//Get the keystates
				const Uint8* state = SDL_GetKeyboardState(NULL);
				const float step_value = 0.1f;

				while (!quit)
				{
					

					SDtime = (float)(SDL_GetTicks() / 100000.0f);
					SDL_Event e;
					while (SDL_PollEvent(&e))
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

								std::cout << "width: " << w << " height: " << h << std::endl;
								glViewport(0, 0, w, h);
							}
						}
					}

					/*if(incz){ stepz += 0.1f; }
					if(decz) { stepz -= 0.1f; }*/

					
					stepz += ((int)state[SDL_SCANCODE_W] * step_value) + -((int)state[SDL_SCANCODE_S] * step_value);
					stepx += ((int)state[SDL_SCANCODE_A] * step_value) + -((int)state[SDL_SCANCODE_D] * step_value);
					
					std::cout << stepz << std::endl;
					

					
					


					//Render
					ourShader.use();


					//Transformations
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::rotate(model, (float)(SDL_GetTicks() / 1000.0f) * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));


					glm::mat4 view = glm::mat4(1.0f);
					// note that we're translating the scene in the reverse direction of where we want to move
					view = glm::translate(view, glm::vec3(stepx, -1.0f, stepz));

					glm::mat4 proj;
					proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

					//Camera



					//glm::mat4 trans = glm::mat4(1.0f);

					//trans = glm::translate(trans, glm::vec3(stepx, stepy, 0.0f));
					//trans = glm::rotate(trans, (float)(SDL_GetTicks() / 1000.0f), glm::vec3(0.0f, 0.0f, 1.0f));

					//ourShader.setMat("transform", trans);


					ourShader.setMat("view", view);
					ourShader.setMat("proj", proj);

					glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texture1);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, texture2);


					glBindVertexArray(VAO[0]);
					for (unsigned int i = 0; i < 10; i++)
					{
						glm::mat4 model = glm::mat4(1.0f);
						model = glm::translate(model, cubePositions[i]);
						float angle = 20.0f * i;
						model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
						ourShader.setMat("model", model);

						glDrawArrays(GL_TRIANGLES, 0, 36);
					}





					SDL_GL_SwapWindow(gWindow);
				}
				SDL_Quit();
			}
		}
	}
	return 0;
}
