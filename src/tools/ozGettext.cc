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

#include <ozCore/ozCore.hh>

#include <cstdlib>
#include <cstdio>

using namespace oz;

enum LuaSyntaxState
{
  NORMAL,
  STRING1,
  STRING2,
  LINE_COMMENT,
  MULTILINE_COMMENT
};

static HashMap<String, String> messages;

static void printUsage()
{
  Log::printRaw(
    "Usage: ozGettext <data_dir>\n"
    "  <data_dir>  Package directory that contains source game data. The output POT\n"
    "              files will be written into <data_dir>/lingua.\n\n");
}

static void readLuaChunk(const char* begin, int size, const char* path)
{
  Stream is(begin, begin + size);

  LuaSyntaxState state          = NORMAL;
  int            lineNum        = 1;
  int            gettextLineNum = 1;
  char           last[4]        = {'\0', '\0', '\0', '\0'};
  List<char>     lastString;
  bool           restartString  = true;
  bool           inGettext      = false;

  while (is.available() != 0) {
    last[3] = last[2];
    last[2] = last[1];
    last[1] = last[0];
    last[0] = is.readChar();

    switch (state) {
      case NORMAL: {
        if (last[0] == '(') {
          if (is.tell() >= 10 && Arrays::equals(is.pos() - 10, 9, "ozGettext")) {
            inGettext = true;
            gettextLineNum = lineNum;
          }
        }
        else if (last[0] == ')') {
          if (inGettext) {
            inGettext = false;

            lastString.add('\0');

            String locationInfo = String::format("%s:%d", path, gettextLineNum);
            String message      = &lastString[0];

            messages.include(message, locationInfo);
          }
        }
        else if (last[1] == '.' && last[0] == '.') {
          restartString = false;
        }
        else if (last[0] == '"' || last[0] == '\'') {
          state = last[0] == '"' ? STRING1 : STRING2;

          if (restartString) {
            lastString.clear();
          }
        }
        else if (last[1] == '-' && last[0] == '-') {
          state = LINE_COMMENT;
        }
        else if (last[3] == '-' && last[2] == '-' && last[1] == '[' && last[0] == '[') {
          state = MULTILINE_COMMENT;
        }
        break;
      }
      case STRING1:
      case STRING2: {
        if (last[1] == '\\') {
          if (last[0] == 'n') {
            lastString.add('\n');
          }
          else if (last[0] == 't') {
            lastString.add('\t');
          }
          else {
            lastString.add(last[0]);
          }
        }
        else if ((state == STRING1 && last[0] == '"') ||
                 (state == STRING2 && last[0] == '\''))
        {
          state = NORMAL;
          restartString = true;
        }
        else if (last[0] != '\\') {
          lastString.add(last[0]);
        }
        break;
      }
      case LINE_COMMENT: {
        if (last[0] == '\n') {
          state = NORMAL;
        }
        break;
      }
      case MULTILINE_COMMENT: {
        if (last[3] == ']' && last[2] == ']' && last[1] == '-' && last[0] == '-') {
          state = NORMAL;
        }
        break;
      }
    }

    if (last[0] == '\n') {
      ++lineNum;
    }
  }
}

static void readLua(const File& file)
{
  Stream buffer = file.read().OZ_UNWRAP("Failed to read '%s'", file.c());
  readLuaChunk(buffer.begin(), buffer.available(), file);
}

static void readBSP(const File& file)
{
  Json config;

  if (!config.load(file)) {
    OZ_ERROR("Failed to load '%s'", file.c());
  }

  const char* title       = config["title"].get("");
  const char* description = config["description"].get("");

  if (String::isEmpty(title)) {
    messages.include(file.baseName(), file);
  }
  else {
    messages.include(title, file);
  }
  if (!String::isEmpty(description)) {
    messages.include(description, file);
  }

  const Json& entities = config["entities"];
  int nEntities = entities.size();

  if (!entities.isNull() && entities.type() != Json::ARRAY) {
    OZ_ERROR("'entities' entry in '%s' is not an array", file.c());
  }

  for (int i = 0; i < nEntities; ++i) {
    const char* entityTitle = entities[i]["title"].get("");

    if (!String::isEmpty(entityTitle)) {
      messages.include(entityTitle, file);
    }
  }

  config.clear();
}

static void readClass(const File& file)
{
  Json config;

  if (!config.load(file)) {
    OZ_ERROR("Failed to read '%s'", file.c());
  }

  const char* title       = config["title"].get("");
  const char* description = config["description"].get("");

  if (String::isEmpty(title)) {
    messages.include(file.baseName(), file);
  }
  else {
    messages.include(title, file);
  }
  if (!String::isEmpty(description)) {
    messages.include(description, file);
  }

  const Json& weaponsConfig = config["weapons"];

  for (int i = 0; i < weaponsConfig.size(); ++i) {
    const char* weaponTitle = weaponsConfig[i]["title"].get("");

    if (!String::isEmpty(weaponTitle)) {
      messages.include(weaponTitle, file);
    }
  }
}

static void readNirvana(const File& dir)
{
  File techFile = dir / "techGraph.json";
  Json techConfig;

  if (techConfig.load(techFile)) {
    for (const Json& node : techConfig.arrayCRange()) {
      const char* technology  = node["technology"].get("");
      const char* title       = node["title"].get(technology);
      const char* description = node["description"].get("");

      if (!String::isEmpty(technology)) {
        messages.include(title, techFile);
      }
      if (!String::isEmpty(description)) {
        messages.include(description, techFile);
      }
    }
  }

  File mindsDir = dir / "mind";

  for (const File& file : mindsDir.list()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    Json json(file);

    for (const Json& state : json.arrayCRange()) {
      const String& onEnter  = state["onEnter"].get(String::EMPTY);
      const String& onUpdate = state["onUpdate"].get(String::EMPTY);

      if (!onEnter.isEmpty()) {
        readLuaChunk(onEnter.c(), onEnter.length(), file);
      }
      if (!onUpdate.isEmpty()) {
        readLuaChunk(onUpdate.c(), onUpdate.length(), file);
      }

      for (const Json& link : state["links"].arrayCRange()) {
        const String& condition = link["if"].get(String::EMPTY);

        if (!condition.isEmpty()) {
          readLuaChunk(condition.c(), condition.length(), file);
        }
      }
    }
  }
}

static void readCredits(const File& file)
{
  if (!file.isFile()) {
    return;
  }

  Stream is = file.read().unwrap();
  String contents;

  while (is.available() != 0) {
    contents += is.readLine() + "\n";
  }

  messages.include(contents, file);
}

static void readSequence(const File& file)
{
  Json sequence(file);

  int nSteps = sequence.size();
  for (int i = 0; i < nSteps; ++i) {
    const char* title = sequence[i]["title"].get("");

    if (!String::isEmpty(title)) {
      String locationInfo = String::format("%s:step #%d", file.c(), i + 1);

      messages.include(title, locationInfo);
    }
  }
}

static void readDescription(const File& file)
{
  Json descriptionConfig(file);

  const char* title       = descriptionConfig["title"].get("");
  const char* description = descriptionConfig["description"].get("");

  if (!String::isEmpty(title)) {
    messages.include(title, file);
  }
  if (!String::isEmpty(description)) {
    messages.include(description, file);
  }
}

static void writePOT(const HashMap<String, String>* hs, const File& outFile)
{
  Stream os(0);
  String s;

  bool isFirst = true;
  for (const auto& i : *hs) {
    if (!isFirst) {
      os.writeChar('\n');
    }
    isFirst = false;

    os.writeLine("#: " + i.value);

    // Escape backslashes and quotes.
    s = i.key;
    for (int j = 0; j < s.length(); ++j) {
      if (s[j] == '\\' || s[j] == '"') {
        s = s.substring(0, j) + "\\" + s.substring(j);
        ++j;
      }
    }

    // If multi-line, put each line into a new line in .pot file and escape newlines.
    if (s.index('\n') < 0) {
      os.writeLine(String::format("msgid \"%s\"", s.c()));
    }
    else {
      List<String> stringLines = s.split('\n');

      os.writeLine("msgid \"\"");

      for (const String& l : stringLines) {
        if (&l == &stringLines.last() && l.isEmpty()) {
          break;
        }

        if (&l == &stringLines.last()) {
          s = String::format("\"%s\"", l.c());
        }
        else {
          s = String::format("\"%s\\n\"", l.c());
        }

        os.writeLine(s);
      }
    }

    os.writeLine("msgstr \"\"");
  }

  if (!outFile.write(os)) {
    OZ_ERROR("Failed to write '%s'", outFile.c());
  }
}

int main(int argc, char** argv)
{
  System::init();

  if (argc != 2) {
    printUsage();
    return EXIT_FAILURE;
  }

#ifdef _WIN32
  File pkgDir = String::replace(argv[1], '\\', '/');
#else
  File pkgDir = argv[1];
#endif

  while (pkgDir.last() == '/') {
    pkgDir = pkgDir.substring(0, pkgDir.length() - 1);
  }
  if (pkgDir.isEmpty() || pkgDir.isRoot()) {
    OZ_ERROR("Package directory cannot be root ('/')");
  }

  String pkgName = pkgDir.substring(pkgDir.lastIndex('/') + 1);

  File luaCommonDir  = pkgDir / "lua/common";
  File luaMatrixDir  = pkgDir / "lua/matrix";
  File luaNirvanaDir = pkgDir / "lua/nirvana";
  File luaClientDir  = pkgDir / "lua/client";

  List<File> luaCommonFiles  = luaCommonDir.list();
  List<File> luaMatrixFiles  = luaMatrixDir.list();
  List<File> luaNirvanaFiles = luaNirvanaDir.list();
  List<File> luaClientFiles  = luaClientDir.list();

  List<File> luaFiles;
  luaFiles.addAll(luaCommonFiles.begin(), luaCommonFiles.size());
  luaFiles.addAll(luaMatrixFiles.begin(), luaMatrixFiles.size());
  luaFiles.addAll(luaNirvanaFiles.begin(), luaNirvanaFiles.size());
  luaFiles.addAll(luaClientFiles.begin(), luaClientFiles.size());

  for (const File& file : luaFiles) {
    if (!file.hasExtension("lua")) {
      continue;
    }

    readLua(file);
  }

  File bspDir = pkgDir / "baseq3/maps";

  for (const File& file : bspDir.list()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    readBSP(file);
  }

  File classDir = pkgDir / "class";

  for (const File& file : classDir.list()) {
    if (!file.hasExtension("json")) {
      continue;
    }

    readClass(file);
  }

  File nirvanaDir = pkgDir / "nirvana";
  readNirvana(nirvanaDir);

  File creditsFile = pkgDir / "credits/" + pkgName + ".txt";
  readCredits(creditsFile);

  if (!messages.isEmpty()) {
    File pkgLingua = pkgDir / "lingua";
    File mainPOT   = pkgLingua / pkgName + ".pot";

    Log::print("%s ...", mainPOT.c());

    pkgLingua.mkdir();
    writePOT(&messages, mainPOT);

    Log::printEnd(" OK");
  }

  messages.clear();

  File missionsDir = pkgDir / "mission";

  for (const File& mission : missionsDir.list()) {
    if (!mission.isDirectory()) {
      continue;
    }

    for (const File& file : mission.list()) {
      if (file.hasExtension("lua")) {
        readLua(file);
      }
      else if (file.hasExtension("json")) {
        if (file.endsWith(".sequence.json")) {
          readSequence(file);
        }
        else {
          readDescription(file);
        }
      }
    }

    if (!messages.isEmpty()) {
      File missionLingua = mission / "lingua";
      File missionPOT    = missionLingua / "messages.pot";

      Log::print("%s ...", missionPOT.c());

      missionLingua.mkdir();
      writePOT(&messages, missionPOT);

      Log::printEnd(" OK");
    }

    messages.clear();
  }

  messages.trim();

  return EXIT_SUCCESS;
}
