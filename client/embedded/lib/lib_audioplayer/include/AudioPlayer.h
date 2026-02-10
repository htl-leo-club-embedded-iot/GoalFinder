#pragma once
#include <FileSystem.h>
#include <AudioFileSourceFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>

class AudioPlayer 
{
    public:
        AudioPlayer(FileSystem* fileSystem, int bclkPin, int wclkPin, int doutPin);
        virtual ~AudioPlayer();
        void PlayMP3(const char* path);
        void SetVolume(uint8_t percent);
        void Loop();
        void Stop();
        bool IsPlaying();
    private:
        FileSystem* fileSystem;
        AudioFileSource* currentFile;
        AudioGeneratorMP3* mp3Generator;
        AudioOutputI2S* audioOutput;
        /** The volume in percent */
        uint8_t volumePc;
};