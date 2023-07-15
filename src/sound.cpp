#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include <sndfile.h>
#include <vector>

bool init_sound(ALCdevice** device, ALCcontext** context) {
    *device = alcOpenDevice(NULL);
    if (!*device) {
        std::cerr << "Failed to open the audio device" << std::endl;
        return false;
    }

    *context = alcCreateContext(*device, NULL);
    if (!*context) {
        std::cerr << "Failed to create audio context" << std::endl;
        return false;
    }

    if (!alcMakeContextCurrent(*context)) {
        std::cerr << "Failed to make audio context current" << std::endl;
        return false;
    }

    return true;
}

void cleanup_sound(ALCdevice* device, ALCcontext* context) {
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

bool play_sound(const std::string& filename) {
    // Open the audio file
    SF_INFO fileInfo;
    SNDFILE* audioFile = sf_open(filename.c_str(), SFM_READ, &fileInfo);
    if (!audioFile) {
        std::cerr << "Failed to open audio file: " << filename << std::endl;
        return false;
    }

    // Read the audio data
    std::vector<short> samples(fileInfo.frames * fileInfo.channels);
    sf_read_short(audioFile, samples.data(), samples.size());

    // Get the audio file info
    ALsizei dataSize = samples.size() * sizeof(short);
    ALsizei sampleRate = fileInfo.samplerate;
    ALenum format = (fileInfo.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    // Generate an OpenAL buffer
    ALuint bufferID;
    alGenBuffers(1, &bufferID);

    // Fill the buffer with audio data
    alBufferData(bufferID, format, samples.data(), dataSize, sampleRate);

    // Generate an OpenAL source
    ALuint sourceID;
    alGenSources(1, &sourceID);

    // Attach the buffer to the source
    alSourcei(sourceID, AL_BUFFER, bufferID);

    // Play the sound
    alSourcePlay(sourceID);

    // Wait until the sound finishes playing
    ALint state;
    do {
        alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);

    // Clean up resources
    alDeleteSources(1, &sourceID);
    alDeleteBuffers(1, &bufferID);
    sf_close(audioFile);

    return true;
}
