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

#include <builder/Builder.hh>

#include <builder/Context.hh>
#include <builder/Compiler.hh>
#include <builder/UI.hh>
#include <builder/Caelum.hh>
#include <builder/Terra.hh>
#include <builder/BSP.hh>
#include <builder/Class.hh>
#include <builder/AssImp.hh>
#include <builder/MD2.hh>
#include <builder/MD3.hh>

#include <unistd.h>

namespace oz
{
namespace builder
{

static bool skipReferences = false;

void Builder::printUsage()
{
  Log::printRaw(
    "Usage: ozBuild [OPTIONS] <src_dir> [<out_dir>]\n"
    "  <src_dir>  Path to directory that includes data to be built. Name of this\n"
    "             directory name is used as package name.\n"
    "  <out_dir>  Directory where output directory and archive will be created.\n"
    "             Defaults to './share/openzone/<pkg_name>'.\n"
    "  -v         More verbose log output.\n"
    "  -l         Build translations.\n"
    "  -u         Build UI.\n"
    "  -g         Copy shaders.\n"
    "  -c         Build caela (skies).\n"
    "  -t         Build terrae (terrains).\n"
    "  -b         Compile maps into BSPs.\n"
    "  -e         Build textures referenced by terrae and BSPs.\n"
    "  -a         Scan and copy object class definitions.\n"
    "  -f         Scan and copy fragment pool definitions.\n"
    "  -p         Scan and copy particle class definitions.\n"
    "  -m         Build models.\n"
    "  -s         Copy referenced sounds (by UI, BSPs and object classes).\n"
    "  -x         Check and copy Lua scripts.\n"
    "  -k         Copy tech graph files.\n"
    "  -r         Copy music tracks.\n"
    "  -i         Build missions.\n"
    "  -A         Everything above.\n"
    "  -R         Allow missing model texture and sound references.\n"
    "  -C         Use S3 texture compression.\n"
    "  -Z         Compress created ZIP archive (highest compression level).\n"
    "  -7         Create non-solid LZMA-compressed 7zip archive.\n\n");
}

void Builder::copyFiles(const File& srcDir, const File& destDir, const char* ext, bool recurse)
{
  List<File> dirList = srcDir.list();

  if (dirList.isEmpty()) {
    return;
  }

  Log::println("Copying '%s*.%s' -> '%s' {", srcDir.c(), ext, destDir.c());
  Log::indent();

  destDir.mkdir();

  for (const File& file : dirList) {
    String fileName = file.name();

    if (file.isDirectory()) {
      if (recurse) {
        copyFiles(srcDir / file.name(), destDir / file.name(), ext, true);
      }
    }
    else if (file.hasExtension(ext) || fileName.beginsWith("README") ||
             fileName.beginsWith("COPYING"))
    {
      Log::print("Copying '%s' -> '%s' ...", file.c(), destDir.c());

      if (!file.copyTo(destDir)) {
        OZ_ERROR("Failed to copy '%s' -> '%s'", file.c(), destDir.c());
      }

      Log::printEnd(" OK");
      continue;
    }
  }

  Log::unindent();
  Log::println("}");
}

void Builder::buildCaela()
{
  Log::println("Building Caela {");
  Log::indent();

  for (const File& file : File("@caelum").list()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    String name = file.baseName();

    File("caelum").mkdir();
    caelum.build(name);
  }

  Log::unindent();
  Log::println("}");
}

void Builder::buildTerrae()
{
  Log::println("Building Terrae {");
  Log::indent();

  for (const File& file : File("@terra").list()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    File("terra").mkdir();
    terra.build(file.baseName());
  }

  Log::unindent();
  Log::println("}");
}

void Builder::buildBSPs()
{
  Log::println("Building BSPs {");
  Log::indent();

  for (const File& file : File("@baseq3/maps").list()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    File("bsp").mkdir();
    bsp.build(file.baseName());
  }

  Log::unindent();
  Log::println("}");
}

void Builder::buildBSPTextures()
{
  if (context.usedTextures.isEmpty()) {
    return;
  }

  Log::println("Building used BSP textures {");
  Log::indent();

  Set<File> usedDirs;

  for (const File& subDir : File("@baseq3/textures").list()) {
    if (!subDir.isDirectory()) {
      continue;
    }

    for (const File& file : subDir.list()) {
      File   path = file.stripExtension();
      String name = path.substring(17); // strlen("@baseq3/textures/") == 17

      // Skip "*_m.*", "*_n.*" etc. as those get build together with diffuse texture.
      if (!context.isBaseTexture(name)) {
        continue;
      }
      if (!context.usedTextures.contains(name)) {
        continue;
      }

      usedDirs.include(subDir);

      File outDir = "tex";

      outDir.mkdir();
      (outDir / subDir.name()).mkdir();

      context.buildTexture(path, outDir / name);
      context.usedTextures.exclude(name);
    }
  }

  for (const File& subDir : usedDirs) {
    for (const File& file : subDir.list()) {
      if (!file.isFile()) {
        continue;
      }

      String name = file.name();

      if (name.beginsWith("COPYING") || name.beginsWith("README")) {
        Log::print("Copying '%s' ...", file.c());

        File destFile = String::format("tex/%s/%s", subDir.name().c(), name.c());
        if (!file.copyTo(destFile)) {
          OZ_ERROR("Failed to copy '%s' -> '%s'", file.c(), destFile.c());
        }

        Log::printEnd(" OK");
        continue;
      }
    }
  }

  if (!skipReferences && !context.usedTextures.isEmpty()) {
    Log::println("*** The following referenced textures are missing in 'baseq3/textures' *** {");
    Log::indent();

    for (const auto& tex : context.usedTextures) {
      Log::println("'%s' referenced by %s", tex.key.c(), tex.value.c());
    }

    Log::unindent();
    Log::println("}");
  }

  Log::unindent();
  Log::println("}");
}

void Builder::buildClasses()
{
  copyFiles("@class", "class", "json", false);

  Log::println("Scanning object classes {");
  Log::indent();

  String dirName = "@class";
  File dir = dirName;

  for (const File& file : dir.list()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    String name = file.baseName();

    Log::print("%s ...", name.c());

    clazz.scanObjClass(name);

    Log::printEnd(" OK");
  }

  Log::unindent();
  Log::println("}");
}

void Builder::buildFragPools()
{
  copyFiles("@frag", "frag", "json", false);

  Log::println("Scanning fragment pools {");
  Log::indent();

  String dirName = "@frag";
  File dir = dirName;

  for (const File& file : dir.list()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    String name = file.baseName();

    Log::print("%s ...", name.c());

    clazz.scanFragPool(name);

    Log::printEnd(" OK");
  }

  Log::unindent();
  Log::println("}");
}

void Builder::buildModels()
{
  if (context.usedModels.isEmpty()) {
    return;
  }

  Log::println("Building used models {");
  Log::indent();

  for (const File& dir : File("@mdl").list()) {
    if (!context.usedModels.exclude(dir.name())) {
      continue;
    }

    File("mdl").mkdir();
    dir.toNative().mkdir();

    for (const File& file : dir.list()) {
      if (!file.isFile()) {
        continue;
      }

      String name = file.name();

      if (name.beginsWith("COPYING") || name.beginsWith("README")) {
        Log::print("Copying '%s' ...", file.c());

        File destFile = file.toNative();
        if (!file.copyTo(destFile)) {
          OZ_ERROR("Failed to write '%s' -> '%s'", file.c(), destFile.c());
        }

        Log::printEnd(" OK");
        continue;
      }
    }

    File daeFile = dir / "data.dae";
    File objFile = dir / "data.obj";
    File md2File = dir / "tris.md2";

    if (daeFile.isFile() || objFile.isFile()) {
      assImp.build(dir);
    }
    else if (md2File.isFile()) {
      md2.build(dir);
    }
    else {
      md3.build(dir);
    }
  }

  context.usedModels.exclude("");

  if (!skipReferences && !context.usedModels.isEmpty()) {
    Log::println("*** The following referenced models are missing in 'mdl' *** {");
    Log::indent();

    for (const auto& mdl : context.usedModels) {
      Log::println("'%s' referenced by %s", mdl.key.c(), mdl.value.c());
    }

    Log::unindent();
    Log::println("}");
  }

  Log::unindent();
  Log::println("}");
}

void Builder::copySounds()
{
  if (context.usedSounds.isEmpty()) {
    return;
  }

  Log::println("Copying used sounds {");
  Log::indent();

  Set<File> usedDirs;

  for (const File& subDir : File("@snd").list()) {
    if (!subDir.isDirectory()) {
      continue;
    }

    for (const File& file : subDir.list()) {
      if (!file.isFile() ||
          (!file.hasExtension("wav") && !file.hasExtension("oga") &&
           !file.hasExtension("ogg")))
      {
        continue;
      }

      String name = file.stripExtension().substring(5); // strlen("@snd/") == 5

      if (!context.usedSounds.exclude(name)) {
        continue;
      }

      Log::print("Copying '%s' ...", name.c());

      usedDirs.include(subDir);

      File("snd").mkdir();
      subDir.toNative().mkdir();

      File destFile = file.toNative();

      if (!file.copyTo(destFile)) {
        OZ_ERROR("Failed to copy '%s' -> '%s'", file.c(), destFile.c());
      }

      Log::printEnd(" OK");
    }
  }

  for (const File& subDir : usedDirs) {
    for (const File& file : subDir.list()) {
      if (!file.isFile()) {
        continue;
      }

      String name = file.name();

      if (name.beginsWith("COPYING") || name.beginsWith("README")) {
        Log::print("Copying '%s' ...", file.c());

        File("snd").mkdir();
        subDir.toNative().mkdir();

        File destFile = file.toNative();

        if (!file.copyTo(destFile)) {
          OZ_ERROR("Failed to write '%s'", destFile.c());
        }

        Log::printEnd(" OK");
        continue;
      }
    }
  }

  context.usedSounds.exclude("");

  if (!skipReferences && !context.usedSounds.isEmpty()) {
    Log::println("*** The following referenced sounds are missing in 'snd' *** {");
    Log::indent();

    for (const auto& snd : context.usedSounds) {
      Log::println("'%s' referenced by %s", snd.key.c(), snd.value.c());
    }

    Log::unindent();
    Log::println("}");
  }

  Log::unindent();
  Log::println("}");
}

void Builder::checkLua(const File& dir)
{
  Log::println("Checking Lua scripts in '%s' {", dir.c());
  Log::indent();

  for (const File& file : dir.list()) {
    if (!file.hasExtension("lua")) {
      continue;
    }

    String cmdLine = "luac -p " + file.realPath();

    Log::println("%s", cmdLine.c());
    if (system(cmdLine) != 0) {
      OZ_ERROR("Lua syntax check failed");
    }
  }

  Log::unindent();
  Log::println("}");
}

void Builder::buildMissions()
{
  Log::println("Building missions {");
  Log::indent();

  List<File> missions = File("@mission").list();

  if (!missions.isEmpty()) {
    File("mission").mkdir();
  }

  for (const File& mission : missions) {
    checkLua(mission);

    copyFiles(mission, mission.toNative(), "lua", false);
    copyFiles(mission, mission.toNative(), "json", false);

    File srcFile = mission / "description.png";
    if (!srcFile.exists()) {
      continue;
    }

    Log::print("Building thumbnail '%s' ...", srcFile.c());

    ImageBuilder::options = 0;
    ImageBuilder::scale   = 1.0f;

    if (!ImageBuilder::convertToDDS(srcFile, mission.toNative())) {
      OZ_ERROR("Failed to convert '%s' to DDS", srcFile.c());
    }

    Log::printEnd(" OK");
  }

  copyFiles("@mission", "mission", "mo", true);

  Log::unindent();
  Log::println("}");
}

void Builder::packArchive(const char* name, bool useCompression, bool use7zip)
{
  Log::println("Packing archive {");
  Log::indent();

  File archive = String::format("../%s.%s", name, use7zip ? "7z" : "zip");

  String cmdLine = use7zip ? String::format("7z u -ms=off -mx=9 '%s' *", archive.c()) :
                   String::format("zip -ur %s '%s' *", useCompression ? "-9" : "-0", archive.c());

  Log::println("%s", cmdLine.c());
  Log::println();

  if (system(cmdLine) != 0) {
    OZ_ERROR(use7zip ? "Packing 7zip archive failed" : "Packing ZIP archive failed");
  }

  int size = archive.length();
  if (size >= 0) {
    Log::println();
    Log::println("Archive size: %.2f MiB = %.2f MB",
                 float(size) / (1024.0f * 1024.0f),
                 float(size) / (1000.0f * 1000.0f));
  }

  Log::unindent();
  Log::println("}");
}

int Builder::main(int argc, char** argv)
{
  config = Json::OBJECT;

  bool doCat          = false;
  bool doUI           = false;
  bool doShaders      = false;
  bool doCaela        = false;
  bool doTerrae       = false;
  bool doBSPs         = false;
  bool doTextures     = false;
  bool doClasses      = false;
  bool doFrags        = false;
  bool doParticles    = false;
  bool doModels       = false;
  bool doSounds       = false;
  bool doLua          = false;
  bool doTech         = false;
  bool doMusic        = false;
  bool doMissions     = false;
  bool useCompression = false;
  bool use7zip        = false;

  context.useS3TC     = false;
  context.useFast     = false;

  optind = 1;
  int opt;
  while ((opt = getopt(argc, argv, "lugctbeafpmsxkriARCFZ7h?")) >= 0) {
    switch (opt) {
      case 'l': {
        doCat = true;
        break;
      }
      case 'u': {
        doUI = true;
        break;
      }
      case 'g': {
        doShaders = true;
        break;
      }
      case 'c': {
        doCaela = true;
        break;
      }
      case 't': {
        doTerrae = true;
        break;
      }
      case 'b': {
        doBSPs = true;
        break;
      }
      case 'e': {
        doTextures = true;
        break;
      }
      case 'a': {
        doClasses = true;
        break;
      }
      case 'f': {
        doFrags = true;
        break;
      }
      case 'p': {
        doParticles = true;
        break;
      }
      case 'm': {
        doModels = true;
        break;
      }
      case 's': {
        doSounds = true;
        break;
      }
      case 'x': {
        doLua = true;
        break;
      }
      case 'k': {
        doTech = true;
        break;
      }
      case 'r': {
        doMusic = true;
        break;
      }
      case 'i': {
        doMissions = true;
        break;
      }
      case 'A': {
        doCat       = true;
        doUI        = true;
        doShaders   = true;
        doCaela     = true;
        doTerrae    = true;
        doBSPs      = true;
        doTextures  = true;
        doClasses   = true;
        doFrags     = true;
        doParticles = true;
        doModels    = true;
        doSounds    = true;
        doLua       = true;
        doTech      = true;
        doMusic     = true;
        doMissions  = true;
        break;
      }
      case 'R': {
        skipReferences = true;
        break;
      }
      case 'C': {
        context.useS3TC = true;
        break;
      }
      case 'F': {
        context.useFast = true;
        break;
      }
      case 'Z': {
        useCompression = true;
        break;
      }
      case '7': {
        use7zip = true;
        break;
      }
      default: {
        printUsage();
        return EXIT_FAILURE;
      }
    }
  }

  if (optind != argc - 2 && optind != argc - 1) {
    printUsage();
    return EXIT_FAILURE;
  }

  File::init();
  ImageBuilder::init();

  bool hasOutDir = optind != argc - 1;

#ifdef _WIN32
  File srcDir = String::replace(argv[optind], '\\', '/');
  File outDir = hasOutDir ? String::replace(argv[optind + 1], '\\', '/') : "share/openzone";
#else
  File srcDir = argv[optind];
  File outDir = hasOutDir ? argv[optind + 1] : "share/openzone";
#endif

  while (srcDir.last() == '/') {
    srcDir = srcDir.substring(0, srcDir.length() - 1);
  }
  if (srcDir.isEmpty() || srcDir.isRoot()) {
    OZ_ERROR("Source directory cannot be root ('/')");
  }

  String pkgName = srcDir.substring(srcDir.lastIndex('/') + 1);

  if (srcDir[0] != '/') {
    srcDir = File::cwd() / srcDir;
  }
  if (outDir[0] != '/') {
    outDir = File::cwd() / outDir;
  }

  outDir.mkdir();

  if (!hasOutDir) {
    outDir /= pkgName;
    outDir.mkdir();
  }

  Log::println("Chdir to output directory '%s'", outDir.c());
  if (!outDir.chdir()) {
    OZ_ERROR("Failed to set working directory '%s'", outDir.c());
  }

  Log::println("Adding source directory '%s' to search path", srcDir.c());
  if (!srcDir.mountAt(nullptr, true)) {
    OZ_ERROR("Failed to add directory '%s' to search path", srcDir.c());
  }

  config.add("window.width", 400);
  config.add("window.height", 40);
  config.add("window.fullscreen", false);

  context.init();
  compiler.init();

  uint startTime = Time::clock();

  // copy package README/COPYING and credits
  copyFiles("@", ".", "txt", false);
  copyFiles("@credits", "credits", "txt", false);

  if (doCat) {
    copyFiles("@lingua", "lingua", "mo", true);
    copyFiles("@lingua", "lingua", "json", true);
  }
  if (doUI) {
    UI::buildIcons();

    copyFiles("@ui/cur", "ui/cur", "", false);
    copyFiles("@ui/font", "ui/font", "ttf", false);
    copyFiles("@ui/icon", "ui/icon", "", false);
    copyFiles("@ui", "ui", "json", false);
  }
  if (doShaders) {
    copyFiles("@glsl", "glsl", "glsl", false);
    copyFiles("@glsl", "glsl", "vert", false);
    copyFiles("@glsl", "glsl", "frag", false);
    copyFiles("@glsl", "glsl", "json", false);
    copyFiles("@glsl", "glsl", "dds", false);
  }
  if (doCaela) {
    buildCaela();
  }
  if (doTerrae) {
    buildTerrae();
  }
  if (doBSPs) {
    buildBSPs();
  }
  if (doTextures) {
    buildBSPTextures();
  }
  if (doClasses) {
    buildClasses();
  }
  if (doFrags) {
    buildFragPools();
  }
  if (doParticles) {
    copyFiles("@part", "part", "json", false);
  }
  if (doModels) {
    buildModels();
  }
  if (doSounds) {
    copySounds();
  }
  if (doLua) {
    checkLua("@lua/common");
    checkLua("@lua/matrix");
    checkLua("@lua/nirvana");

    copyFiles("@lua", "lua", "lua", true);
  }
  if (doTech) {
    copyFiles("@tech", "tech", "json", true);
  }
  if (doMusic) {
    copyFiles("@music", "music", "oga", true);
    copyFiles("@music", "music", "ogg", true);
  }
  if (doMissions) {
    buildMissions();
  }

  packArchive(pkgName, useCompression, use7zip);

  uint endTime = Time::clock();
  Log::println("Build time: %.2f s", float(endTime - startTime) / 1000.0f);

  compiler.destroy();
  context.destroy();
  config.clear();

  ImageBuilder::destroy();
  File::destroy();

  Log::printProfilerStatistics();
  Profiler::clear();

  Log::printMemorySummary();

  return EXIT_SUCCESS;
}

Builder builder;

}
}
