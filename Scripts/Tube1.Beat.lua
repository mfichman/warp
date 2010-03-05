-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- NOTE: my_function{val="blah"} is a shortcut for my_function({val="blah"})
Level:loadLoop{id=0, path="loops/Effected Beat 01.wav", bpm=113, n_beats=16}
Level:loadLoop{id=1, path="loops/Effected Beat 02.wav", bpm=110, n_beats=16}

Level:startLoop{id=0}
Level:startBeatServer{bpm=110}

Level:waitForDistance(350)

Level:startLoop{id=1}

Level:waitForBeatSet()
Level:setLight{name="Light", diffuse={0, 0, 0.4}}

while (true) do

Level:waitForBeat()
Level:setLight{name="Light", diffuse={0.4, 0, 0}}

Level:waitForBeat()
Level:setLight{name="Light", diffuse={0, 0.4, 0}}

end
