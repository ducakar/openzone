/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include <builder/BSP.hh>

#include <builder/Context.hh>

#define OZ_MODEL_FLAG(flagBit, flagName, defValue) \
  model.flags |= flagBit & -entityConfig[flagName].get(defValue)

namespace oz::builder
{

void BSP::load()
{
  File configFile = "@baseq3/maps/" + name + ".json";
  File bspFile    = "@baseq3/maps/" + name + ".bsp";

  Json config;
  if (!config.load(configFile)) {
    OZ_ERROR("BSP config loading failed");
  }

  title = config["title"].get(name);
  description = config["description"].get("");

  float scale = config["scale"].get(DEFAULT_SCALE);

  life = config["life"].get(DEFAULT_LIFE);
  resistance = config["resistance"].get(DEFAULT_RESISTANCE);

  waterFogColour = config["waterFogColour"].get(Vec4(0.00f, 0.05f, 0.20f, 1.00f));
  lavaFogColour  = config["lavaFogColour"].get(Vec4(0.30f, 0.20f, 0.00f, 1.00f));

  if (life <= 0.0f || !Math::isFinite(life)) {
    OZ_ERROR("%s: Invalid life value. Should be > 0 and finite. If you want infinite life rather"
             " set resistance to infinity (\"inf\").", name.c());
  }
  if (resistance < 0.0f) {
    OZ_ERROR("%s: Invalid resistance. Should be >= 0.", name.c());
  }

  fragPool = config["fragPool"].get("");
  nFrags   = config["nFrags"].get(0);

  demolishSound = config["demolishSound"].get("");
  groundOffset  = config["groundOffset"].get(0.0f);

  mins = Point(-Math::INF, -Math::INF, -Math::INF);
  maxs = Point(+Math::INF, +Math::INF, +Math::INF);

  if (Math::isNaN(scale)) {
    OZ_ERROR("Invalid BSP config");
  }

  Stream is(0, Endian::LITTLE);
  if (!bspFile.read(&is)) {
    OZ_ERROR("Cannot read BSP file '%s'", bspFile.c());
  }

  char id[4];
  id[0] = is.readChar();
  id[1] = is.readChar();
  id[2] = is.readChar();
  id[3] = is.readChar();

  int version = is.readInt();

  if (id[0] != 'I' || id[1] != 'B' || id[2] != 'S' || id[3] != 'P' || version != 46) {
    OZ_ERROR("Not a Quake 3 BSP format");
  }

  List<QBSPLump> lumps(QBSPLump::MAX);
  for (int i = 0; i < QBSPLump::MAX; ++i) {
    lumps[i].offset = is.readInt();
    lumps[i].length = is.readInt();
  }

  textures.resize(lumps[QBSPLump::TEXTURES].length / sizeof(QBSPTexture));

  is.rewind();
  is.readSkip(lumps[QBSPLump::TEXTURES].offset);

  for (Texture& texture : textures) {
    texture.name  = is.readSkip(64);
    texture.flags = is.readInt();
    texture.type  = is.readInt();

    if (texture.name == "noshader") {
      texture.name = "";
    }
    else if (texture.name.length() <= 9) {
      OZ_ERROR("Invalid texture name '%s'", texture.name.c());
    }
    else {
      texture.name = texture.name.substring(9);
    }

    if (texture.type & QBSP_SEA_TYPE_BIT) {
      texture.name = "@sea:" + texture.name;
    }

    Log::println("Texture '%s' flags %x type %x",
                 texture.name.c(),
                 texture.flags,
                 texture.type);
  }

  planes.resize(lumps[QBSPLump::PLANES].length / sizeof(QBSPPlane));

  is.rewind();
  is.readSkip(lumps[QBSPLump::PLANES].offset);

  for (Plane& plane : planes) {
    plane.n.x = is.readFloat();
    plane.n.y = is.readFloat();
    plane.n.z = is.readFloat();
    plane.d   = is.readFloat() * scale;
  }

  nodes.resize(lumps[QBSPLump::NODES].length / sizeof(QBSPNode));

  is.rewind();
  is.readSkip(lumps[QBSPLump::NODES].offset);

  for (oz::BSP::Node& node : nodes) {
    node.plane = is.readInt();
    node.front = is.readInt();
    node.back  = is.readInt();

    // int bb[2][3]
    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();
  }

  leaves.resize(lumps[QBSPLump::LEAFS].length / sizeof(QBSPLeaf));
  leafClusters.resize(leaves.size());

  is.rewind();
  is.readSkip(lumps[QBSPLump::LEAFS].offset);

  for (int i = 0; i < leaves.size(); ++i) {
    // int cluster
    leafClusters[i] = is.readInt();
    // int area
    is.readInt();
    // int bb[2][3]
    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();
    // int firstFace
    is.readInt();
    // int nFaces
    is.readInt();

    leaves[i].firstBrush = is.readInt();
    leaves[i].nBrushes   = is.readInt();
  }

  leafBrushes.resize(lumps[QBSPLump::LEAFBRUSHES].length / sizeof(int));

  is.rewind();
  is.readSkip(lumps[QBSPLump::LEAFBRUSHES].offset);

  for (int& leafBrush : leafBrushes) {
    leafBrush = is.readInt();
  }

  int nModels = lumps[QBSPLump::MODELS].length / sizeof(QBSPModel);

  models.resize(nModels - 1);
  modelFaces.resize(nModels);

  is.rewind();
  is.readSkip(lumps[QBSPLump::MODELS].offset);

  const Json& modelsConfig = config["entities"];

  if (!modelsConfig.isNull() && modelsConfig.type() != Json::ARRAY) {
    OZ_ERROR("'entities' entry in '%s' is not an array", configFile.c());
  }

  // skip model 0 (whole BSP)
  is.readSkip(sizeof(QBSPModel));

  for (int i = 0; i < models.size(); ++i) {
    const Json& entityConfig = modelsConfig[i];
    Model&      model        = models[i];

    model.mins.x = is.readFloat() * scale - 2.0f * EPSILON;
    model.mins.y = is.readFloat() * scale - 2.0f * EPSILON;
    model.mins.z = is.readFloat() * scale - 2.0f * EPSILON;

    model.maxs.x = is.readFloat() * scale + 2.0f * EPSILON;
    model.maxs.y = is.readFloat() * scale + 2.0f * EPSILON;
    model.maxs.z = is.readFloat() * scale + 2.0f * EPSILON;

    // int firstFace
    is.readInt();
    // int nFaces
    is.readInt();

    model.firstBrush = is.readInt();
    model.nBrushes   = is.readInt();

    model.title      = entityConfig["title"].get("");

    static const EnumMap<EntityClass::Type> entityMap = {
      {EntityClass::STATIC, "STATIC"},
      {EntityClass::MOVER,  "MOVER" },
      {EntityClass::DOOR,   "DOOR"  },
      {EntityClass::PORTAL, "PORTAL"}
    };

    model.type         = entityMap[entityConfig["type"].get("")];
    model.flags        = 0;

    OZ_MODEL_FLAG(EntityClass::IGNORANT,   "flag.ignorant",  false);
    OZ_MODEL_FLAG(EntityClass::PUSHER,     "flag.pusher",    false);
    OZ_MODEL_FLAG(EntityClass::CRUSHER,    "flag.crusher",   false);
    OZ_MODEL_FLAG(EntityClass::REVERTER,   "flag.reverter",  false);
    OZ_MODEL_FLAG(EntityClass::AUTO_OPEN,  "flag.autoOpen",  false);
    OZ_MODEL_FLAG(EntityClass::AUTO_CLOSE, "flag.autoClose", false);

    model.closeTimeout = entityConfig["closeTimeout"].get(0.0f);
    model.openTimeout  = entityConfig["openTimeout"].get(0.0f);

    Vec3  move         = entityConfig["move"].get(Vec3::ZERO);
    float slideTime    = entityConfig["slideTime"].get(1.0f);

    if (slideTime <= 0.0f) {
      OZ_ERROR("entities[%d].slideTime must be > 0", i);
    }

    model.moveDir      = move.sqN() == 0.0f ? move : ~move;
    model.moveLength   = !move;
    model.moveStep     = model.moveLength / (slideTime * Timer::TICKS_PER_SEC);

    float defaultMargin = model.flags & EntityClass::AUTO_OPEN ? DEFAULT_MARGIN : 0.0f;

    model.margin       = entityConfig["margin"].get(defaultMargin);

    model.openSound    = entityConfig["openSound"].get("");
    model.closeSound   = entityConfig["closeSound"].get("");
    model.frictSound   = entityConfig["frictSound"].get("");

    model.target       = entityConfig["target"].get(-1);
    model.key          = entityConfig["key"].get(0);

    const Json& modelConfig = entityConfig["model"];

    if (!modelConfig.isNull()) {
      model.modelName = modelConfig["name"].get("");
      if (model.modelName.isEmpty()) {
        OZ_ERROR("entities[%d].model.name is empty", i);
      }

      Vec3 translation = modelConfig["translation"].get(Vec3::ZERO);
      Vec3 rotation    = modelConfig["rotation"].get(Vec3::ZERO);

      model.modelTransf = Mat4::translation(models[i].p() + translation - Point::ORIGIN);
      model.modelTransf.rotateY(Math::rad(rotation.y));
      model.modelTransf.rotateX(Math::rad(rotation.x));
      model.modelTransf.rotateZ(Math::rad(rotation.z));
    }
  }

  is.rewind();
  is.readSkip(lumps[QBSPLump::MODELS].offset);

  for (int i = 0; i < models.size() + 1; ++i) {
    // float bb[2][3]
    is.readFloat();
    is.readFloat();
    is.readFloat();
    is.readFloat();
    is.readFloat();
    is.readFloat();

    modelFaces[i].firstFace = is.readInt();
    modelFaces[i].nFaces    = is.readInt();

    // int firstBrush
    is.readInt();
    // int nBrushes
    is.readInt();
  }

  brushSides.resize(lumps[QBSPLump::BRUSHSIDES].length / sizeof(QBSPBrushSide));

  is.rewind();
  is.readSkip(lumps[QBSPLump::BRUSHSIDES].offset);

  for (int& brushSide : brushSides) {
    brushSide = is.readInt();

    // int texture
    is.readInt();
  }

  brushes.resize(lumps[QBSPLump::BRUSHES].length / sizeof(QBSPBrush));

  if (brushes.size() > oz::BSP::MAX_BRUSHES) {
    OZ_ERROR("Too many brushes %d, maximum is %d", brushes.size(), oz::BSP::MAX_BRUSHES);
  }

  is.rewind();
  is.readSkip(lumps[QBSPLump::BRUSHES].offset);

  for (oz::BSP::Brush& brush : brushes) {
    brush.firstSide = is.readInt();
    brush.nSides    = is.readInt();
    brush.flags     = 0;

    int texture = is.readInt();

    if (textures[texture].flags & QBSP_NONSOLID_FLAG_BIT) {
      if (textures[texture].flags & QBSP_LADDER_FLAG_BIT) {
        brush.flags |= Medium::LADDER_BIT;
      }
      else if (textures[texture].type & QBSP_AIR_TYPE_BIT) {
        brush.flags |= Medium::AIR_BIT;
      }
      else if (textures[texture].type & QBSP_WATER_TYPE_BIT) {
        brush.flags |= Medium::WATER_BIT;
      }
      else if (textures[texture].type & QBSP_LAVA_TYPE_BIT) {
        brush.flags |= Medium::LAVA_BIT;
      }
      else if (textures[texture].type & QBSP_SEA_TYPE_BIT) {
        brush.flags |= Medium::SEA_BIT;
      }
    }
    else {
      brush.flags |= Material::STRUCT_BIT;

      if (textures[texture].flags & QBSP_SLICK_FLAG_BIT) {
        brush.flags |= Material::SLICK_BIT;
      }
    }
  }

  vertices.resize(lumps[QBSPLump::VERTICES].length / sizeof(QBSPVertex));

  is.rewind();
  is.readSkip(lumps[QBSPLump::VERTICES].offset);

  for (QBSPVertex& vertex : vertices) {
    vertex.pos[0]      = is.readFloat() * scale;
    vertex.pos[1]      = is.readFloat() * scale;
    vertex.pos[2]      = is.readFloat() * scale;

    vertex.texCoord[0] = is.readFloat();
    vertex.texCoord[1] = is.readFloat();

    // float lightmapCoord[2]
    is.readFloat();
    is.readFloat();

    vertex.normal[0]   = is.readFloat();
    vertex.normal[1]   = is.readFloat();
    vertex.normal[2]   = is.readFloat();

    // char colour[4]
    is.readChar();
    is.readChar();
    is.readChar();
    is.readChar();
  }

  indices.resize(lumps[QBSPLump::INDICES].length / sizeof(int));

  is.rewind();
  is.readSkip(lumps[QBSPLump::INDICES].offset);

  for (int & index : indices) {
    index = is.readInt();
  }

  faces.resize(lumps[QBSPLump::FACES].length / sizeof(QBSPFace));

  is.rewind();
  is.readSkip(lumps[QBSPLump::FACES].offset);

  for (Face& face : faces) {
    face.texture     = is.readInt();

    // int effect
    is.readInt();
    // int type
    is.readInt();

    face.firstVertex = is.readInt();
    face.nVertices   = is.readInt();

    face.firstIndex  = is.readInt();
    face.nIndices    = is.readInt();

    // int lightmap
    is.readInt();
    // int lightmapCorner[2]
    is.readInt();
    is.readInt();
    // int lightmapSize[2]
    is.readInt();
    is.readInt();
    // float lightmapPos[3]
    is.readFloat();
    is.readFloat();
    is.readFloat();
    // float lightmapVecs[2][3]
    is.readFloat();
    is.readFloat();
    is.readFloat();
    is.readFloat();
    is.readFloat();
    is.readFloat();
    // float normal[3]
    is.readFloat();
    is.readFloat();
    is.readFloat();
    // int size[2]
    is.readInt();
    is.readInt();
  }

  if (lumps[QBSPLump::VISUALDATA].length != 0) {
    is.rewind();
    is.readSkip(lumps[QBSPLump::VISUALDATA].offset);

    nClusters     = is.readInt();
    nClusterBytes = is.readInt();

    clusters.resize(nClusters * nClusterBytes * 8);
    is.read(reinterpret_cast<char*>(clusters.begin()), nClusters * nClusterBytes);
  }
  else {
    is.writeInt(0);
    is.writeInt(0);
  }

  const Json& objectsConfig = config["boundObjects"];
  int nBoundObjects = objectsConfig.size();

  for (int i = 0; i < nBoundObjects; ++i) {
    const Json& objectConfig = objectsConfig[i];

    String clazz = objectConfig["class"].get("");

    if (!clazz.isEmpty()) {
      BoundObject object;

      object.clazz = clazz;
      object.pos   = objectConfig["pos"].get(Point::ORIGIN);

      String sHeading = objectConfig["heading"].get("");
      if (sHeading == "NORTH") {
        object.heading = NORTH;
      }
      else if (sHeading == "WEST") {
        object.heading = WEST;
      }
      else if (sHeading == "SOUTH") {
        object.heading = SOUTH;
      }
      else if (sHeading == "EAST") {
        object.heading = EAST;
      }
      else if (sHeading.isEmpty()) {
        OZ_ERROR("Missing heading for a BSP bound object");
      }
      else {
        OZ_ERROR("Invalid object heading '%s'", sHeading.c());
      }

      boundObjects.add(object);
    }
  }

  config.clear(true);
}

void BSP::optimise()
{
  Log::println("Optimising BSP structure {");
  Log::indent();

  // remove brushes that lay out of boundaries
  for (int i = 0; i < brushes.size();) {
    OZ_ASSERT(brushes[i].nSides >= 0);

    if (brushes[i].nSides != 0) {
      ++i;
      continue;
    }

    brushes.erase(i);
    Log::print("Outside brush removed ");

    // adjust brush references (for leaves)
    for (int j = 0; j < leafBrushes.size();) {
      if (leafBrushes[j] < i) {
        ++j;
      }
      else if (i < leafBrushes[j]) {
        --leafBrushes[j];
        ++j;
      }
      else {
        leafBrushes.erase(j);
        Log::printRaw(".");

        for (oz::BSP::Leaf& leaf : leaves) {
          if (j < leaf.firstBrush) {
            --leaf.firstBrush;
          }
          else if (j < leaf.firstBrush + leaf.nBrushes) {
            OZ_ASSERT(leaf.nBrushes > 0);

            --leaf.nBrushes;
          }
        }
      }
    }
    // adjust brush references (for models)
    for (Model& model : models) {
      if (i < model.firstBrush) {
        --model.firstBrush;
      }
      else if (i < model.firstBrush + model.nBrushes) {
        OZ_ASSERT(model.nBrushes > 0);

        --model.nBrushes;
      }
    }
    Log::println();
  }

  // remove model brushes from the static tree (WTF Quake BSP puts them there?)
  Log::print("Removing model brush references ");

  for (Model& model : models) {
    for (int j = 0; j < model.nBrushes; ++j) {
      int brush = model.firstBrush + j;

      for (int k = 0; k < leafBrushes.size();) {
        if (leafBrushes[k] != brush) {
          ++k;
          continue;
        }

        leafBrushes.erase(k);
        Log::printRaw(".");

        // adjust leaf references
        for (oz::BSP::Leaf& leaf : leaves) {
          if (k < leaf.firstBrush) {
            --leaf.firstBrush;
          }
          else if (k < leaf.firstBrush + leaf.nBrushes) {
            OZ_ASSERT(leaf.nBrushes > 0);

            --leaf.nBrushes;
          }
        }
      }
    }
  }

  Log::printEnd(" OK");

  // remove unreferenced leaves
  Log::print("Removing unreferenced and empty leaves ");

  for (int i = 0; i < leaves.size();) {
    bool isReferenced = false;

    for (oz::BSP::Node& node : nodes) {
      if (node.front == ~i || node.back == ~i) {
        isReferenced = true;
        break;
      }
    }

    if (isReferenced && leaves[i].nBrushes != 0) {
      ++i;
      continue;
    }

    leaves.erase(i);
    Log::printRaw(".");

    // update references and tag unnecessary nodes, will be removed in the next pass (index 0 is
    // invalid as the root cannot be referenced)
    for (oz::BSP::Node& node : nodes) {
      if (~node.front == i) {
        node.front = 0;
      }
      else if (~node.front > i) {
        ++node.front;
      }

      if (~node.back == i) {
        node.back = 0;
      }
      else if (~node.back > i) {
        ++node.back;
      }
    }
  }

  Log::printEnd(" OK");

  // collapse unnecessary nodes
  Log::print("Collapsing nodes ");

  bool hasCollapsed = false;
  do {
    hasCollapsed = false;

    for (int i = 0; i < nodes.size();) {
      if (nodes[i].front != 0 && nodes[i].back != 0) {
        ++i;
        continue;
      }
      if (i == 0) {
        // change root node for one of its children, and set i to that child index, so it will
        // be removed instead of root
        if (nodes[0].front == 0) {
          i = nodes[0].back;
        }
        else if (nodes[0].back == 0) {
          i = nodes[0].front;
        }
        else {
          OZ_ASSERT(false);
        }
        nodes[0] = nodes[i];

        Log::printRaw("x");
      }
      else {
        // find parent
        int* parentsRef = nullptr;
        for (oz::BSP::Node& node : nodes) {
          if (node.front == i) {
            OZ_ASSERT(parentsRef == nullptr);

            parentsRef = &node.front;
          }
          if (node.back == i) {
            OZ_ASSERT(parentsRef == nullptr);

            parentsRef = &node.back;
          }
        }
        OZ_ASSERT(parentsRef != nullptr);

        if (nodes[i].front == 0) {
          *parentsRef = nodes[i].back;
        }
        else if (nodes[i].back == 0) {
          *parentsRef = nodes[i].front;
        }
        else {
          OZ_ASSERT(false);
        }
      }

      nodes.erase(i);

      for (oz::BSP::Node& node : nodes) {
        OZ_ASSERT(node.front != i);
        OZ_ASSERT(node.back != i);
      }

      // shift nodes' references
      for (oz::BSP::Node& node : nodes) {
        if (node.front > i) {
          --node.front;
        }
        if (node.back > i) {
          --node.back;
        }
      }

      Log::printRaw(".");
      hasCollapsed = true;
    }
  }
  while (hasCollapsed);

  Log::printEnd(" OK");

  // remove unused brush sides
  Log::print("Removing unused brush sides ");

  List<bool> usedBrushSides(brushSides.size());
  Arrays::clear(usedBrushSides.begin(), usedBrushSides.size());

  for (oz::BSP::Brush& brush : brushes) {
    for (int j = 0; j < brush.nSides; ++j) {
      usedBrushSides[brush.firstSide + j] = true;
    }
  }

  for (int i = 0; i < brushSides.size();) {
    if (usedBrushSides[i]) {
      ++i;
      continue;
    }

    brushSides.erase(i);
    usedBrushSides.erase(i);
    Log::printRaw(".");

    for (oz::BSP::Brush& brush : brushes) {
      if (i < brush.firstSide) {
        --brush.firstSide;
      }
      else if (i < brush.firstSide + brush.nSides) {
        // removed brush side shouldn't be referenced by any brush
        OZ_ASSERT(false);
      }
    }
  }

  usedBrushSides.clear();
  usedBrushSides.trim();

  Log::printEnd(" OK");

  // remove unused planes
  Log::print("Removing unused planes ");

  List<bool> usedPlanes(planes.size());

  for (oz::BSP::Node& node : nodes) {
    usedPlanes[node.plane] = true;
  }
  for (int brushSide : brushSides) {
    usedPlanes[brushSide] = true;
  }

  for (int i = 0; i < planes.size();) {
    if (usedPlanes[i]) {
      ++i;
      continue;
    }

    planes.erase(i);
    usedPlanes.erase(i);
    Log::printRaw(".");

    // adjust plane references
    for (oz::BSP::Node& node : nodes) {
      OZ_ASSERT(node.plane != i);

      if (node.plane > i) {
        --node.plane;
      }
    }
    for (int& brushSide : brushSides) {
      OZ_ASSERT(brushSide != i);

      if (brushSide > i) {
        --brushSide;
      }
    }
  }

  usedPlanes.clear();
  usedPlanes.trim();

  Log::printEnd(" OK");

  // optimise bounds
  Log::print("Fitting bounds: ");

  mins = Point(+Math::INF, +Math::INF, +Math::INF);
  maxs = Point(-Math::INF, -Math::INF, -Math::INF);

  for (int brushSide : brushSides) {
    Plane& plane = planes[brushSide];

    if (plane.n.x == -1.0f) {
      mins.x = min(-plane.d, mins.x);
    }
    else if (plane.n.x == 1.0f) {
      maxs.x = max(+plane.d, maxs.x);
    }
    else if (plane.n.y == -1.0f) {
      mins.y = min(-plane.d, mins.y);
    }
    else if (plane.n.y == 1.0f) {
      maxs.y = max(+plane.d, maxs.y);
    }
    else if (plane.n.z == -1.0f) {
      mins.z = min(-plane.d, mins.z);
    }
    else if (plane.n.z == 1.0f) {
      maxs.z = max(+plane.d, maxs.z);
    }
  }

  mins -= 2.0f * EPSILON * Vec3::ONE;
  maxs += 2.0f * EPSILON * Vec3::ONE;

  Log::printEnd("(%g %g %g) (%g %g %g)", mins.x, mins.y, mins.z, maxs.x, maxs.y, maxs.z);

  Log::unindent();
  Log::println("}");
  Log::println("Optimising BSP model {");
  Log::indent();

  // remove faces that lay out of boundaries
  for (int i = 0; i < faces.size();) {
    OZ_ASSERT(faces[i].nVertices > 0 && faces[i].nIndices >= 0);

    if (faces[i].nIndices != 0) {
      ++i;
      continue;
    }

    faces.erase(i);
    Log::println("Outside face removed");

    // adjust face references
    for (int j = 0; j < models.size() + 1; ++j) {
      if (i < modelFaces[j].firstFace) {
        --modelFaces[j].firstFace;
      }
      else if (i < modelFaces[j].firstFace + modelFaces[j].nFaces) {
        OZ_ASSERT(modelFaces[j].nFaces > 0);

        --modelFaces[j].nFaces;
      }
    }
  }

  Log::unindent();
  Log::println("}");
}

void BSP::check() const
{
  Log::print("Integrity check ...");

  Bitset usedNodes(nodes.size());
  Bitset usedLeaves(leaves.size());
  Bitset usedBrushes(brushes.size());

  for (const oz::BSP::Node& node : nodes) {
    if (node.front < 0) {
      if (usedLeaves.get(~node.front)) {
        OZ_ERROR("BSP leaf %d referenced twice", ~node.front);
      }
      usedLeaves.set(~node.front);
    }
    else if (node.front != 0) {
      if (usedNodes.get(node.front)) {
        OZ_ERROR("BSP node %d referenced twice", node.front);
      }
      usedNodes.set(node.front);
    }
    else {
      OZ_ERROR("BSP root node referenced");
    }

    if (node.back < 0) {
      if (usedLeaves.get(~node.back)) {
        OZ_ERROR("BSP leaf %d referenced twice", ~node.back);
      }
      usedLeaves.set(~node.back);
    }
    else if (node.back != 0) {
      if (usedNodes.get(node.back)) {
        OZ_ERROR("BSP node %d referenced twice", node.back);
      }
      usedNodes.set(node.back);
    }
    else {
      OZ_ERROR("BSP root node referenced");
    }
  }

  for (const Model& model : models) {
    for (int j = 0; j < model.nBrushes; ++j) {
      int index = model.firstBrush + j;

      if (usedBrushes.get(index)) {
        OZ_ERROR("BSP brush %d referenced by two models", index);
      }
      usedBrushes.set(index);
    }
  }

  usedBrushes.clear();

  for (const oz::BSP::Leaf& leaf : leaves) {
    for (int j = 0; j < leaf.nBrushes; ++j) {
      int index = leafBrushes[leaf.firstBrush + j];

      usedBrushes.set(index);
    }
  }
  for (const Model& model : models) {
    for (int j = 0; j < model.nBrushes; ++j) {
      int index = model.firstBrush + j;

      if (usedBrushes.get(index)) {
        OZ_ERROR("BSP model brush %d referenced by static tree", index);
      }
      usedBrushes.set(index);
    }
  }

  if (usedNodes.get(0)) {
    OZ_ERROR("BSP root node referenced");
  }
  for (int i = 1; i < nodes.size(); ++i) {
    if (!usedNodes.get(i)) {
      OZ_ERROR("BSP node %d not referenced", i);
    }
  }
  for (int i = 0; i < leaves.size(); ++i) {
    if (!usedLeaves.get(i)) {
      OZ_ERROR("BSP leaf %d not referenced", i);
    }
  }
  for (int i = 0; i < brushes.size(); ++i) {
    if (!usedBrushes.get(i)) {
      OZ_ERROR("BSP brush %d not referenced", i);
    }
  }

  for (int i = 0; i < planes.size(); ++i) {
    if (!Math::isFinite(planes[i].d)) {
      OZ_ERROR("BSP has invalid plane %d", i);
    }
  }

  Log::printEnd(" OK");

  Log::println("Statistics {");
  Log::indent();
  Log::println("%4d  models",      models.size());
  Log::println("%4d  nodes",       nodes.size());
  Log::println("%4d  leaves",      leaves.size());
  Log::println("%4d  brushes",     brushes.size());
  Log::println("%4d  brush sides", brushSides.size());
  Log::println("%4d  planes",      planes.size());
  Log::println("%4d  faces",       faces.size());
  Log::println("%4d  textures",    textures.size());
  Log::unindent();
  Log::println("}");
}

void BSP::saveMatrix()
{
  File destFile = "bsp/" + name + ".ozBSP";

  Log::print("Writing BSP structure to '%s' ...", destFile.c());

  Stream os(0, Endian::LITTLE);

  os.write<Point>(mins);
  os.write<Point>(maxs);

  os.writeString(title);
  os.writeString(description);

  os.writeFloat(life);
  os.writeFloat(resistance);

  os.writeString(fragPool);
  os.writeInt(nFrags);

  os.writeString(demolishSound);
  os.writeFloat(groundOffset);

  os.writeInt(planes.size());
  os.writeInt(nodes.size());
  os.writeInt(leaves.size());
  os.writeInt(leafBrushes.size());
  os.writeInt(brushes.size());
  os.writeInt(brushSides.size());
  os.writeInt(models.size());
  os.writeInt(boundObjects.size());

  for (const Plane& plane : planes) {
    os.write<Plane>(plane);
  }

  for (const oz::BSP::Node& node : nodes) {
    os.writeInt(node.plane);
    os.writeInt(node.front);
    os.writeInt(node.back);
  }

  for (const oz::BSP::Leaf& leaf : leaves) {
    os.writeInt(leaf.firstBrush);
    os.writeInt(leaf.nBrushes);
  }

  for (int leafBrush : leafBrushes) {
    os.writeInt(leafBrush);
  }

  for (const oz::BSP::Brush& brush : brushes) {
    os.writeInt(brush.firstSide);
    os.writeInt(brush.nSides);
    os.writeInt(brush.flags);
  }

  for (int brushSide : brushSides) {
    os.writeInt(brushSide);
  }

  for (const Model& model : models) {
    os.write<Point>(model.mins);
    os.write<Point>(model.maxs);

    os.writeInt(model.firstBrush);
    os.writeInt(model.nBrushes);

    os.writeString(model.title);

    os.writeInt(model.type);
    os.writeInt(model.flags);

    os.writeFloat(model.closeTimeout);
    os.writeFloat(model.openTimeout);

    os.write<Vec3>(model.moveDir);
    os.writeFloat(model.moveLength);
    os.writeFloat(model.moveStep);

    os.writeFloat(model.margin);

    context.usedSounds.include(model.openSound, name + " (BSP)");
    context.usedSounds.include(model.closeSound, name + " (BSP)");
    context.usedSounds.include(model.frictSound, name + " (BSP)");

    os.writeString(model.openSound);
    os.writeString(model.closeSound);
    os.writeString(model.frictSound);

    os.writeInt(model.target);
    os.writeInt(model.key);

    context.usedModels.include(model.modelName, name + " (BSP)");

    os.writeString(model.modelName);
    os.write<Mat4>(model.modelTransf);
  }

  for (const BoundObject& boundObject : boundObjects) {
    os.writeString(boundObject.clazz);
    os.write<Point>(boundObject.pos);
    os.writeInt(boundObject.heading);
  }

  if (!destFile.write(os)) {
    OZ_ERROR("Failed to write '%s'", destFile.c());
  }

  Log::printEnd(" OK");
}

void BSP::saveClient()
{
  File destFile = "bsp/" + name + ".ozcModel";

  compiler.beginModel();

  compiler.enable(Compiler::UNIQUE);
  compiler.enable(Compiler::CLOCKWISE);

  for (int i = 0; i <= models.size(); ++i) {
    compiler.beginNode();

    if (modelFaces[i].nFaces != 0) {
      compiler.beginMesh();

      int lastTexture = -1;

      for (int j = 0; j < modelFaces[i].nFaces; ++j) {
        const Face&    face = faces[modelFaces[i].firstFace + j];
        const Texture& tex  = textures[face.texture];

        if (tex.name.isEmpty()) {
          OZ_ERROR("BSP has a visible face without a texture");
        }
        if (!tex.name.beginsWith("@sea:")) {
          context.usedTextures.include(tex.name, name + " (BSP)");
        }

        if (face.texture != lastTexture) {
          if (j != 0) {
            int meshId = compiler.endMesh();

            compiler.beginNode();
            compiler.bindMesh(meshId);
            compiler.endNode();

            compiler.beginMesh();
          }

          compiler.texture(tex.name);
          compiler.blend(tex.type & QBSP_ALPHA_TYPE_BIT);

          lastTexture = face.texture;
        }

        compiler.begin(Compiler::TRIANGLES);

        for (int k = 0; k < face.nIndices; ++k) {
          const QBSPVertex& v = vertices[face.firstVertex + indices[face.firstIndex + k]];

          compiler.texCoord(v.texCoord);
          compiler.normal(v.normal);
          compiler.vertex(v.pos);
        }

        if (textures[face.texture].type & QBSP_WATER_TYPE_BIT) {
          for (int k = face.nIndices - 1; k >= 0; --k) {
            const QBSPVertex& v = vertices[face.firstVertex + indices[face.firstIndex + k]];

            compiler.texCoord(v.texCoord);
            compiler.normal(-v.normal[0], -v.normal[1], -v.normal[2]);
            compiler.vertex(v.pos);
          }
        }

        compiler.end();
      }

      int meshId = compiler.endMesh();

      compiler.beginNode();
      compiler.bindMesh(meshId);
      compiler.endNode();
    }

    compiler.endNode();
  }

  compiler.endModel();

  Stream os(0, Endian::LITTLE);

  compiler.writeModel(&os, true);

  os.writeInt(leafClusters.size());

  for (int leafCluster : leafClusters) {
    os.writeInt(leafCluster);
  }

  os.writeInt(nClusters);
  os.writeInt(nClusterBytes * 8);
  os.writeBitset(clusters);

  os.write<Vec4>(waterFogColour);
  os.write<Vec4>(lavaFogColour);

  Log::print("Writing BSP model to '%s' ...", destFile.c());

  if (!destFile.write(os)) {
    OZ_ERROR("Failed to write '%s'", destFile.c());
  }

  Log::printEnd(" OK");
}

void BSP::build(const char* name_)
{
  Log::println("Prebuilding BSP '%s' {", name_);
  Log::indent();

  name = name_;

  load();
  optimise();
  check();
  saveMatrix();
  saveClient();

  name          = "";
  title         = "";
  description   = "";
  fragPool      = "";
  demolishSound = "";

  textures.clear();
  planes.clear();
  planes.trim();
  nodes.clear();
  nodes.trim();
  leaves.clear();
  leaves.trim();
  leafBrushes.clear();
  leafBrushes.trim();
  models.clear();
  brushes.clear();
  brushes.trim();
  brushSides.clear();
  brushSides.trim();
  modelFaces.clear();
  vertices.clear();
  indices.clear();
  faces.clear();
  faces.trim();
  boundObjects.clear();
  boundObjects.trim();

  Log::unindent();
  Log::println("}");
}

BSP bsp;

}
