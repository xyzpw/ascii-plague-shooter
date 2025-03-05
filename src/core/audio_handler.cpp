#include <thread>
#include <mutex>
#include <condition_variable>
#include <SDL2/SDL.h>
#include "audio_handler.h"

std::mutex audioMutex;
std::condition_variable audioFinishCondition;
bool isAudioPlaying = false;

void playAudio(std::string filename)
{
    if (filename == ""){
        return;
    }

    {
        std::lock_guard<std::mutex> lock(audioMutex);
        isAudioPlaying = true;
    }
    std::string location = std::string("assets/") + filename;
    SDL_AudioSpec wavSpec;
    Uint8* wavBuf;
    Uint32 wavLen;

    if (SDL_LoadWAV(location.c_str(), &wavSpec, &wavBuf, &wavLen) == NULL)
    {
        {
            std::lock_guard<std::mutex> lock(audioMutex);
            isAudioPlaying = false;
        }
        audioFinishCondition.notify_all();
        return;
    }

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    SDL_QueueAudio(device, wavBuf, wavLen);
    SDL_PauseAudioDevice(device, 0);

    while (SDL_GetQueuedAudioSize(device) > 0)
        SDL_Delay(10);

    {
        std::lock_guard<std::mutex> lock(audioMutex);
        try{
            SDL_CloseAudioDevice(device);
            SDL_FreeWAV(wavBuf);
        } catch(...){
            audioFinishCondition.notify_all();
        }
        isAudioPlaying = false;
    }
    audioFinishCondition.notify_all();
}

void initializeAudio()
{
    SDL_Init(SDL_INIT_AUDIO);
}
void cleanupAudio()
{
    std::unique_lock<std::mutex> lock(audioMutex);
    audioFinishCondition.wait(lock, [] { return !isAudioPlaying; });
    SDL_Quit();
}
