-- Criterium: CS248 Final Project
-- Copyright (c) 2010 Matt Fichman

t = 0



function printtable(t)
    for i,v in pairs(t) do
        print(i, v)
    end
end

while (true) do
t = t + 0.1
crSetLight("Light", {diffuse = {(math.sin(t)+1)/2, -(math.sin(t)+1)/2, 0.5} })
printtable(crGetNode("Bike"))
print("here")
coroutine.yield(1)

end
