-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- Takes a vector now
Level:setGravity{0, 0, 0}
local distance = Level:getSpineNodeDistance()

distance = distance + 60
Level:waitForDistance(distance)

for i=1,4 do

Level:createAIEnemy("SnakeCrab", AI.outsideCrawl)
Level:createAIEnemy("SnakeCrab", AI.outsideCrawl)
Level:createAIEnemy("SnakeCrab", AI.outsideCrawl)

distance = distance + 200
Level:waitForDistance(distance)
end


for i=1,4 do

Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam)
Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam)

distance = distance + 200
Level:waitForDistance(distance)
end

for i=1,20 do

Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam)
Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam)

distance = distance + 200
Level:waitForDistance(distance)

Level:createEnemy("Shield"):left()
distance = distance + 100
Level:waitForDistance(distance)

Level:createEnemy("Shield"):right()
distance = distance + 400
Level:waitForDistance(distance)

end




--Level:createEnemy("Shield")
--while (true) do
    --Level:createCity()
    --Level:waitForDistance(distance)
    --Level:createAIEnemy("FireCycle", AI.flyFromBehind)
    --Level:createAIEnemy("FireCycle", AI.flyFromBehind)
    --Level:createAIEnemy("FireCycle", AI.flyFromBehind)

    --Level:createAIEnemy("Banshee", AI.flyFromBehind)
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Banshee")
    --Level:createEnemy("Shark")
    --Level:createEnemy("Shark")
    --Level:createEnemy("Banshee")
    --Level:createAIEnemy("StarBlazer", AI.flyFromBehind)
    --Level:createEnemy("LightBike")
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Seeker")
    --Level:createEnemy("Seeker")
    --distance = distance + 200
    --print(Level:getPlayerPosition():length())
--end
                                                 
