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
 * @file client/NaClDownloader.cc
 */

#ifdef __native_client__

#include <client/NaClDownloader.hh>

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/url_request_info.h>
#include <ppapi/cpp/url_loader.h>

namespace oz
{
namespace client
{

void NaClDownloader::readCallback(void* data, int result)
{
  NaClDownloader* downloader = static_cast<NaClDownloader*>(data);

  if (result < 0) {
    downloader->stream.free();
  }
  else {
    int length = downloader->stream.tell() - 4096 + result;

    downloader->stream.rewind();
    downloader->stream.skip(length);

    if (result != 0) {
      pp::URLLoader* loader = downloader->loader;

      long64 received, total;
      int hasProgress = loader->GetDownloadProgress(&received, &total);

      if (hasProgress && total >= 0) {
        downloader->downloadProgress = float(received) / float(total);
      }
      else {
        downloader->downloadProgress = Math::NaN;
      }

      int ret = loader->ReadResponseBody(downloader->stream.skip(4096), 4096,
                                         pp::CompletionCallback(readCallback, downloader));
      if (ret == PP_OK_COMPLETIONPENDING) {
        return;
      }
    }
  }

  delete downloader->loader;

  downloader->semaphore.post();
}

void NaClDownloader::beginCallback(void* data, int result)
{
  NaClDownloader* downloader = static_cast<NaClDownloader*>(data);

  if (result == PP_OK) {
    pp::URLLoader* loader = downloader->loader;
    int ret = loader->ReadResponseBody(downloader->stream.skip(4096), 4096,
                                       pp::CompletionCallback(readCallback, downloader));
    if (ret == PP_OK_COMPLETIONPENDING) {
      return;
    }
  }

  delete downloader->loader;

  downloader->stream.free();
  downloader->semaphore.post();
}

bool NaClDownloader::isComplete() const
{
  return semaphore.counter() == 1;
}

float NaClDownloader::progress() const
{
  return downloadProgress;
}

void NaClDownloader::begin(const char* url_)
{
  url = url_;

  stream = OutputStream(0);

  MainCall() << [&]
  {
    pp::URLRequestInfo request(Pepper::instance());
    request.SetURL(url);
    request.SetMethod("GET");
    request.SetRecordDownloadProgress(true);

    loader = new pp::URLLoader(Pepper::instance());
    if (loader == nullptr) {
      semaphore.post();
      return;
    }

    int ret = loader->Open(request, pp::CompletionCallback(beginCallback, this));
    if (ret != PP_OK_COMPLETIONPENDING) {
      delete loader;
      semaphore.post();
      return;
    }
  };
}

OutputStream NaClDownloader::take()
{
  semaphore.wait();

  return static_cast<OutputStream&&>(stream);
}

}
}

#endif
