// Copyright 2019 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <AL/al.h>
#include <AL/alc.h>
#include <libnyquist/Decoders.h>

#include <algorithm>
#include <iostream>
#include <smk/Audio.hpp>
#include <smk/SoundBuffer.hpp>
#include <vector>

namespace smk {

SoundBuffer::SoundBuffer() {}

/// @brief Load a sound resource into memory from a file.
SoundBuffer::SoundBuffer(const std::string filename) : SoundBuffer() {
  if (!Audio::Initialized()) {
    static bool once = true;
    if (once) {
      std::cerr
          << "Error: smk::Audio has not been initialized. Please create a "
             "smk::Audio instance in the main() function before creating a "
             "smk::SoundBuffer"
          << std::endl;
      once = false;
    }
  }

  nqr::AudioData fileData;
  nqr::NyquistIO loader;
  loader.Load(&fileData, filename);

  ALsizei sample_rate = static_cast<ALsizei>(fileData.sampleRate);

  std::vector<ALshort> data;
  for (auto& it : fileData.samples) {
    it = std::min(it, +1.f);
    it = std::max(it, -1.f);
    data.push_back(it * ((1 << 15) - 1));
  }

  // clang-format off
  ALenum format;
  switch (fileData.channelCount) {
    case 1: format = AL_FORMAT_MONO16; break;
    case 2: format = AL_FORMAT_STEREO16; break;
    default: std::cerr << "SoundBuffer: Unsupported format file " + filename << std::endl;
      return;
  }
  // clang-format on.

  alGenBuffers(1, &buffer);
  alBufferData(buffer, format, data.data(), data.size() * sizeof(ALshort), sample_rate);

  if (alGetError() != AL_NO_ERROR) {
    std::cerr << "SoundBuffer: OpenAL error" << std::endl;
    return;
  }
}

SoundBuffer::~SoundBuffer() {
  if (buffer)
    alDeleteBuffers(1, &buffer);
}

SoundBuffer::SoundBuffer(SoundBuffer&& o) {
  this->operator=(std::move(o));
}

void SoundBuffer::operator=(SoundBuffer&& o) {
  std::swap(buffer, o.buffer);
}

}  // namespace smk
