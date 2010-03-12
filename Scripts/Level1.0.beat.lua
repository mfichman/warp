-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- NOTE: my_function{val="blah"} is a shortcut for my_function({val="blah"})

-- snake crabs!

Level:loadSFX{id=0, path="sfx/dance_hihat_02.wav"} -- on fire
Level:loadSFX{id=1, path="sfx/dance_clap_01.wav"} -- on hit

Level:loadSFX{id=2, path="sfx/hiphop_flash_wah.wav"} -- on banshee explosion sound
Level:loadSFX{id=3, path="sfx/Tape Rewinding 02_C.wav"} -- on banshee explosion sound
Level:loadSFX{id=4, path="sfx/planet_burst.wav"} -- on banshee explosion sound
Level:loadSFX{id=5, path="sfx/hiphop_clicksurge.wav"} -- on banshee explosion sound
Level:loadSFX{id=6, path="sfx/Electricty Surge_B.wav"} -- on player hit
Level:loadSFX{id=7, path="sfx/glass_breaking_2.wav"}
Level:loadSFX{id=8, path="sfx/hiphop_congo_02.wav"}
Level:loadSFX{id=9, path="sfx/hiphop_congo_03.wav"}

Level:loadLoop{id=0, path="loops/Synthesizer Bass 05 120_C_8.wav", bpm=120, n_beats=8}
Level:loadLoop{id=1, path="beats/Club Dance Beat 036 130_16.wav", bpm=130, n_beats=16}
Level:loadLoop{id=2, path="beats/Electronic Drum Beat 03 130_32.wav", bpm=130, n_beats=16}

Level:loadLoop{id=3, path="loops/Dance Floor Pattern 26 80_C_4.wav", bpm=80, n_beats=4}
Level:loadLoop{id=4, path="loops/Deep Pad 01.wav", bpm=120, n_beats=8}
Level:loadLoop{id=5, path="loops/Pluck Synth 01 120_C_4.wav", bpm=120, n_beats=4}
Level:loadLoop{id=6, path="beats/Exotic Beat 04 108_8.wav", bpm=108, n_beats=8}

Level:loadLoop{id=7, path="loops/Disco Pickbass 05 120_C_4.wav", bpm=120, n_beats=4}
Level:loadLoop{id=8, path="loops/Synthesizer Bass 07 120_C_16.wav", bpm=120, n_beats=16}
Level:loadLoop{id=9, path="beats/Effected Drum Kit 06 95_8.wav", bpm=95, n_beats=8}


Level:startLoop{id=0}
Level:startLoop{id=1}
Level:startBeatServer{bpm=120} -- this should always be 120 for the loops to be in the right key

Level:waitForBeat()
Level:startLoop{id=2}
Level:waitForBeatSet()
Level:waitForBeatSet()
Level:stopLoop{id=0}
Level:stopLoop{id=1}

Level:startLoop{id=7}
Level:startLoop{id=8}

Level:waitForBeatSet()
Level:startLoop{id=9}

Level:waitForBeatSet()
Level:waitForBeatSet()
Level:startLoop{id=0}
Level:startLoop{id=1}
Level:stopLoop{id=2}



