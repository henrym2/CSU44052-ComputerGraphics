#include "boids.h"


const float TURN_FACTOR = 0.05f;
const float MARGIN = 5.0f;
const float CENTERING_FACTOR = 0.04f;
const float VISUAL_RANGE = 4.0f;

const float MIN_DISTANCE= 5.0f;
const float AVOID_FACTOR = 0.03f;
const float MATCH_FACTOR = 0.05f;
const float SPEED_LIMIT = 0.1f;

Boids::Boids(){}

Boids::Boids(std::vector<Mesh> mesh, int num, vec3 areaMax, vec3 areaMin) {
	boidMesh = mesh;
	num_boid = num;
	boidAreaMax = areaMax;
	boidAreaMin = areaMin;
	boids = new Boid[num];
	for (int i = 0; i < num; i++) {
		boids[i].model = Model(mesh);
		boids[i].id = i;
		float x, y, z;
		x = -(rand() % (int)boidAreaMin.v[0]) + (rand() % (int)boidAreaMax.v[0]);
		y = -(rand() % (int)boidAreaMin.v[1]) + (rand() % (int)boidAreaMax.v[1]);
		z = -(rand() % (int)boidAreaMin.v[2]) + (rand() % (int)boidAreaMax.v[2]);
		boids[i].location = vec3(x, y, z);
		
		float dx, dy, dz;

		dx = (float)rand() / RAND_MAX;
		dy = (float)rand() / RAND_MAX;
		dz = (float)rand() / RAND_MAX;

		boids[i].Velocity = vec3(dx, dy, dz);
	}
}

void Boids::generateObjectBufferMesh(Shader shaderProgram) {
	for (int i = 0; i < num_boid; i++) {
		boids[i].model.generateObjectBufferMesh(shaderProgram);
	}
}

float Boids::distance(Boid boid1, Boid boid2) {
	float total = pow(boid1.location.v[0] - boid2.location.v[0], 2) 
		+ pow(boid1.location.v[1] - boid2.location.v[1], 2) 
		+ pow(boid1.location.v[2] - boid2.location.v[2], 2);
	return sqrt(total);
}

void Boids::keepInBounds(Boid &boid) {
	if (boid.location.v[0] < -boidAreaMin.v[0]) {
		boid.Velocity.v[0] += TURN_FACTOR;
	}

	if (boid.location.v[0] > boidAreaMax.v[0]) {
		boid.Velocity.v[0] -= TURN_FACTOR;
	}

	if (boid.location.v[1] < -boidAreaMin.v[1]) {
		boid.Velocity.v[1] += TURN_FACTOR;
	}

	if (boid.location.v[1] > boidAreaMax.v[1]) {
		boid.Velocity.v[1] -= TURN_FACTOR;
	}

	if (boid.location.v[2] < -boidAreaMin.v[2]) {
		boid.Velocity.v[2] += TURN_FACTOR;
	}

	if (boid.location.v[2] > boidAreaMax.v[2]) {
		boid.Velocity.v[2] -= TURN_FACTOR;
	}
}

void Boids::flyTowardsCenter(Boid &boid) {
	float centerX = 0;
	float centerY = 0;
	float centerZ = 0;
	int neighbors = 0;

	for (int i = 0; i < num_boid; i++) {
		if (distance(boid, boids[i])) {
			centerX += boids[i].location.v[0];
			centerY += boids[i].location.v[1];
			centerZ += boids[i].location.v[2];
			neighbors += 1;
		}
	}

	if (neighbors) {
		centerX = centerX / neighbors;
		centerY = centerY / neighbors;
		centerZ = centerZ / neighbors;

		boid.Velocity.v[0] += (centerX - boid.location.v[0]) * CENTERING_FACTOR;
		boid.Velocity.v[1] += (centerY - boid.location.v[1]) * CENTERING_FACTOR;
		boid.Velocity.v[2] += (centerZ - boid.location.v[2]) * CENTERING_FACTOR;

	}
}

void Boids::avoidOthers(Boid &boid) {
	float moveX = 0;
	float moveY = 0;
	float moveZ = 0;

	for (int i = 0; i < num_boid; i++) {
		if (boid.id != boids[i].id) {
			if (distance(boid, boids[i]) < MIN_DISTANCE) {
				moveX += boid.location.v[0] - boids[i].location.v[0];
				moveY += boid.location.v[1] - boids[i].location.v[1];
				moveZ += boid.location.v[2] - boids[i].location.v[2];

			}
		}
	}

	boid.Velocity.v[0] += moveX * AVOID_FACTOR;
	boid.Velocity.v[1] += moveY * AVOID_FACTOR;
	boid.Velocity.v[2] += moveZ * AVOID_FACTOR;

}

void Boids::matchVelocity(Boid &boid) {
	float neighbors = 0;
	vec3 avg;

	for (int i = 0; i < num_boid; i++) {
		if (distance(boid, boids[i]) < VISUAL_RANGE) {
			avg += boids[i].Velocity;
		}
	}

	if (neighbors) {
		avg = avg / neighbors;
		boid.Velocity += (avg - boid.Velocity) * MATCH_FACTOR;
	}
}

void Boids::limitSpeed(Boid &boid) {
	const float speed = (boid.Velocity.v[0] + boid.Velocity.v[1] + boid.Velocity.v[2])/3;
	if (speed > SPEED_LIMIT) {
		boid.Velocity = (boid.Velocity / speed) * SPEED_LIMIT;
	}
}

void drawBoid(Boid &boid, Shader shader) {
	int matrix_location = glGetUniformLocation(shader.ID, "model");
	mat4 boidMat = identity_mat4();
	float angle = static_cast<float>(atan2(boid.Velocity.v[0], -boid.Velocity.v[2]) * 180 / M_PI);
	boidMat = rotate_x_deg(boidMat, angle);
	boidMat = translate(boidMat, boid.location);
	boid.model.draw(boidMat, matrix_location, 0, shader);
}

void Boids::drawAll(Shader shader) {
	shader.use();
	for (int i = 0; i < num_boid; i++) {
		drawBoid(boids[i], shader);
	}
}

void Boids::updateAll() {
	for (int i = 0; i < num_boid; i++) {

		flyTowardsCenter(boids[i]);
		avoidOthers(boids[i]);
		matchVelocity(boids[i]);
		limitSpeed(boids[i]);
		keepInBounds(boids[i]);


		boids[i].location += boids[i].Velocity;
	}
}	