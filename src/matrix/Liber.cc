/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file matrix/Library.cc
 *
 * Map of all resources, object types, scripts etc.
 */

#include <matrix/Liber.hh>

#include <matrix/Vehicle.hh>

#define OZ_REGISTER_BASECLASS(name) \
  baseClasses.include(#name, name##Class::createClass)

namespace oz
{

static HashMap<String, BSP>                      bsps;
static HashMap<String, ObjectClass::CreateFunc*> baseClasses;
static HashMap<String, ObjectClass*>             objClasses;
static HashMap<String, FragPool>                 fragPools;

static HashMap<String, int>                      shaderIndices;
static HashMap<String, int>                      textureIndices;
static HashMap<String, int>                      soundIndices;
static HashMap<String, int>                      caelumIndices;
static HashMap<String, int>                      terraIndices;
static HashMap<String, int>                      partIndices;
static HashMap<String, int>                      modelIndices;

static HashMap<String, int>                      mindIndices;
static HashMap<String, int>                      musicTrackIndices;

static HashMap<String, int>                      deviceIndices;
static HashMap<String, int>                      imagoIndices;
static HashMap<String, int>                      audioIndices;

const BSP* Liber::bsp(const char* name) const
{
  const BSP* value = bsps.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid BSP requested '%s'", name);
  }
  return value;
}

const ObjectClass* Liber::objClass(const char* name) const
{
  const ObjectClass* const* value = objClasses.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid object class requested '%s'", name);
  }
  return *value;
}

const FragPool* Liber::fragPool(const char* name) const
{
  const FragPool* value = fragPools.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid fragment pool requested '%s'", name);
  }
  return value;
}

int Liber::shaderIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  const int* value = shaderIndices.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid shader requested '%s'", name);
  }
  return *value;
}

int Liber::textureIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  const int* value = textureIndices.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid texture requested '%s'", name);
  }
  return *value;
}

int Liber::soundIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  const int* value = soundIndices.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid sound requested '%s'", name);
  }
  return *value;
}

int Liber::caelumIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  const int* value = caelumIndices.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid caelum index requested '%s'", name);
  }
  return *value;
}

int Liber::terraIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  const int* value = terraIndices.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid terra index requested '%s'", name);
  }
  return *value;
}

int Liber::partIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  const int* value = partIndices.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid particle index requested '%s'", name);
  }
  return *value;
}

int Liber::modelIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  const int* value = modelIndices.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid model index requested '%s'", name);
  }
  return *value;
}

int Liber::mindIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  const int* value = mindIndices.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid mind index requested '%s'", name);
  }
  return *value;
}

int Liber::musicTrackIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  const int* value = musicTrackIndices.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid music track index requested '%s'", name);
  }
  return *value;
}

int Liber::deviceIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  const int* value = deviceIndices.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid device index requested '%s'", name);
  }
  return *value;
}

int Liber::imagoIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  const int* value = imagoIndices.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid imago index requested '%s'", name);
  }
  return *value;
}

int Liber::audioIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  const int* value = audioIndices.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid audio index requested '%s'", name);
  }
  return *value;
}

void Liber::freeBSPs()
{
  for (auto& bsp : bsps) {
    if (bsp.value.nUsers != 0) {
      bsp.value.unload();
      bsp.value.nUsers = 0;
    }
  }
}

void Liber::initShaders()
{
  Log::println("Shader programs (*.json in 'glsl') {");
  Log::indent();

  File dir = "@glsl";

  for (const File& file : dir.ls()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    String name = file.baseName();

    Log::println("%s", name.c());

    shaderIndices.add(name, shaders.length());
    shaders.add(Resource{ name, "" });
  }

  shaderIndices.trim();
  shaders.trim();

  Log::unindent();
  Log::println("}");
}

void Liber::initTextures()
{
  Log::println("Textures (*.dds in 'tex/*') {");
  Log::indent();

  File dir = "@tex";

  for (const File& subDir : dir.ls()) {
    if (subDir.type() != File::DIRECTORY) {
      continue;
    }

    for (const File& file : subDir.ls()) {
      if (!file.hasExtension("dds") ||
          file.path().endsWith("_m.dds") || file.path().endsWith("_n.dds"))
      {
        continue;
      }

      String name = subDir.name() + "/" + file.baseName();

      Log::println("%s", name.c());

      textureIndices.add(name, textures.length());
      textures.add(Resource{ name, "@tex/" + name });
    }
  }

  textureIndices.trim();
  textures.trim();

  Log::unindent();
  Log::println("}");
}

void Liber::initSounds()
{
  Log::println("Sounds (*.wav, *.oga, *.ogg in 'snd') {");
  Log::indent();

  File dir = "@snd";

  for (const File& subDir : dir.ls()) {
    if (subDir.type() != File::DIRECTORY) {
      continue;
    }

    for (const File& file : subDir.ls()) {
      if (!file.hasExtension("wav") && !file.hasExtension("oga") && !file.hasExtension("ogg")) {
        continue;
      }

      String name = subDir.name() + "/" + file.baseName();

      Log::println("%s", name.c());

      soundIndices.add(name, sounds.length());
      sounds.add(Resource{ name, file.path() });
    }
  }

  soundIndices.trim();
  sounds.trim();

  Log::unindent();
  Log::println("}");
}

void Liber::initCaela()
{
  Log::println("Caela (directories in 'caelum') {");
  Log::indent();

  File dir = "@caelum";

  for (const File& file : dir.ls()) {
    if (file.type() != File::DIRECTORY) {
      continue;
    }

    String name = file.baseName();

    Log::println("%s", name.c());

    caelumIndices.add(name, caela.length());
    caela.add(Resource{ name, file.path() });
  }

  caelumIndices.trim();
  caela.trim();

  Log::unindent();
  Log::println("}");
}

void Liber::initTerrae()
{
  Log::println("Terrae (*.ozTerra, *.ozcTerra in 'terra') {");
  Log::indent();

  File dir = "@terra";

  for (const File& file : dir.ls()) {
    if (!file.hasExtension("ozTerra")) {
      continue;
    }

    String name = file.baseName();

    Log::println("%s", name.c());

    terraIndices.add(name, terrae.length());
    terrae.add(Resource{ name, file.path() });
  }

  terraIndices.trim();
  terrae.trim();

  Log::unindent();
  Log::println("}");
}

void Liber::initParticles()
{
  Log::println("Particle classes (*.json in 'part') {");
  Log::indent();

  File dir = "@part";

  for (const File& file : dir.ls()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    String name = file.baseName();

    Log::println("%s", name.c());

    partIndices.add(name, parts.length());
    parts.add(Resource{ name, file.path() });
  }

  partIndices.trim();
  parts.trim();

  Log::unindent();
  Log::println("}");
}

void Liber::initModels()
{
  Log::println("Models (*/data.ozcModel in 'mdl') {");
  Log::indent();

  File dir = "@mdl";

  for (const File& file : dir.ls()) {
    if (file.type() != File::DIRECTORY) {
      continue;
    }

    String name = file.name();
    String path = file.path() + "/data.ozcModel";

    if (File(path).type() != File::REGULAR) {
      OZ_ERROR("Invalid model '%s'", name.c());
    }

    Log::println("%s", name.c());

    if (modelIndices.contains(name)) {
      OZ_ERROR("Duplicated model '%s'", name.c());
    }

    modelIndices.add(name, models.length());
    models.add(Resource{ name, path });
  }

  modelIndices.trim();
  models.trim();

  Log::unindent();
  Log::println("}");
}

void Liber::initFragPools()
{
  Log::println("Fragment pools (*.json in 'frag') {");
  Log::indent();

  File dir = "@frag";

  for (const File& file : dir.ls()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    String name = file.baseName();

    Log::println("%s", name.c());

    Json config;
    if (!config.load(file)) {
      OZ_ERROR("Failed to read '%s'", file.path().c());
    }

    fragPools.add(name, FragPool(config, name, fragPools.length()));

    Log::showVerbose = true;
    config.clear(true);
    Log::showVerbose = false;
  }

  fragPools.trim();
  nFragPools = fragPools.length();

  Log::unindent();
  Log::println("}");
}

void Liber::initClasses()
{
  OZ_REGISTER_BASECLASS(Object);
  OZ_REGISTER_BASECLASS(Dynamic);
  OZ_REGISTER_BASECLASS(Weapon);
  OZ_REGISTER_BASECLASS(Bot);
  OZ_REGISTER_BASECLASS(Vehicle);

  baseClasses.trim();

  Log::println("Object classes (*.json in 'class') {");
  Log::indent();

  File dir = "@class";

  // First we only add class instances, we don't initialise them as each class may have references
  // to other classes that haven't been created yet.
  for (const File& file : dir.ls()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    Json config;
    if (!config.load(file)) {
      OZ_ERROR("Failed to read '%s'", file.path().c());
    }

    String name = file.baseName();
    const String& base = config["base"].get("");

    if (objClasses.contains(name)) {
      OZ_ERROR("Duplicated class '%s'", name.c());
    }

    if (String::isEmpty(base)) {
      OZ_ERROR("%s: 'base' missing in class description", name.c());
    }

    ObjectClass::CreateFunc* const* createFunc = baseClasses.find(base);
    if (createFunc == nullptr) {
      OZ_ERROR("%s: Invalid class base '%s'", name.c(), base.c());
    }

    const String& deviceType = config["deviceType"].get("");
    const String& imagoType  = config["imagoType"].get("");
    const String& audioType  = config["audioType"].get("");

    if (!deviceType.isEmpty()) {
      deviceIndices.include(deviceType, deviceIndices.length());
    }
    if (!imagoType.isEmpty()) {
      imagoIndices.include(imagoType, imagoIndices.length());
    }
    if (!audioType.isEmpty()) {
      audioIndices.include(audioType, audioIndices.length());
    }

    objClasses.add(name, (*createFunc)());
  }

  objClasses.trim();
  deviceIndices.trim();
  imagoIndices.trim();
  audioIndices.trim();

  nDeviceClasses = deviceIndices.length();
  nImagoClasses  = imagoIndices.length();
  nAudioClasses  = audioIndices.length();

  // Initialise all classes.
  for (const auto& classIter : objClasses) {
    const String& name  = classIter.key;
    ObjectClass*  clazz = classIter.value;

    Log::print("%s ...", name.c());

    File file = "@class/" + name + ".json";
    Json config;
    if (!config.load(file)) {
      OZ_ERROR("Failed to read '%s'", file.path().c());
    }

    clazz->init(config, name);

    Log::showVerbose = true;
    config["base"];
    config.clear(true);
    Log::showVerbose = false;

    Log::printEnd(" OK");
  }

  // Sanity checks.
  for (const auto& classIter : objClasses) {
    ObjectClass* objClazz = classIter.value;

    // check that all items are valid
    for (int i = 0; i < objClazz->defaultItems.length(); ++i) {
      const ObjectClass* itemClazz = objClazz->defaultItems[i];

      if ((itemClazz->flags & (Object::DYNAMIC_BIT | Object::ITEM_BIT)) !=
          (Object::DYNAMIC_BIT | Object::ITEM_BIT))
      {
        OZ_ERROR("Invalid item class '%s' in '%s', must be dynamic and have item flag",
                 itemClazz->name.c(), objClazz->name.c());
      }
    }

    // check if weaponItem is a valid weapon for bots
    if (objClazz->flags & Object::BOT_BIT) {
      const BotClass* botClazz = static_cast<const BotClass*>(objClazz);

      if (botClazz->weaponItem >= 0) {
        if (uint(botClazz->weaponItem) >= uint(botClazz->defaultItems.length())) {
          OZ_ERROR("Invalid weaponItem index for '%s'", botClazz->name.c());
        }

        // we already checked it in the previous loop it's non-nullptr and a valid item
        const ObjectClass* itemClazz = botClazz->defaultItems[botClazz->weaponItem];

        if (!(itemClazz->flags & Object::WEAPON_BIT)) {
          OZ_ERROR("Default weapon of '%s' is of a non-weapon class", botClazz->name.c());
        }

        const WeaponClass* weaponClazz = static_cast<const WeaponClass*>(itemClazz);

        if (!botClazz->name.beginsWith(weaponClazz->userBase)) {
          OZ_ERROR("Default weapon of '%s' is not allowed for this bot class",
                   botClazz->name.c());
        }
      }
    }
  }

  Log::unindent();
  Log::println("}");
}

void Liber::initBSPs()
{
  Log::println("BSP structures (*.ozBSP, *.ozcModel in 'bsp') {");
  Log::indent();

  File dir = "@bsp";

  for (const File& file : dir.ls()) {
    if (!file.hasExtension("ozBSP")) {
      continue;
    }

    String name = file.baseName();

    Log::println("%s", name.c());

    bsps.add(name, BSP(name, bsps.length()));
  }

  bsps.trim();
  nBSPs = bsps.length();

  Log::unindent();
  Log::println("}");
}

void Liber::initMusicRecurse(const char* path)
{
  File dir = path;

  for (const File& file : dir.ls()) {
    if (file.type() == File::DIRECTORY) {
      initMusicRecurse(file.path());
    }
    if (file.hasExtension("oga") || file.hasExtension("ogg") ||
        (mapMP3s && file.hasExtension("mp3")) || (mapAACs && file.hasExtension("aac")))
    {
      Log::println("%s", file.path().c());

      musicTracks.add(Resource{ file.baseName(), file.path() });
    }
  }
}

void Liber::initMusic(const char* userMusicPath)
{
  if (userMusicPath == nullptr || String::isEmpty(userMusicPath)) {
    Log::println("Music (*.oga, *.ogg%s%s in 'music') {",
                 mapMP3s ? ", *.mp3" : "", mapAACs ? "*.aac" : "");
  }
  else {
    Log::println("Music (*.oga, *.ogg%s%s in 'music' and '%s') {",
                 mapMP3s ? ", *.mp3" : "", mapAACs ? ", *.aac" : "", userMusicPath);
  }
  Log::indent();

  initMusicRecurse("@music");

  for (int i = 0; i < musicTracks.length(); ++i) {
    musicTrackIndices.add(musicTracks[i].name, i);
  }

  initMusicRecurse("@userMusic");

  musicTrackIndices.trim();
  musicTracks.trim();

  Log::unindent();
  Log::println("}");
}

void Liber::init(const char* userMusicPath)
{
  Log::println("Initialising Library {");
  Log::indent();

  Log::verboseMode = true;

  Log::println("Mapping resources {");
  Log::indent();

  initShaders();
  initTextures();
  initSounds();
  initCaela();
  initTerrae();
  initModels();
  initFragPools();
  initClasses();
  initBSPs();
  initMusic(userMusicPath);

  Log::unindent();
  Log::println("}");

  Log::verboseMode = false;

  Log::println("Summary {");
  Log::indent();

  Log::println("%5d  shaders", shaders.length());
  Log::println("%5d  textures", textures.length());
  Log::println("%5d  sounds", sounds.length());
  Log::println("%5d  caela", caela.length());
  Log::println("%5d  terrae", terrae.length());
  Log::println("%5d  BSPs", nBSPs);
  Log::println("%5d  models", models.length());
  Log::println("%5d  music tracks", musicTracks.length());
  Log::println("%5d  fragment pools", fragPools.length());
  Log::println("%5d  object classes", objClasses.length());

  Log::unindent();
  Log::println("}");

  Log::unindent();
  Log::println("}");
}

void Liber::destroy()
{
  shaders.clear();
  shaders.trim();
  textures.clear();
  textures.trim();
  sounds.clear();
  sounds.trim();
  caela.clear();
  caela.trim();
  terrae.clear();
  terrae.trim();
  models.clear();
  models.trim();
  minds.clear();
  minds.trim();
  musicTracks.clear();
  musicTracks.trim();

  shaderIndices.clear();
  shaderIndices.trim();
  textureIndices.clear();
  textureIndices.trim();
  soundIndices.clear();
  soundIndices.trim();
  caelumIndices.clear();
  caelumIndices.trim();
  terraIndices.clear();
  terraIndices.trim();
  modelIndices.clear();
  modelIndices.trim();
  mindIndices.clear();
  mindIndices.trim();
  musicTrackIndices.clear();
  musicTrackIndices.trim();

  deviceIndices.clear();
  deviceIndices.trim();
  imagoIndices.clear();
  imagoIndices.trim();
  audioIndices.clear();
  audioIndices.trim();

  bsps.clear();
  bsps.trim();
  baseClasses.clear();
  baseClasses.trim();
  objClasses.free();
  objClasses.trim();
  fragPools.clear();
  fragPools.trim();
}

Liber liber;

}
