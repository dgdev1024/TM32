--
-- @file    premake5.lua
--

-- Format Output Directories
local outputdir = "%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}"

-- Include any and all common external dependencies
local external_includes = {
    "./external/**/include"
}

-- Include any and all link libraries
local external_links = {
    
}

-- TM32 Workspace
workspace "TM32"

    -- Language and Runtime Settings
    language        "C++"
    cppdialect      "C++23"
    architecture    "x64"
    staticruntime   "On"

    -- Build Configurations
    configurations {
        "Debug",
        "Release",
        "Distribute"
    }
    filter { "configurations:Debug" }
        defines {
            "TM32_DEBUG"
        }
        symbols "On"
    filter { "configurations:Release" }
        defines {
            "TM32_RELEASE"
        }
        optimize "On"
    filter { "configurations:Distribute" }
        defines {
            "TM32_DISTRIBUTE"
        }
        optimize "Full"
        symbols "Off"
    filter { "system:linux" }
        cppdialect  "gnu++23"
        cdialect    "gnu17"
        defines {
            "TM32_LINUX"
        }
    filter {}

    -- Project: `tm32` - TM32 CPU Library
    project "tm32"
        kind        "StaticLib"
        location    "./build/%{outputdir}/tm32"
        targetdir   "./build/%{outputdir}/bin"
        objdir      "./build/%{outputdir}/obj/tm32"
        files       { "./projects/tm32/**.hpp", "./projects/tm32/**.cpp" }
        includedirs { 
            "./projects", 
            table.unpack(external_includes) 
        }
        
        filter { "system:windows" }
            systemversion   "latest"
            defines         { "TM32_WINDOWS" }
        filter { "system:linux" }
            pic             "On"
            defines         { "TM32_LINUX" }
        filter {}

    -- Project: `tm32asm` - TM32 CPU Assembler
    project "tm32asm"
        kind        "ConsoleApp"
        location    "./build/%{outputdir}/tm32asm"
        targetdir   "./build/%{outputdir}/bin"
        objdir      "./build/%{outputdir}/obj/tm32asm"
        files       { "./projects/tm32asm/**.hpp", "./projects/tm32asm/**.cpp" }
        includedirs { 
            "./projects", 
            table.unpack(external_includes) 
        }
        
    -- Project: `tomboy` - TM32 Virtual Machine
    project "tomboy"
        kind        "ConsoleApp"
        location    "./build/%{outputdir}/tomboy"
        targetdir   "./build/%{outputdir}/bin"
        objdir      "./build/%{outputdir}/obj/tomboy"
        files       { "./projects/tomboy/**.hpp", "./projects/tomboy/**.cpp" }
        includedirs { 
            "./projects", 
            table.unpack(external_includes) 
        }
        links {
            "tm32",
            table.unpack(external_links)
        }
