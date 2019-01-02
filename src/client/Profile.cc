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

#include <client/Profile.hh>

#include <matrix/BotClass.hh>
#include <matrix/WeaponClass.hh>

#include <cstdlib>
#include <cwchar>
#include <cwctype>

namespace oz::client
{

void Profile::save()
{
  File profileFile = appConfig["dir.config"].get(String::EMPTY) + "/profile.json";
  Json profileConfig = Json::OBJECT;

  profileConfig.add("_version", OZ_VERSION);
  profileConfig.add("name", name);
  profileConfig.add("class", clazz == nullptr ? String::EMPTY : clazz->name);

  Json& itemsConfig = profileConfig.add("items", Json::ARRAY);
  for (const ObjectClass* item : items) {
    itemsConfig.add(item->name);
  }

  profileConfig.add("weaponItem", weaponItem);
  profileConfig.add("persistent", persistent);

  profileConfig.save(profileFile, CONFIG_FORMAT);
}

void Profile::init()
{
  File profileFile = appConfig["dir.config"].get(String::EMPTY) + "/profile.json";
  Json profileConfig(profileFile);

  name       = profileConfig["name"].get("");
  clazz      = nullptr;
  weaponItem = -1;

  // Get username and capitalise it (needs conversion to Unicode and back to UTF-8).
  if (name.isEmpty()) {
    const char* userName = getenv("USER");

    if (userName == nullptr || String::isEmpty(userName)) {
      name = OZ_GETTEXT("Player");
    }
    else {
      wchar_t wcUserName[128];
      char    mbUserName[128];

      mbstowcs(wcUserName, userName, 127);
      wcUserName[0] = wchar_t(towupper(wint_t(wcUserName[0])));
      wcUserName[127] = L'\0';

      wcstombs(mbUserName, wcUserName, 127);
      mbUserName[127] = '\0';

      name = mbUserName;
    }
  }

  if (profileConfig["_version"].get(String::EMPTY) != OZ_VERSION) {
    const String& sClazz = profileConfig["class"].get(String::EMPTY);
    clazz = sClazz.isEmpty() ? nullptr : static_cast<const BotClass*>(liber.objClass(sClazz));

    if (clazz != nullptr) {
      const Json& itemsConfig = profileConfig["items"];
      int nItems = itemsConfig.size();

      if (nItems > clazz->nItems) {
        OZ_ERROR("Too many items for player class '%s' in profile", clazz->name.c());
      }

      items.clear();
      items.trim();

      for (int i = 0; i < nItems; ++i) {
        const char* sItem = itemsConfig[i].get("");

        const ObjectClass* itemClazz = liber.objClass(sItem);
        if ((itemClazz->flags & (Object::DYNAMIC_BIT | Object::ITEM_BIT)) !=
            (Object::DYNAMIC_BIT | Object::ITEM_BIT))
        {
          OZ_ERROR("Invalid item '%s' in profile", sItem);
        }

        items.add(static_cast<const DynamicClass*>(itemClazz));
      }

      weaponItem = profileConfig["weaponItem"].get(-1);

      if (weaponItem != -1) {
        if (uint(weaponItem) >= uint(items.size())) {
          OZ_ERROR("Invalid weaponItem #%d in profile", weaponItem);
        }

        const WeaponClass* weaponClazz = static_cast<const WeaponClass*>(items[weaponItem]);

        if (!(weaponClazz->flags & Object::WEAPON_BIT)) {
          OZ_ERROR("Invalid weaponItem #%d '%s' in profile",
                   weaponItem, weaponClazz->name.c());
        }

        if (!clazz->name.beginsWith(weaponClazz->userBase)) {
          OZ_ERROR("Invalid weapon class '%s' for player class '%s' in profile",
                   weaponClazz->name.c(), clazz->name.c());
        }
      }
    }
  }

  persistent = profileConfig["persistent"];

  if (persistent.type() != Json::OBJECT) {
    persistent = Json::OBJECT;
  }
}

void Profile::destroy()
{
  save();

  items.clear();
  items.trim();
  persistent.clear();
}

Profile profile;

}
