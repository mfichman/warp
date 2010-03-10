-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- Takes a vector now
Level:setGravity{0, 0, 0}
local distance = 0
--Level:createEnemy("Seeker")
while (true) do
    Level:createCity()
    Level:waitForDistance(distance)
    --Level:createEnemy("Seeker")
    Level:createEnemy("Shark")
    Level:createEnemy("Shark")
    Level:createEnemy("Shark")
    Level:createEnemy("Shark")
    Level:createEnemy("Shark")
    Level:createEnemy("Shark")
    Level:createEnemy("Shark")
    Level:createEnemy("Shark")
    --Level:createEnemy("StarBlazer")
    --Level:createEnemy("LightBike")
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Seeker")
    distance = distance + 150
    --print(Level:getPlayerPosition():length())
end
