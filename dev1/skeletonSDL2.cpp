// DH2323 Lab 1
// Introduction lab that covers:
// * SDL2 (https://www.libsdl.org/)
// * C++, std::vector and glm::vec3 (https://glm.g-truc.net)
// * 2D graphics
// * Plotting pixels
// * Video memory
// * Color representation
// * Linear interpolation

#include <iostream>
#include <glm/glm.hpp>
#include "../SDL2Auxiliary/SDL2Auxiliary.h"
#include "TestModel.h"
#include <algorithm>
#include <numbers>
#include <cmath>

using namespace std;
using glm::vec3;
using glm::mat3;

// ---------------------------------------------------------
// GLOBAL VARIABLES
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL2Aux *sdlAux;
vec3 cameraPos(0, 0, -1.0f);
float focalLength = SCREEN_HEIGHT/2;
glm::mat3 R = glm::mat3(1.0f); //rotation matrix
float yaw = 0; //angle which the camera should be rotated around the y-axis
float speed = 0.0005f;
float rotateSpeed = 0.0005f;
int t;

// ---------------------------------------------------------
// FUNCTION DECLARATIONS
void Draw();
void Update(); 
// ---------------------------------------------------------

// FUNCTION DEFINITIONS
int main(int argc, char* argv[])
{	

	sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();
	while (!sdlAux->quitEvent()) {
		Update();
		Draw();
	}
	
	sdlAux->saveBMP("screenshot.bmp");
	return 0;
}

void Draw()
{
	sdlAux->clearPixels(); //screen cleared before we draw it
	
	float radius = 0.5f;
	vec3 sphereCenter(0, 0, 0);
	float stepSize = 0.05f; //arbitrary
	//coloring with black
	for (int y = 0; y<SCREEN_HEIGHT; ++y)
	{			
		for (int x = 0; x<SCREEN_WIDTH; ++x)
		{	
			//direction of the ray of the single pixel
			vec3 dir(
                x - SCREEN_WIDTH/2, 
                y - SCREEN_HEIGHT/2,
                focalLength
            );

			//multypling by R to rotate direction of the ray with respect to the camera
			dir = R * glm::normalize(dir); //we normalize dir to have stepSize constant
			vec3 currentPos = cameraPos;
			vec3 pixelColor(0, 0, 0); //the background is black
			float opacity = 0;


			//ray marching loop: checking if we are in the sphere
			float transmittance = 1.0f;

			for(int i=0; i<64; i++){
				//in a for cycle we go on with little steps
				currentPos += dir * stepSize;

				//testing if we are inside (with euclidian distance)
				float distanceToCenter = glm::distance(currentPos, sphereCenter);
				
				
				if(distanceToCenter < radius){

					// //we are inside the sphere -> image 0
					// pixelColor = vec3(1.0, 1.0, 1.0);
					// break; 

					//************** image1: nebula effect*/
					//float density = (radius - distanceToCenter); //more dense in the center (think about a cloud)
					/************** */

					//models the density with a spatial frequency
					float noise = (sin(currentPos.x*10.0f)*cos(currentPos.y*10.0f)*sin(currentPos.z*10.0f)) * 0.5f + 0.5f;
					float density = (radius - distanceToCenter)*noise;
					
					float absorption = 0.5f; //coefficient of absorption
					float localOpacity = density * stepSize * absorption; 

					float brightness = 10.0f;
					pixelColor += transmittance * localOpacity * vec3(1.0, 1.0, 1.0) * brightness;
					
					transmittance *= (1.0f - localOpacity); //reducing light that passes for future samples
					//opacity += density*stepSize;
					if(transmittance <= 0.01f) break; //if all is opaque, break

				}
				//************** */
			}
			sdlAux->putPixel(x, y, pixelColor);
		}
	}


	sdlAux->render();
}


void Update(void)
{	//vec3 cameraPos(0, 0, -2);
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	//cout << "Render time: " << dt << " ms." << endl;
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	
	if(keystate [SDL_SCANCODE_LEFT] )
	{
		// Move camera to the left, negative x axis
		//cameraPos.x -= dt*speed;
		yaw -= rotateSpeed*dt; //update of yaw 
        
	}
	if(keystate [SDL_SCANCODE_RIGHT] )
	{
		// Move camera to the right, positive x axis
		//cameraPos.x += dt*speed;
		yaw += rotateSpeed*dt;//update of yaw 
	}
	/** */

	R = mat3(
		vec3 (cos(yaw), 0, -sin(yaw)), 
		vec3(0, 1, 0),
		vec3(sin(yaw), 0, cos(yaw))
	);

	//task 5.4
	vec3 right(		R[0][0], R[0][1], R[0][2]);
	vec3 down(		R[1][0], R[1][1], R[1][2]);
	vec3 forward (	R[2][0], R[2][1], R[2][2]);	
	if ( keystate [SDL_SCANCODE_UP] )
	{
	 	//move camera forward, towards z positive
	 	cameraPos += dt*speed * forward;
	}
	if(keystate [SDL_SCANCODE_DOWN])
	{
		// Move camera backward, towards z negative
		cameraPos -= dt*speed * forward;
	}
	if(keystate [SDL_SCANCODE_L] )
	{
		// Move camera to the left, negative x axis
		cameraPos -= dt*speed*right;
        
	}
	if(keystate [SDL_SCANCODE_R] )
	{
		// Move camera to the right, positive x axis
		cameraPos += dt*speed*right;
	}
	if(keystate [SDL_SCANCODE_U] )
	{
		// Move camera to the left, negative x axis
		cameraPos -= dt*speed*down;
        
	}
	if(keystate [SDL_SCANCODE_B] )
	{
		// Move camera to the right, positive x axis
		cameraPos += dt*speed*down;
	}

};
