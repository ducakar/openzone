/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifdef __native_client__

#include "Pepper.hh"

#include "SpinLock.hh"

#include <cstdio>
#include <ppapi_simple/ps.h>
#include <ppapi_simple/ps_event.h>
#include <ppapi_simple/ps_interface.h>

namespace oz
{

static PP_Instance          ppInstance;
static const PPB_Core*      ppbCore       = nullptr;
static const PPB_Var*       ppbVar        = nullptr;
static const PPB_Messaging* ppbMessaging  = nullptr;
static List<String>         messageQueue;
static SpinLock             messageLock;
static Semaphore            mainSemaphore;

static void messageHandler(PP_Var, PP_Var value, void*)
{
  uint        length;
  const char* message = ppbVar->VarToUtf8(value, &length);

  messageLock.lock();
  messageQueue.add(String(message, length));
  messageLock.unlock();
}

void Pepper::mainCall(Callback* callback, void* data)
{
  ppbCore->CallOnMainThread(0, PP_MakeCompletionCallback(callback, data), 0);
}

void Pepper::post(const char* message)
{
  PP_Var var = ppbVar->VarFromUtf8(message, String::length(message));

  ppbMessaging->PostMessage(ppInstance, var);
  ppbVar->Release(var);
}

String Pepper::pop()
{
  messageLock.lock();
  String s = messageQueue.popFirst();
  messageLock.unlock();

  return s;
}

void Pepper::push(const char* message)
{
  messageLock.lock();
  messageQueue.pushLast(message);
  messageLock.unlock();
}

void Pepper::init()
{
  ppInstance   = PSGetInstanceId();
  ppbCore      = PSInterfaceCore();
  ppbVar       = PSInterfaceVar();
  ppbMessaging = PSInterfaceMessaging();

  PSEventRegisterMessageHandler("oz", messageHandler, nullptr);
}

thread_local Semaphore* MainCall::localSemaphore = &mainSemaphore;

}

#endif
