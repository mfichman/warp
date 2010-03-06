-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

local distance = 0
while (true) do
    Level:waitForDistance(distance)
    Level:createEnemy("Banshee")
    Level:createEnemy("Banshee")
    Level:createEnemy("Banshee")
    distance = distance + 150
end