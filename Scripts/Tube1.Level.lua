-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Level:setGravity(0)
local distance = 0
--Level:createEnemy("Seeker")
while (true) do
    Level:createCity()
    Level:waitForDistance(distance)
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("StarBlazer")
    --Level:createEnemy("LightBike")
    Level:createEnemy("Seeker")
    Level:createEnemy("Seeker")
    Level:createEnemy("Seeker")
    Level:createEnemy("Seeker")
    distance = distance + 70
    --print(Level:getPlayerPosition():length())
end
