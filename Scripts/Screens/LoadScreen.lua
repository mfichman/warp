-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

LoadScreen = Object:new()


function LoadScreen:init()
    self:setSplash("TitleScreen.jpg");
end

function LoadScreen:onDestroy()
end

function LoadScreen:onKeyPressed(key)
    if (key == "S") then
        self:setLevel("Tube1")
    end
end
