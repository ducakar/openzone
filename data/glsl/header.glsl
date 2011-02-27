#version 120

uniform int       oz_TextureEnabled;
uniform sampler2D oz_Textures[2];
uniform float     oz_TextureScale;

uniform vec4      oz_DiffuseMaterial;
uniform vec4      oz_SpecularMaterial;

uniform vec3      oz_AmbientLight;
uniform vec3      oz_SkyLight[2];
uniform vec3      oz_PointLights[16];
