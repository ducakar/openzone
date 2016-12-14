/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include <matrix/Liber.hh>

#include <matrix/Vehicle.hh>

#define OZ_REGISTER_BASECLASS(name) \
  baseClasses.include(#name, name##Class::createClass)

namespace oz
{

static HashMap<String, ObjectClass::CreateFunc*> baseClasses;

static HashMap<String, int>                      shaderIndices;
static HashMap<String, int>                      textureIndices;
static HashMap<String, int>                      soundIndices;
static HashMap<String, int>                      musicTrackIndices;
static HashMap<String, int>                      caelumIndices;
static HashMap<String, int>                      terraIndices;
static HashMap<String, int>                      partIndices;
static HashMap<String, int>                      modelIndices;

static HashMap<String, BSP>                      bspMap;
static HashMap<String, ObjectClass*>             objClassMap;
static HashMap<String, FragPool>                 fragPoolMap;

static HashMap<String, int>                      mindIndices;

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
    OZ_ERROR("Invalid caelum requested '%s'", name);
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
    OZ_ERROR("Invalid terra requested '%s'", name);
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
    OZ_ERROR("Invalid particle requested '%s'", name);
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
    OZ_ERROR("Invalid model requested '%s'", name);
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
    OZ_ERROR("Invalid music track requested '%s'", name);
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
    OZ_ERROR("Invalid mind requested '%s'", name);
  }
  return *value;
}

const FragPool* Liber::fragPool(const char* name) const
{
  if (String::isEmpty(name)) {
    return nullptr;
  }

  const FragPool* value = fragPoolMap.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid fragment pool requested '%s'", name);
  }
  return value;
}

const ObjectClass* Liber::objClass(const char* name) const
{
  if (String::isEmpty(name)) {
    return nullptr;
  }

  const ObjectClass* const* value = objClassMap.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid object class requested '%s'", name);
  }
  return *value;
}

const BSP* Liber::bsp(const char* name) const
{
  if (String::isEmpty(name)) {
    return nullptr;
  }

  const BSP* value = bspMap.find(name);

  if (value == nullptr) {
    OZ_ERROR("Invalid BSP index requested '%s'", name);
  }
  return value;
}

int Liber::deviceIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  return devices.index(name);
}

int Liber::imagoIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  return imagines.index(name);
}

int Liber::audioIndex(const char* name) const
{
  if (String::isEmpty(name)) {
    return -1;
  }

  return audios.index(name);
}

void Liber::initShaders()
{
  Log::println("Shader programs (*.json in 'glsl') {");
  Log::indent();

  File dir = "@glsl";

  for (const File& file : dir.list()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    String name = file.baseName();

    Log::println("%s", name.c());

    shaderIndices.add(name, shaders.length());
    shaders.add(Resource{name, file});
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

  for (const File& subDir : dir.list()) {
    if (!subDir.isDirectory()) {
      continue;
    }

    for (const File& file : subDir.list()) {
      if (!file.hasExtension("dds") || file.endsWith("_m.dds") || file.endsWith("_n.dds")) {
        continue;
      }

      String name = subDir.name() + "/" + file.baseName();

      Log::println("%s", name.c());

      textureIndices.add(name, textures.length());
      textures.add(Resource{name, "@tex/" + name});
    }
  }

  textureIndices.trim();
  textures.trim();

  Log::unindent();
  Log::println("}");
}

void Liber::initSounds()
{
  Log::println("Sounds (*.wav, *.opus, *.oga, *.ogg in 'snd') {");
  Log::indent();

  File dir = "@snd";

  for (const File& subDir : dir.list()) {
    if (!subDir.isDirectory()) {
      continue;
    }

    for (const File& file : subDir.list()) {
      if (!file.hasExtension("wav") && !file.hasExtension("opus") &&
          !file.hasExtension("oga") && !file.hasExtension("ogg"))
      {
        continue;
      }

      String name = subDir.name() + "/" + file.baseName();

      Log::println("%s", name.c());

      if (soundIndices.contains(name)) {
        OZ_ERROR("Duplicated sound '%s'", name.c());
      }

      soundIndices.add(name, sounds.length());
      sounds.add(Resource{name, file});
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

  for (const File& subDir : dir.list()) {
    if (!subDir.isDirectory()) {
      continue;
    }

    String name = subDir.baseName();

    Log::println("%s", name.c());

    caelumIndices.add(name, caela.length());
    caela.add(Resource{name, subDir});
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

  for (const File& file : dir.list()) {
    if (!file.hasExtension("ozTerra")) {
      continue;
    }

    String name = file.baseName();

    Log::println("%s", name.c());

    terraIndices.add(name, terrae.length());
    terrae.add(Resource{name, file});
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

  for (const File& file : dir.list()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    String name = file.baseName();

    Log::println("%s", name.c());

    partIndices.add(name, parts.length());
    parts.add(Resource{name, file});
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

  for (const File& subDir : dir.list()) {
    if (!subDir.isDirectory()) {
      continue;
    }

    String name = subDir.name();
    File   file = subDir / "data.ozcModel";

    if (!file.isFile()) {
      OZ_ERROR("Invalid model '%s'", name.c());
    }

    Log::println("%s", name.c());

    if (modelIndices.contains(name)) {
      OZ_ERROR("Duplicated model '%s'", name.c());
    }

    modelIndices.add(name, models.length());
    models.add(Resource{name, file});
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

  for (const File& file : dir.list()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    String name = file.baseName();

    Log::println("%s", name.c());

    Json config;
    if (!config.load(file)) {
      OZ_ERROR("Failed to read '%s'", file.c());
    }

    FragPool& pool = fragPoolMap.add(name, FragPool(config, name, fragPools.length())).value;
    fragPools.add(&pool);

    Log::showVerbose = true;
    config.clear(true);
    Log::showVerbose = false;
  }

  fragPoolMap.trim();
  fragPools.trim();

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
  for (const File& file : dir.list()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    Json config;
    if (!config.load(file)) {
      OZ_ERROR("Failed to read '%s'", file.c());
    }

    String name = file.baseName();
    String base = config["base"].get("");

    if (objClassMap.contains(name)) {
      OZ_ERROR("Duplicated class '%s'", name.c());
    }

    if (String::isEmpty(base)) {
      OZ_ERROR("%s: 'base' missing in class description", name.c());
    }

    ObjectClass::CreateFunc* const* createFunc = baseClasses.find(base);
    if (createFunc == nullptr) {
      OZ_ERROR("%s: Invalid class base '%s'", name.c(), base.c());
    }

    String deviceType = config["deviceType"].get("");
    String imagoType  = config["imagoType"].get("");
    String audioType  = config["audioType"].get("");

    if (!deviceType.isEmpty()) {
      devices.include(deviceType);
    }
    if (!imagoType.isEmpty()) {
      imagines.include(imagoType);
    }
    if (!audioType.isEmpty()) {
      audios.include(audioType);
    }

    ObjectClass* clazz = (*createFunc)();

    objClassMap.add(name, clazz);
    objClasses.add(clazz);
  }

  objClassMap.trim();
  objClasses.trim();
  devices.trim();
  imagines.trim();
  audios.trim();

  // Initialise all classes.
  for (const auto& classIter : objClassMap) {
    const String& name  = classIter.key;
    ObjectClass*  clazz = classIter.value;

    Log::print("%s ...", name.c());

    File file = "@class/" + name + ".json";
    Json config;
    if (!config.load(file)) {
      OZ_ERROR("Failed to read '%s'", file.c());
    }

    clazz->init(config, name);

    Log::showVerbose = true;
    config["base"];
    config.clear(true);
    Log::showVerbose = false;

    Log::printEnd(" OK");
  }

  // Sanity checks.
  for (const auto& classIter : objClassMap) {
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

      if (botClazz->weaponItem != -1) {
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

  for (const File& file : dir.list()) {
    if (!file.hasExtension("ozBSP")) {
      continue;
    }

    String name = file.baseName();

    Log::println("%s", name.c());

    BSP& bsp = bspMap.add(name, BSP(name, bsps.length())).value;
    bsps.add(&bsp);

    bsp.load();
  }

  bspMap.trim();
  bsps.trim();

  Log::unindent();
  Log::println("}");
}

void Liber::initMusicRecurse(const File& dir)
{
  for (const File& file : dir.list()) {
    if (file.isDirectory()) {
      initMusicRecurse(file);
    }
    if (file.hasExtension("opus") || file.hasExtension("oga") || file.hasExtension("ogg")) {
      Log::println("%s", file.c());

      musicTracks.add(Resource{file.baseName(), file});
    }
  }
}

void Liber::initMusic(const char* userMusicPath)
{
  if (userMusicPath == nullptr || String::isEmpty(userMusicPath)) {
    Log::println("Music (*.opus, *.oga, *.ogg in 'music') {");
  }
  else {
    Log::println("Music (*.opus, *.oga, *.ogg in 'music' and '%s') {", userMusicPath);
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
  Log::println("%5d  music tracks", musicTracks.length());
  Log::println("%5d  caela", caela.length());
  Log::println("%5d  terrae", terrae.length());
  Log::println("%5d  models", models.length());
  Log::println("%5d  fragment pools", fragPools.length());
  Log::println("%5d  object classes", objClasses.length());
  Log::println("%5d  BSPs", bsps.length());

  Log::unindent();
  Log::println("}");

  Log::unindent();
  Log::println("}");
}

void Liber::destroy()
{
  shaders.clear();
  shaders.trim();
  shaderIndices.clear();
  shaderIndices.trim();

  textures.clear();
  textures.trim();
  textureIndices.clear();
  textureIndices.trim();

  sounds.clear();
  sounds.trim();
  soundIndices.clear();
  soundIndices.trim();

  caela.clear();
  caela.trim();
  caelumIndices.clear();
  caelumIndices.trim();

  terrae.clear();
  terrae.trim();
  terraIndices.clear();
  terraIndices.trim();

  models.clear();
  models.trim();
  modelIndices.clear();
  modelIndices.trim();

  musicTracks.clear();
  musicTracks.trim();
  musicTrackIndices.clear();
  musicTrackIndices.trim();

  mindIndices.clear();
  mindIndices.trim();

  devices.clear();
  devices.trim();
  imagines.clear();
  imagines.trim();
  audios.clear();
  audios.trim();

  baseClasses.clear();
  baseClasses.trim();

  for (auto& bsp: bspMap) {
    bsp.value.unload();
  }

  bsps.clear();
  bsps.trim();
  bspMap.clear();
  bspMap.trim();

  objClasses.free();
  objClasses.trim();
  objClassMap.clear();
  objClassMap.trim();

  fragPools.clear();
  fragPools.trim();
  fragPoolMap.clear();
  fragPoolMap.trim();
}

Liber liber;

}
