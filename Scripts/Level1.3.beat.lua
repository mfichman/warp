-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- outside part. rings of seekers and starblazers (note: they don't shoot at you yet but they should)

Level:loadSFX{id=0, path="sfx/dance_hihat_02.wav"} -- on fire
Level:loadSFX{id=1, path="sfx/dance_clap_01.wav"} -- on hit

Level:loadSFX{id=2, path="sfx/Tape Rewinding 02_C.wav"} -- on banshee explosion sound

Level:loadLoop{id=0, path="loops/Dance Floor Pattern 26 120_C_4.wav", bpm=80, n_beats=4}
Level:loadLoop{id=1, path="loops/Deep Pad 01.wav", bpm=120, n_beats=8}

Level:startLoop{id=0}
Level:startLoop{id=1}
Level:startBeatServer{bpm=120} -- this should always be 120 for the loops to be in the right key

Level:waitForBeat()
Level:waitForBeat()
Level:waitForBeat()

while true do
    Level:createAIEnemy("Seeker", AI.spiral).on_death_sound = 2
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral).on_death_sound = 2
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral).on_death_sound = 2
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral).on_death_sound = 2
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral).on_death_sound = 2
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral).on_death_sound = 2
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral).on_death_sound = 2
    Level:sleep(.5)

    Level:sleep(5)

    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 2
    Level:sleep(.6)
    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 2
    Level:sleep(.6)
    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 2
    Level:sleep(.6)
    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 2
    Level:sleep(.6)
    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 2
    Level:sleep(.6)
    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 2
    Level:sleep(.6)
    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 2
    Level:sleep(.6)

    Level:sleep(5)
end
