-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Object = {}

-- This function initializes a new object.  Note that Lua classes 
-- are like Javascript classes: they use prototyping.  So calling
-- A = Object:new could also create a subclass of type A.
function Object:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if o.init then o:init() end
	return o
end

require "Scripts.Objects.Enemy"
