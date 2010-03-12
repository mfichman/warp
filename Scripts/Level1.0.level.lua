-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- Takes a vector now
Level:setGravity{0, 0, 0}
local distance = Level:getSpineNodeDistance()
Level:waitForDistance(60)


while (true) do
    Level:waitForDistance(distance)
    --Level:createEnemy("Shield")
    distance = distance + 200
end
                                                 
