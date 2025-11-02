#pragma once

namespace GTS {

	RE::COL_LAYER GetCollidesWith(const std::uint32_t& collisionFilterInfo);
	RE::COL_LAYER GetCollidesWith(const hkpCollidable* collidable);
	RE::COL_LAYER GetCollidesWith(const hkpWorldObject* entity);

	void SetCollidesWith(std::uint32_t& collisionFilterInfo, const RE::COL_LAYER& newLayer);
	void SetCollidesWith(hkpCollidable* collidable, const RE::COL_LAYER& newLayer);
	void SetCollidesWith(hkpWorldObject* entity, const RE::COL_LAYER& newLayer);

}