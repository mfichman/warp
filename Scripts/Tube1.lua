-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

--print("Sleeping")
--wSleep(100)
--print("Awake")
wQueueStartLoop("loop1", {path="loops/Effected Beat 01.wav", bpm=113, n_beats=16})
wStartBeatServer({bpm=110});

print("Waiting")
wWaitForSpineNode(5)
print("Awake")

wQueueStartLoop("loop2", {path="loops/Effected Beat 02.wav", bpm=110, n_beats=16})
wWaitForBeatSet()
wSetLight("Light", {diffuse={0, 0, 1}})

while (true) do

wWaitForBeat()
wSetLight("Light", {diffuse={1, 0, 0}})

wWaitForBeat()
wSetLight("Light", {diffuse={0, 1, 0}})

end

