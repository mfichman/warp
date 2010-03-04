-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

local distance = 0
while (true) do
    Level:waitForDistance(distance)
    Level:createObject("Banshee")
    Level:createObject("Banshee")
    Level:createObject("Banshee")
    distance = distance + 150
end

