-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- Takes a vector now
Level:setGravity{0, 0, 0}
local distance = Level:getSpineNodeDistance()
--Level:createEnemy("Seeker")
while (true) do
    Level:createCity()
    Level:waitForDistance(distance)
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Shark")
    --Level:createEnemy("Shark")
    Level:createEnemy("SnakeCrab")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("StarBlazer")
    --Level:createEnemy("LightBike")
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Seeker")
    distance = distance + 70
    --print(Level:getPlayerPosition():length())
end
