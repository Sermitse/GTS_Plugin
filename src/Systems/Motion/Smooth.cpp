#include "Systems/Motion/Smooth.hpp"

namespace {

	// Spring code from https://theorangeduck.com/page/spring-roll-call
	float halflife_to_damping(float halflife, float eps = 1e-5f){
		return (4.0f * 0.69314718056f) / (halflife + eps);
	}

	float damping_to_halflife(float damping, float eps = 1e-5f){
		return (4.0f * 0.69314718056f) / (damping + eps);
	}

	float fast_negexp(float x){
		return 1.0f / (1.0f + x + 0.48f*x*x + 0.235f*x*x*x);
	}
}

namespace GTS {

	void critically_damped(float& x, float& v, float x_goal, float halflife, float dt){
		float y = halflife_to_damping(halflife) / 2.0f;
		float j0 = x - x_goal;
		float j1 = v + j0*y;
		float eydt = fast_negexp(y*dt);

		x = eydt*(j0 + j1*dt) + x_goal;
		v = eydt*(v - j1*y*dt);
	}

	//https://www.desmos.com/calculator/8lqgse3jkr
	//https://www.desmos.com/calculator/peog2oomvo
	float bezier_curve(const float x,const float x1, const float x2, const float x3, const float x4, const float i, const float k) {
		return pow((((1-x)*(1-x)*(1-x)*x1) + (3.0f*(1-x)*(1-x)*x*x2) + (3.0f*(1-x)*x*x*x3) + (x*x*x*x4)),i)*k;
		//i determines the steepness of the curve - how slow or fast it accelerates and decelerates. Should be between two and three (two might be the best one)
		//x1 should always be 0, x3 and x4 should always be 1
		//x2 determines if it accelerates fast but deceleares slow, or vice verca. Should be between 0.8 and 1.
	}
	// https://www.desmos.com/calculator/pqgliwxzi2
	float soft_power(const float x, const float k,const float n,const float s,const float o,const float a) {
		return pow(1.0f+pow(k*(x),n*s),1.0f/s)/pow(1.0f+pow(k*o,n*s), 1.0f/s) + a;
	}
	// https://www.desmos.com/calculator/vyofjrqmrn
	float soft_core(const float x, const float k, const float n, const float s, const float o, const float a) {
		return 1.0f/soft_power(x, k, n, s, o, 0.0f) + a;
	}

	float soft_core(const float x, const SoftPotential& soft_potential) {
		return soft_core(x, soft_potential.k, soft_potential.n, soft_potential.s, soft_potential.o, soft_potential.a);
	}

	float soft_power(const float x, const SoftPotential& soft_potential) {
		return soft_power(x, soft_potential.k, soft_potential.n, soft_potential.s, soft_potential.o, soft_potential.a);
	}

	// https://en.wikipedia.org/wiki/Smoothstep
	float clamp(float lowerlimit, float upperlimit, float x) {
		if (x < lowerlimit) {
			x = lowerlimit;
		}
		if (x > upperlimit) {
			x = upperlimit;
		}
		return x;
	}

	float smootherstep(float edge0, float edge1, float x) {
		// Scale, and clamp x to 0..1 range
		x = clamp(0.0f, 1.0f, (x - edge0) / (edge1 - edge0));
		// Evaluate polynomial
		return x * x * x * (x * (x * 6 - 15) + 10);
	}

	float smoothstep (float edge0, float edge1, float x) {
		// Scale/bias into [0..1] range
		x = clamp(0.0f, 1.0f, (x - edge0) / (edge1 - edge0));

		return x * x * (3 - 2 * x);
	}
}
