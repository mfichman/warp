-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- NOTE: my_function{val="blah"} is a shortcut for my_function({val="blah"})


-- seeker barrage! (probably best outside)

Level:loadSFX{id=0, path="sfx/dance_hihat_02.wav"} -- on fire
Level:loadSFX{id=1, path="sfx/dance_clap_01.wav"} -- on hit

Level:loadSFX{id=2, path="sfx/hiphop_flash_wah.wav"} -- on banshee explosion sound

Level:loadLoop{id=0, path="loops/Synthesizer Bass 05 120_C_8.wav", bpm=120, n_beats=8}
Level:loadLoop{id=1, path="beats/Club Dance Beat 036 130_16.wav", bpm=130, n_beats=16}
Level:loadLoop{id=2, path="beats/Electronic Drum Beat 03 130_32.wav", bpm=130, n_beats=16}

Level:startLoop{id=0}
Level:startLoop{id=1}
Level:startBeatServer{bpm=120} -- this should always be 120 for the loops to be in the right key

Level:waitForBeat()
Level:waitForBeat()
Level:waitForBeat()
Level:startLoop{id=2}

while true do
    Level:createAIEnemy("Seeker", AI.flyFromBehind).on_death_sound = 2
    Level:createAIEnemy("Seeker", AI.flyFromBehind).on_death_sound = 2
    Level:createAIEnemy("Seeker", AI.flyFromBehind).on_death_sound = 2
    Level:createAIEnemy("Seeker", AI.flyFromBehind).on_death_sound = 2
    Level:createAIEnemy("Seeker", AI.flyFromBehind).on_death_sound = 2
    Level:createAIEnemy("Seeker", AI.flyFromBehind).on_death_sound = 2
    Level:waitForBeatSet()
end
