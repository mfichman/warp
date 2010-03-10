-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

--Level:setGravity(-10)
local distance = 0
Level:createEnemy("Seeker")
while (true) do
    Level:createCity()
    Level:waitForDistance(distance)
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("StarBlazer")
    --Level:createEnemy("LightBike")
    distance = distance + 150
    --print(Level:getPlayerPosition():length())
end
