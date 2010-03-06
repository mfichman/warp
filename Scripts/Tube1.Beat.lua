-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- NOTE: my_function{val="blah"} is a shortcut for my_function({val="blah"})
Level:loadSFX{id=0, path="sfx/Tape Rewinding 02_C.wav"} -- banshee explosion sound
Level:loadSFX{id=1, path="sfx/dance_hihat_02.wav"} -- banshee explosion sound
--Level:loadSFX{id=0, path="sfx/hiphop_snare_01.wav"} -- banshee explosion sound

Level:loadLoop{id=1, path="beats/Club Dance Beat 023 125_16.wav", bpm=125, n_beats=16}
Level:loadLoop{id=0, path="loops/Dance Floor Pattern 05.wav", bpm=120, n_beats=4}

Level:startLoop{id=0}
Level:startLoop{id=1}
Level:startBeatServer{bpm=120} -- this should always be 120 for the loops to be in the right key

while (true) do

Level:waitForBeat()
Level:setLight{name="Light", diffuse={0.4, 0, 0}}
print "beat"

Level:waitForBeat()
Level:setLight{name="Light", diffuse={0, 0.4, 0}}
print "beat"

end
