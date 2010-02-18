-- Warp: CS248 Final Project
-- Copyright (c) 2010 Matt Fichman

while (true) do

wQueueStartLoop("loop1", {path="loops/Effected Beat 01.wav", bpm=113, n_beats=16})

wStartBeatServer({bpm="120"});

print("Waiting")
wWaitForSpineNode(4)
print("Awake")

wQueueStartLoop("loop2", {path="loops/Effected Beat 02.wav", bpm=110, n_beats=16})

wWaitForBeat()
wSetLight("Light", {diffuse={1, 0, 0}})
print("Waiting")
wWaitForSpineNodeReverse(10)
print("Awake")
wSetLight("Light", {diffuse={0, 1, 0}})

end

