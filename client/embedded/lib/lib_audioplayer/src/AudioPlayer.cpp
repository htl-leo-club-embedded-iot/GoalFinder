#include <AudioPlayer.h>
#include "util/Logger.h"

AudioPlayer::AudioPlayer(FileSystem* fileSystem, int bclkPin, int wclkPin, int doutPin) : volumePc(0)
{
    this->fileSystem = fileSystem;
    currentFile = new AudioFileSourceFS(*fileSystem->GetInternalFileSystem());
    mp3Generator = new AudioGeneratorMP3();
    audioOutput = new AudioOutputI2S();
    audioOutput->SetPinout(bclkPin, wclkPin, doutPin);
    SetVolume(50);
}

AudioPlayer::~AudioPlayer() 
{
    delete currentFile;
    delete mp3Generator;
    delete audioOutput;
}

void AudioPlayer::PlayMP3(const char* path)
{
    Stop();
    currentFile->open(path);
    mp3Generator->begin(currentFile, audioOutput);
}

void AudioPlayer::SetVolume(uint8_t percent) 
{
    if (percent != volumePc) {
        volumePc = percent;
        // gain is in range from 0.0 to 4.0-epsilon (4.0 mutes the sound)
        // "percent" is in range from 0 to 100
        float base = 25.0f;
        float epsilon = 0.01f;
        // clip the volume
        uint8_t gainPc = volumePc > 100 ? 100 : volumePc;
        // calculate the gain for the player
        float gain = (gainPc / base) - epsilon;
        
        Logger::log("AudioPlayer", Logger::LogLevel::INFO, "%4.3f: setting audio gain to '%.3f'", millis() / 1000.0, gain);
        audioOutput->SetGain(gain);
    }
}

void AudioPlayer::Loop() 
{
    if(mp3Generator->isRunning() && !mp3Generator->loop()) {
        mp3Generator->stop();
    }
}

void AudioPlayer::Stop() 
{
    if(IsPlaying()) {
        mp3Generator->stop();
    }
}

bool AudioPlayer::IsPlaying() 
{
    return mp3Generator->isRunning();
}