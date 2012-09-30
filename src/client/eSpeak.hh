/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file client/eSpeak.hh
 */

#pragma once

#include <client/common.hh>

#include <espeak/speak_lib.h>

namespace oz
{
namespace client
{

extern decltype( ::espeak_Initialize       )* espeak_Initialize;
extern decltype( ::espeak_Terminate        )* espeak_Terminate;
extern decltype( ::espeak_SetParameter     )* espeak_SetParameter;
extern decltype( ::espeak_SetVoiceByName   )* espeak_SetVoiceByName;
extern decltype( ::espeak_SetSynthCallback )* espeak_SetSynthCallback;
extern decltype( ::espeak_Synth            )* espeak_Synth;

}
}
