-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

--print("Sleeping")
--wSleep(100)
--print("Awake")
Game:queueStartLoop("loop1", {path="loops/Effected Beat 01.wav", bpm=113, n_beats=16})
Game:startBeatServer({bpm=110});

print("Waiting")
Game:waitForSpineNode(5)
print("Awake")

Game:queueStartLoop("loop2", {path="loops/Effected Beat 02.wav", bpm=110, n_beats=16})
Game:waitForBeatSet()
Game:setLight("Light", {diffuse={0, 0, 1}})

while (true) do

Game:waitForBeat()
Game:setLight("Light", {diffuse={1, 0, 0}})

Game:waitForBeat()
Game:setLight("Light", {diffuse={0, 1, 0}})

end

