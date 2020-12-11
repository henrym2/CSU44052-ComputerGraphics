#pragma once
//SOURCE ::::::: https://learnopengl.com/ :::::::
#define CAMERA_H
#define _USE_MATH_DEFINES


#include <GL/glew.h>
#include <GL/freeglut.h>
#include "maths_funcs.h"
#include <vector>
#include <math.h>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -100.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 55.0f;

class Camera {
public:
	vec3 Position;
	vec3 Front;
	vec3 Up;
	vec3 Right;
	vec3 WorldUp;
	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	Camera(vec3 position = vec3(0.0f, 0.0f, 3.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	mat4 GetViewMatrix()
	{
		return look_at(Position, Position + Front, Up);
	}

	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > ZOOM * 2.0f)
			Zoom = ZOOM * 2.0f;
	}

	private:
	// calculates the front vector from the Camera's (updated) Euler Angles
		void updateCameraVectors()
		{
			// calculate the new Front vector
			vec3 front;
			front.v[0] = cos(Convert(Yaw)) * cos(Convert(Pitch));
			front.v[1] = sin(Convert(Pitch));
			front.v[2] = sin(Convert(Yaw)) * cos(Convert(Pitch));
			
			Front = normalise(front);
			// also re-calculate the Right and Up vector
			Right = normalise(cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = normalise(cross(Right, Front));
		}

		float Convert(float degree) {
			float pi = M_PI;
			return (degree * (pi / 180));
		}
};
