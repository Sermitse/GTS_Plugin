
#include "Systems/Motion/Spring.hpp"

namespace {

	// Spring code from https://theorangeduck.com/page/spring-roll-call
	float halflife_to_damping(float halflife, float eps = 1e-5f)
	{
		return (4.0f * std::numbers::ln2_v<float>) / (halflife + eps);
	}

	float damping_to_halflife(float damping, float eps = 1e-5f)
	{
		return (4.0f * std::numbers::ln2_v<float>) / (damping + eps);
	}
	float fast_negexp(float x)
	{
		return 1.0f / (1.0f + x + 0.48f*x*x + 0.235f*x*x*x);
	}
}

namespace GTS {

	void SpringBase::UpdateValues(float& value, const float& target, float & velocity, const float& halflife, const float& dt) {
		if (std::isinf(target)) {
			return;
		}
		if (fabs(target - value) < 1e-4 && velocity < 1e-4) {
			return;
		}
		float y = halflife_to_damping(halflife) / 2.0f;
		float j0 = value - target;
		float j1 = velocity + j0*y;
		float eydt = fast_negexp(y*dt);

		value = eydt*(j0 + j1*dt) + target;
		velocity = eydt*(velocity - j1*y*dt);
	}

	void Spring::Update(float dt) {
		UpdateValues(this->value, this->target, this->velocity, this->halflife, dt);
	}

	Spring::Spring() {
		SpringHolder::AddSpring(this);
	}

	Spring::Spring(float initial, float halflife) : value(initial), target(initial), halflife(halflife) {
		SpringHolder::AddSpring(this);
	}

	Spring::~Spring() {
		SpringHolder::RemoveSpring(this);
	}

	void Spring3::Update(float delta) {
		UpdateValues(this->value.x, this->target.x, this->velocity.x, this->halflife, delta);
		UpdateValues(this->value.y, this->target.y, this->velocity.y, this->halflife, delta);
		UpdateValues(this->value.z, this->target.z, this->velocity.z, this->halflife, delta);
	}

	Spring3::Spring3() {
		SpringHolder::AddSpring(this);
	}

	Spring3::Spring3(NiPoint3 initial, float halflife) : value(initial), target(initial), halflife(halflife) {
		SpringHolder::AddSpring(this);
	}

	Spring3::~Spring3() {
		SpringHolder::RemoveSpring(this);
	}

	void SpringHolder::AddSpring(SpringBase* spring)  {
		springs.insert(spring);
	}
	void SpringHolder::RemoveSpring(SpringBase* spring) {
		springs.erase(spring);
	}

	std::string SpringHolder::DebugName()  {
		return "::SpringHolder";
	}

	void SpringHolder::Update() {
		float dt = Time::WorldTimeDelta();
		for (auto spring: springs) {
			spring->Update(dt);
		}
	}
}
