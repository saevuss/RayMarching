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
#include <vector>
#include "SDL2Auxiliary/SDL2Auxiliary.h"

using namespace std;
using glm::vec3;

// ---------------------------------------------------------
// GLOBAL VARIABLES
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL2Aux *sdlAux;
vec3 cameraPos(0, 0, -2);
float focalLength = SCREEN_HEIGHT/2;


// ---------------------------------------------------------
// FUNCTION DECLARATIONS
void Draw();
void Update(); 
// ---------------------------------------------------------

// FUNCTION DEFINITIONS
int main(int argc, char* argv[])
{	

	sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);
	while (!sdlAux->quitEvent()) {
		//Update();
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
			dir = glm::normalize(dir); //we normalize dir to have stepSize constant
			vec3 currentPos = cameraPos;
			vec3 pixelColor(0, 0, 0); //the background is black
			float opacity = 0;


			//ray marching loop: checking if we are in the sphere
			for(int i=0; i<64; i++){
				//in a for cycle we go on with little steps
				currentPos += dir * stepSize;

				//testing if we are inside (with euclidian distance)
				float distanceToCenter = glm::distance(currentPos, sphereCenter);
				if(distanceToCenter < radius){
					//we are inside the sphere -> image 0
					pixelColor = vec3(1.0, 1.0, 1.0);
					break; 
				}
			}
			sdlAux->putPixel(x, y, pixelColor);
		}
	}


	sdlAux->render();
}


void Update(){
}
