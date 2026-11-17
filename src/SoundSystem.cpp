#include "SoundSystem.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

SoundSystem::SoundSystem() : enabled_(true) {
    try {
        const unsigned int sampleRate = 44100;

        // 1. Step (short low click)
        generateTone(stepBuffer_, 120.0, 60.0, 0.04, 0.15, 0);
        soundStep_ = std::make_unique<sf::Sound>(stepBuffer_);

        // 2. Interact (clean pip)
        generateTone(interactBuffer_, 520.0, 780.0, 0.08, 0.3, 0);
        soundInteract_ = std::make_unique<sf::Sound>(interactBuffer_);

        // 3. Pickup (pleasant chime)
        generateTone(pickupBuffer_, 587.0, 1174.0, 0.18, 0.35, 0);
        soundPickup_ = std::make_unique<sf::Sound>(pickupBuffer_);

        // 4. DoorUnlock (positive mechanical release)
        generateTone(doorUnlockBuffer_, 350.0, 700.0, 0.28, 0.4, 0);
        soundDoorUnlock_ = std::make_unique<sf::Sound>(doorUnlockBuffer_);

        // 5. DoorLocked (deep denial thud)
        generateTone(doorLockedBuffer_, 160.0, 80.0, 0.16, 0.4, 1);
        soundDoorLocked_ = std::make_unique<sf::Sound>(doorLockedBuffer_);

        // 6. TrapTrigger (gritty alarm buzz)
        generateNoise(trapTriggerBuffer_, 0.35, 0.45);
        soundTrapTrigger_ = std::make_unique<sf::Sound>(trapTriggerBuffer_);

        // 7. PuzzleSuccess (bright harmonic success)
        generateTone(puzzleSuccessBuffer_, 440.0, 880.0, 0.45, 0.45, 2);
        soundPuzzleSuccess_ = std::make_unique<sf::Sound>(puzzleSuccessBuffer_);

        // 8. PuzzleFail (low error buzz)
        generateTone(puzzleFailBuffer_, 220.0, 110.0, 0.28, 0.4, 1);
        soundPuzzleFail_ = std::make_unique<sf::Sound>(puzzleFailBuffer_);

        // 9. Heal (warm ascending shimmer)
        generateTone(healBuffer_, 330.0, 660.0, 0.32, 0.35, 0);
        soundHeal_ = std::make_unique<sf::Sound>(healBuffer_);

        // 10. Victory (glorious rising fanfare)
        generateTone(victoryBuffer_, 440.0, 1320.0, 0.8, 0.5, 2);
        soundVictory_ = std::make_unique<sf::Sound>(victoryBuffer_);

        // 11. GameOver (descending failure tone)
        generateTone(gameOverBuffer_, 300.0, 75.0, 0.7, 0.5, 1);
        soundGameOver_ = std::make_unique<sf::Sound>(gameOverBuffer_);
    } catch (const std::exception& e) {
        std::cerr << "SoundSystem initialization notice: " << e.what() << "\n";
        enabled_ = false;
    }
}

void SoundSystem::generateTone(sf::SoundBuffer& buffer, double freqStart, double freqEnd, double durationSec, double volume, int waveType) {
    const unsigned int sampleRate = 44100;
    const std::size_t sampleCount = static_cast<std::size_t>(sampleRate * durationSec);
    if (sampleCount == 0) return;

    std::vector<std::int16_t> samples(sampleCount);

    double currentPhase = 0.0;
    for (std::size_t i = 0; i < sampleCount; ++i) {
        double progress = static_cast<double>(i) / sampleCount;
        double currentFreq = freqStart + (freqEnd - freqStart) * progress;

        currentPhase += 2.0 * M_PI * currentFreq / sampleRate;
        if (currentPhase > 2.0 * M_PI) {
            currentPhase -= 2.0 * M_PI;
        }

        double rawSample = 0.0;
        if (waveType == 0) {
            // Sine wave
            rawSample = std::sin(currentPhase);
        } else if (waveType == 1) {
            // Square wave with smoothed corners
            rawSample = (std::sin(currentPhase) >= 0.0 ? 0.7 : -0.7);
        } else {
            // Triangle / Organ wave
            rawSample = 0.7 * std::sin(currentPhase) + 0.3 * std::sin(currentPhase * 2.0);
        }

        // Apply ADSR Envelope to eliminate clicks
        double envelope = 1.0;
        if (progress < 0.1) {
            envelope = progress / 0.1; // attack
        } else if (progress > 0.7) {
            envelope = (1.0 - progress) / 0.3; // release
        }

        double finalSample = rawSample * envelope * volume * 32767.0;
        if (finalSample > 32767.0) finalSample = 32767.0;
        if (finalSample < -32768.0) finalSample = -32768.0;

        samples[i] = static_cast<std::int16_t>(finalSample);
    }

    // In SFML 3: loadFromSamples(const std::int16_t* samples, std::uint64_t sampleCount, unsigned int channelCount, unsigned int sampleRate)
    (void)buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate, {sf::SoundChannel::Mono});
}

void SoundSystem::generateNoise(sf::SoundBuffer& buffer, double durationSec, double volume) {
    const unsigned int sampleRate = 44100;
    const std::size_t sampleCount = static_cast<std::size_t>(sampleRate * durationSec);
    if (sampleCount == 0) return;

    std::vector<std::int16_t> samples(sampleCount);

    for (std::size_t i = 0; i < sampleCount; ++i) {
        double progress = static_cast<double>(i) / sampleCount;
        double envelope = (progress < 0.05) ? (progress / 0.05) : (1.0 - progress);

        double randomVal = (static_cast<double>(std::rand()) / RAND_MAX) * 2.0 - 1.0;
        double sample = randomVal * envelope * volume * 32767.0;

        if (sample > 32767.0) sample = 32767.0;
        if (sample < -32768.0) sample = -32768.0;

        samples[i] = static_cast<std::int16_t>(sample);
    }

    (void)buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate, {sf::SoundChannel::Mono});
}

void SoundSystem::play(SoundEffect effect) {
    if (!enabled_) return;

    try {
        switch (effect) {
            case SoundEffect::Step:
                if (soundStep_) soundStep_->play();
                break;
            case SoundEffect::Interact:
                if (soundInteract_) soundInteract_->play();
                break;
            case SoundEffect::Pickup:
                if (soundPickup_) soundPickup_->play();
                break;
            case SoundEffect::DoorUnlock:
                if (soundDoorUnlock_) soundDoorUnlock_->play();
                break;
            case SoundEffect::DoorLocked:
                if (soundDoorLocked_) soundDoorLocked_->play();
                break;
            case SoundEffect::TrapTrigger:
                if (soundTrapTrigger_) soundTrapTrigger_->play();
                break;
            case SoundEffect::PuzzleSuccess:
                if (soundPuzzleSuccess_) soundPuzzleSuccess_->play();
                break;
            case SoundEffect::PuzzleFail:
                if (soundPuzzleFail_) soundPuzzleFail_->play();
                break;
            case SoundEffect::Heal:
                if (soundHeal_) soundHeal_->play();
                break;
            case SoundEffect::Victory:
                if (soundVictory_) soundVictory_->play();
                break;
            case SoundEffect::GameOver:
                if (soundGameOver_) soundGameOver_->play();
                break;
        }
    } catch (...) {
        // Silently tolerate audio device hiccups
    }
}

void SoundSystem::setEnabled(bool enabled) {
    enabled_ = enabled;
}

bool SoundSystem::isEnabled() const {
    return enabled_;
}
