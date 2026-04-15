#pragma once

constexpr int MAX_CBUFFERS = 2;

constexpr int MAX_RENDER_ITEMS = 2048;
//Minus some because of the default Textures like Splash screen
constexpr int MAX_TEXTURES = 512;
constexpr int MAX_LIGHTS = 64;
constexpr int MAX_MATERIALS = 128;

constexpr int PARTICLE_UAV_SLOT = MAX_TEXTURES;
constexpr int PARTICLE_SRV_SLOT = MAX_TEXTURES + 1;
constexpr int PARTICLE_FREELIST_SLOT = MAX_TEXTURES + 2;
constexpr int PARTICLE_FREECOUNT_SLOT = MAX_TEXTURES + 3;
constexpr int PARTICLE_SPAWN_SLOT = MAX_TEXTURES + 4;

constexpr int MAX_PARTICLES = 65536;
constexpr int MAX_PARTICLES_SPAWNS_PER_FRAME = 4096;

constexpr int MAX_DESCRIPTORS = MAX_CBUFFERS + MAX_TEXTURES + 5;

enum DefaultTextures : size_t
{
	_FontPusab = MAX_TEXTURES - 1,
	_SplashScreen = MAX_TEXTURES - 2,
};
