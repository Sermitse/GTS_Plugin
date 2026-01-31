#pragma once

namespace GTS {
	void SpawnHearts(Actor* giant, Actor* tiny, float Z, float scale, bool hugs, NiPoint3 CustomPos = {});
	void SpawnCustomParticle(Actor* actor, ParticleType Type, NiPoint3 spawn_at_point, std::string_view spawn_at_node, float scale_mult);
	void DoDustExplosion(Actor* giant, float modifier, FootEvent kind, std::string_view node);
	void SpawnParticle(Actor* actor, float lifetime, const char* modelName, const NiMatrix3& rotation, const NiPoint3& position, float scale, std::uint32_t flags, NiAVObject* target);
	void SpawnDustParticle(Actor* giant, Actor* tiny, std::string_view node, float size);
	void ShrinkOutburstExplosion(Actor* giant, bool WasHit);
}