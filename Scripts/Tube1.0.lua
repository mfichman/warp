-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- Takes a vector now
Level:setGravity{0, 0, 0}
local distance = Level:getSpineNodeDistance()
Level:waitForDistance(60)

while (true) do
    Level:createCity()
    Level:waitForDistance(distance)
    Level:createAIEnemy("Banshee", AI.flyFromBehind)
    Level:createAIEnemy("Banshee", AI.flyFromBehind)
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Shark")
    --Level:createEnemy("Shark")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("StarBlazer")
    --Level:createEnemy("LightBike")
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Seeker")
    distance = distance + 150
    --print(Level:getPlayerPosition():length())
end
                                                 
