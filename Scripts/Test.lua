-- Warp: CS248 Final Project
-- Copyright (c) 2010 Matt Fichman

print("Sleeping")
wSleep(100)
print("Awake")


while (true) do

print("Waiting")
wWaitForSpineNode(10)
print("Awake")
wWaitForBeat()
wSetLight("Light", {diffuse={1, 0, 0}})
print("Waiting")
wWaitForSpineNodeReverse(10)
print("Awake")
wSetLight("Light", {diffuse={0, 1, 0}})

end

