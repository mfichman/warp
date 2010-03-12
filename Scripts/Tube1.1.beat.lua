

local distance = Level:getSpineNodeDistance()

while (true) do

    Level:createCity()
    Level:waitForDistance(distance)
    distance = distance + 120

end
