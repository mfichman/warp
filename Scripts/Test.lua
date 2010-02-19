-- Warp: CS248 Final Project
-- Copyright (c) 2010 Matt Fichman

wQueueStartLoop("loop2", {path="loops/Effected Beat 02.wav", bpm=110, n_beats=1})
wQueueStartLoop("loop1", {path="loops/Effected Beat 01.wav", bpm=113, n_beats=16})
wStartBeatServer({bpm="220"});
while (true) do

--print("Waiting")
--wWaitForSpineNode(4)
--print("Awake")

wWaitForDownbeat()
wSetLight("Light", {diffuse={1, 0, 0}})
wWaitForDownbeat()
wSetLight("Light", {diffuse={0, 1, 0}})
end

