-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg


Level:waitForBeatSet()
Level:stopLoop{id=0}

Level:stopLoop{id=1}

Level:stopLoop{id=2}
Level:stopLoop{id=7}

Level:stopLoop{id=8}

Level:stopLoop{id=9}

Level:startLoop{id=3}

Level:startLoop{id=4}


----------------------
-- CONGO DRUMS PART --
----------------------
-- exotic sounds for later
--Level:loadSFX{id=2, path="sfx/hiphop_congo_02.wav"} -- on seeker explosion
--Level:loadSFX{id=3, path="sfx/hiphop_congo_03.wav"} -- on seeker explosion
--Level:loadSFX{id=4, path="sfx/hiphop_clicksurge.wav"} -- on seeker explosion
--Level:loadLoop{id=0, path="loops/Pluck Synth 01 120_C_4.wav", bpm=120, n_beats=4}
--Level:loadLoop{id=1, path="beats/Exotic Beat 04 108_8.wav", bpm=108, n_beats=8}



local distance = Level:getSpineNodeDistance()

while (true) do

    Level:createCity()
    Level:waitForDistance(distance)
    distance = distance + 120
end
