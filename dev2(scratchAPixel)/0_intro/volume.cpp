//clang++ -O3 raymarch_chap0.cpp -o render -std=c++17 -DBACKWARD_RAYMARCHING

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include "SDL2Auxiliary/SDL2Auxiliary.h"
#include <vector>
#include <random>
#include <glm.hpp>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::ivec2;
using glm::vec2;

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
vec3 cameraPos(0, 0, -2);
SDL2Aux sdl(SCREEN_WIDTH, SCREEN_HEIGHT); 
int t; 
glm::mat3 R; 
float yaw; 
float speed = 0.0005f;
float rotateSpeed = 0.0005f;



const vec3 background_color{ 0.572f, 0.772f, 0.921f };
const float floatMax = std::numeric_limits<float>::max();

vec3 computeRay(int x, int y, float focalLength);
void saveImage(vec3 image_buffer[SCREEN_WIDTH][SCREEN_HEIGHT], const std::string& filename);
void Draw();
void Update();

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
	/*

	Update the rotation matrix R

	pag 163 of course book

	Ry = 
	| cos(θ) 	0 	sin(θ) | //right vector
	| 0 	 	1 	0 	   | //down  vector
	| -sin(θ) 	0	cos(θ) | //forward vector
	*/
	//update of the rotation matrix
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
	// if(keystate [SDL_SCANCODE_W] )
	// {
	// 	// Move light forward
	// 	lightPos += dt*speed*forward;
	// }
	// if(keystate [SDL_SCANCODE_S] )
	// {
	// 	// Move light backward
	// 	lightPos -= dt*speed*forward;
	// }
	// if(keystate [SDL_SCANCODE_D] )
	// {
	// 	// Move light to the right
	// 	lightPos += dt*speed*right;
	// }
	// if(keystate [SDL_SCANCODE_A] )
	// {
	// 	// Move light to the left
	// 	lightPos -= dt*speed*right;
	// }

};


struct IsectData
{
    float t0{ floatMax }, t1{ floatMax };
    vec3 pHit;
    vec3 nHit;
    bool inside{ false };
};
struct Object
{
public:
    vec3 color;
    int type{ 0 };
    virtual bool intersect(vec3 ray_origin, vec3 ray_direction, float &t0, float &t1) const = 0;
    virtual ~Object() {}
    Object() {}
};

class Sphere: public Object{
    public:
        bool intersect(vec3 ray_origin, vec3 ray_direction, float &t0, float &t1) const{
            //computing ray-sphere intersection
            // P(t) = ray_origin + t*ray_direction = equation of the ray => t is the distance along the ray that we want to find
            // (x-center.x)^2 + (y-center.y)^2 + (z-center.x)^2 = radius^2 => ||P - C||^2 = R^2
            //intersection --> ||ray_origin + t*ray_direction - C ||^2 = R^2
            // ||ray_origin-C + t*ray_direction ||^2 = R^2
            vec3 L = ray_origin - center; // ||L + t*ray_direction||^2 = R^2
            // (L + t*ray_direction)*(L + t*ray_direction) = L^2 + 2tL*ray_direction + t^2*ray_direction^2 = at^2 + bt + c
            float a = glm::dot(ray_direction, ray_direction);
            float b = 2 * glm::dot(L, ray_direction);
            float c = glm::dot(L, L)-radius*radius;
            // disriminant = b^2 - 4ac
            float delta = b*b - 4*a*c;
            if(delta < 0) return false;
            t0 = (-b - sqrt(delta))/(2*a);
            t1 = (-b + sqrt(delta))/(2*a);

            return true;
        };
        float sigma_a = 0.5; //absorption coefficient 
        //80% is tarnsmitted, the green is absorbed, the blue is transmitted at 50%
        vec3 scatter = vec3(0.8, 0.1, 0.5); // used to determine the final color of the transmitted or reflected light 
        vec3 center = vec3(0, 0, 0);
        float radius = 1;
};

vec3 traceScene(vec3 ray_origin, vec3 ray_direction, Sphere* sphere){
    float t0, t1;
    vec3 background_color = vec3(0.572, 0.772, 0.921);
    if(sphere->intersect(ray_origin, ray_direction, t0, t1)){ //we use -> because we have the pointer, if we have an object we must use .
        if(t1<0){
            //it means that the sphere is behind us
            return background_color;
        }
        float tStart = std::max(0.0f, t0); //if t0 < 0 it means we are inside the sphere
        // (1) if there is intersection, we calculate the points on the surface of the sphere where the ray enters and leaves the sphere
        vec3 p1 = ray_origin + ray_direction * tStart;
        vec3 p2 = ray_origin + ray_direction * t1;
        float distance = glm::length(p2-p1); //distance of the ray in the sphere: how much material the ray has crossed 
        
        // (2) we then compute beer's law to compute how much of the light is being transmitted through the sphere
        /**
         * BEER LAMBERT LAW
         * 
         * the concept of density is expressed in terms of the absorption coefficient (and scattering coefficient).
         * Essentially, "the denser the volume, the higher the absorption coefficient"
         */
        float transmission = exp(-distance * sphere->sigma_a);  //how the light is absorbed passing through a mean
        
        return background_color * transmission + sphere->scatter * (1 - transmission); 
    } else {
        return background_color;
    }
}

// void renderImage(){
//     vec3 image_buffer[SCREEN_WIDTH][SCREEN_HEIGHT];
//     Sphere sphere;
//     sphere.center = vec3(0, 0, 0);
//     sphere.radius = 1.0f;
//     sphere.sigma_a = 0.1f;
//     for(int y = 0; y < SCREEN_HEIGHT; y++){
//         for(int x = 0; x < SCREEN_WIDTH; x++){
//             vec3 ray_dir = computeRay(x, y, SCREEN_HEIGHT/2);
//             vec3 pixel_color = traceScene(cameraPos, ray_dir, &sphere);
//             sdl.putPixel(x, y, pixel_color);
//             //image_buffer[x][y] = pixel_color; 
//         }

//     } 
//     saveImage(image_buffer, "output.ppm");  
// }

vec3 computeRay(int x, int y, float focalLength){
    vec3 dir = vec3(
        x - SCREEN_WIDTH/2, 
        y - SCREEN_HEIGHT/2,
        focalLength
    );
    return glm::normalize(R*dir);
}

void saveImage(vec3 image_buffer[SCREEN_WIDTH][SCREEN_HEIGHT], const string& filename = "output.ppm") {
    ofstream file(filename);
    if (!file) {
        cerr << "Cannot open file " << filename << endl;
        return;
    }

    // Header PPM
    file << "P3\n" << SCREEN_WIDTH << " " << SCREEN_HEIGHT << "\n255\n";

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            // converti float 0-1 in int 0-255
            int r = static_cast<int>(min(max(image_buffer[x][y].x, 0.0f), 1.0f) * 255);
            int g = static_cast<int>(min(max(image_buffer[x][y].y, 0.0f), 1.0f) * 255);
            int b = static_cast<int>(min(max(image_buffer[x][y].z, 0.0f), 1.0f) * 255);
            file << r << " " << g << " " << b << " ";
        }
        file << "\n";
    }

    file.close();
    cout << "Image saved to " << filename << endl;
}

int main(){
    t = SDL_GetTicks();
    while (!sdl.quitEvent()) {
        Update();
        Draw();
    }
    
}
void Draw(){
    Sphere sphere;

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            vec3 ray_dir = computeRay(x, y, SCREEN_HEIGHT/2);
            vec3 color = traceScene(cameraPos, ray_dir, &sphere);
            sdl.putPixel(x, y, color);   
        }
    }
    sdl.render();
}