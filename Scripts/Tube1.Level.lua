-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

--Level:setCompositor("Glass")
--Level:setGravity(-10)
local distance = 0
while (true) do
    Level:createCity()
    Level:waitForDistance(distance)
    Level:createEnemy("Banshee")
    Level:createEnemy("Banshee")
    Level:createEnemy("Banshee")
    distance = distance + 150
end
