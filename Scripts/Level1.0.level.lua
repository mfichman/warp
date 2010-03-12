-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- Takes a vector now
Level:setGravity{0, 0, 0}
local distance = Level:getSpineNodeDistance()

distance = distance + 60
Level:waitForDistance(distance)

for i=1,5 do
    Level:createAIEnemy("SnakeCrab", AI.outsideCrawl)
    distance = distance + 50
    Level:waitForDistance(distance)
end

while (true) do
    Level:waitForDistance(distance)

    for i=1,2 do

        Level:createAIEnemy("FireCycle", AI.outsideFloat)
        Level:createAIEnemy("FireCycle", AI.outsideFloat)
        Level:createAIEnemy("FireCycle", AI.outsideFloat)

        distance = distance + 200
        Level:waitForDistance(distance)
    end


    for i=1,3 do

        Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam)
        Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam)
        Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam)

        distance = distance + 200
        Level:waitForDistance(distance)
    end

    for i=1,2 do

        Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam)
        Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam)
        Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam)

        distance = distance + 200
        Level:waitForDistance(distance)

        Level:createEnemy("Shield"):left()
        distance = distance + 100
        Level:waitForDistance(distance)

        Level:createEnemy("Shield"):right()
        distance = distance + 250
        Level:waitForDistance(distance)

    end
end
