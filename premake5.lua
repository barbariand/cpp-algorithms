---@diagnostic disable:undefined-global

require("ecc")

-- Workspace settings
workspace("Hi")
configurations("Debug", "Release")

-- Project settings
project("Hi")
toolset("clang")
kind("ConsoleApp")
language("C++")
cppdialect("C++11")

-- Inputs and outputs
targetdir("bin/%{cfg.buildcfg}")
files({ "include/**.h", "include/**.hpp", "src/**.cpp" })

-- Debug configuration
filter("configurations:Debug")
defines({ "DEBUG" })
symbols("On")

-- Release configuration
filter("configurations:Release")
defines({ "RELEASE" })
optimize("On")
