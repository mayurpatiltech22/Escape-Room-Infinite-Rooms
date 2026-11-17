#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H

#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <cstdint>

enum class SoundEffect {
    Step,
    Interact,
    Pickup,
    DoorUnlock,
    DoorLocked,
    TrapTrigger,
    PuzzleSuccess,
    PuzzleFail,
    Heal,
    Victory,
    GameOver
};

class SoundSystem {
private:
    bool enabled_;
    sf::SoundBuffer stepBuffer_;
    sf::SoundBuffer interactBuffer_;
    sf::SoundBuffer pickupBuffer_;
    sf::SoundBuffer doorUnlockBuffer_;
    sf::SoundBuffer doorLockedBuffer_;
    sf::SoundBuffer trapTriggerBuffer_;
    sf::SoundBuffer puzzleSuccessBuffer_;
    sf::SoundBuffer puzzleFailBuffer_;
    sf::SoundBuffer healBuffer_;
    sf::SoundBuffer victoryBuffer_;
    sf::SoundBuffer gameOverBuffer_;

    std::unique_ptr<sf::Sound> soundStep_;
    std::unique_ptr<sf::Sound> soundInteract_;
    std::unique_ptr<sf::Sound> soundPickup_;
    std::unique_ptr<sf::Sound> soundDoorUnlock_;
    std::unique_ptr<sf::Sound> soundDoorLocked_;
    std::unique_ptr<sf::Sound> soundTrapTrigger_;
    std::unique_ptr<sf::Sound> soundPuzzleSuccess_;
    std::unique_ptr<sf::Sound> soundPuzzleFail_;
    std::unique_ptr<sf::Sound> soundHeal_;
    std::unique_ptr<sf::Sound> soundVictory_;
    std::unique_ptr<sf::Sound> soundGameOver_;

    void generateTone(sf::SoundBuffer& buffer, double freqStart, double freqEnd, double durationSec, double volume = 0.5, int waveType = 0);
    void generateNoise(sf::SoundBuffer& buffer, double durationSec, double volume = 0.5);

public:
    SoundSystem();
    ~SoundSystem() = default;

    void play(SoundEffect effect);
    void setEnabled(bool enabled);
    bool isEnabled() const;
};

#endif // SOUND_SYSTEM_H
