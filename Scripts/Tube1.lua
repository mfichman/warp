-- Warp: CS248 Final Project
-- Copyright (c) 2010 Matt Fichman

print("Sleeping")
wSleep(100)
print("Awake")


while (true) do

print("Waiting")
wWaitForSpineNode(10)
print("Awake")

wQueueStartLoop("loop1", "loops/Effected Beat 01.wav", {bmp=113, n_beats=16})

wWaitForBeat()
wSetLight("Light", {diffuse={1, 0, 0}})
print("Waiting")
wWaitForSpineNodeReverse(10)
print("Awake")
wSetLight("Light", {diffuse={0, 1, 0}})

end
