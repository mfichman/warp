-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- NOTE: my_function{val="blah"} is a shortcut for my_function({val="blah"})
Level:loadSFX{id=0, path="sfx/dance_hihat_02.wav"} -- on fire
Level:loadSFX{id=1, path="sfx/dance_clap_01.wav"} -- on hit
Level:loadSFX{id=2, path="sfx/Tape Rewinding 02_C.wav"} -- on banshee explosion sound

Level:loadLoop{id=0, path="loops/Chordal Synth Pattern 01.wav", bpm=120, n_beats=4}
Level:loadLoop{id=1, path="beats/Club Dance Beat 023 125_16.wav", bpm=125, n_beats=16}

Level:startLoop{id=0}
Level:startBeatServer{bpm=120} -- this should always be 120 for the loops to be in the right key

Level:waitForBeat()
Level:startLoop{id=1}

Level:waitForDownbeat()
while (true) do

--Level:setLight{name="Light", diffuse={1, 1, 1.0}}
Level:waitForBeat()

--Level:setLight{name="Light", diffuse={0.0, 0.0, 0.0}}
Level:waitForBeat()

end
