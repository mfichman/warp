-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Enemy = Object:new() -- Inherit from Object

-- Subclasses will override this method.  Really, we
-- don't need an Enemy class in Lua since it uses 
-- duck typing, but it makes the code clearer and prevents
-- crashes if you forget to define onTimeStep.
function onTimeStep()

end


