--
-- @file    premake5.lua
--

-- Format Output Directories
outputdir = "%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}"

-- Include any and all common external dependencies
external_includes = {
    "./external/**/include"
}

-- Include any and all link libraries
external_links = {
    
}

-- Option to build static rather than shared.
newoption {
    trigger = "build-static",
    description = "Build the TM32's library projects as static libraries"
}

-- TM32 Workspace
workspace "TM32"

    -- Language and Runtime Settings
    language        "C"
    cdialect        "C23"
    architecture    "x64"
    staticruntime   "On"

    -- Build Configurations
    configurations {
        "debug",
        "release",
        "distribute"
    }
    filter { "configurations:debug" }
        defines {
            "TM32_DEBUG", "DEBUG"
        }
        symbols "On"
    filter { "configurations:release" }
        defines {
            "TM32_RELEASE", "NDEBUG"
        }
        optimize "On"
    filter { "configurations:distribute" }
        defines {
            "TM32_DISTRIBUTE", "NDEBUG"
        }
        optimize "Full"
        symbols "Off"
    filter { "system:linux" }
        cdialect    "gnu23"
        defines {
            "TM32_LINUX"
        }
    filter {}

    -- Project: `tm32cpu` - TM32 CPU Backend Library
    project "tm32cpu"
        kind        "SharedLib"
        location    "./build/tm32cpu"
        targetdir   "./build/%{outputdir}/bin"
        objdir      "./build/%{outputdir}/obj/tm32cpu"
        files       { "./projects/tm32cpu/**.h", "./projects/tm32cpu/**.c" }
        includedirs { 
            "./projects", 
            "./projects/tm32cpu/include",
            table.unpack(external_includes) 
        }
        
        filter { "system:windows" }
            systemversion   "latest"
            defines         { "TM32CPU_WINDOWS" }
        filter { "system:linux" }
            pic             "On"
            defines         { "TM32CPU_LINUX" }
        filter { "options:build-static" }
            defines         { "TM32CPU_BUILD_STATIC" }
            kind            "StaticLib"
        filter { "not options:build-static" }
            defines         { "TM32CPU_EXPORTING" }
        filter {}

    -- Project: `tm32core` - TM32 Core System Backend Library
    project "tm32core"
        kind        "SharedLib"
        location    "./build/tm32core"
        targetdir   "./build/%{outputdir}/bin"
        objdir      "./build/%{outputdir}/obj/tm32core"
        files       { "./projects/tm32core/**.h", "./projects/tm32core/**.c" }
        includedirs { 
            "./projects", 
            "./projects/tm32cpu/include",
            "./projects/tm32core/include",
            table.unpack(external_includes) 
        }
        links {
            "tm32cpu"
        }
        filter { "system:windows" }
            systemversion   "latest"
            defines         { "TM32CORE_WINDOWS" }
        filter { "system:linux" }
            pic             "On"
            defines         { "TM32CORE_LINUX" }
        filter { "options:build-static" }
            defines         { "TM32CORE_BUILD_STATIC" }
            kind            "StaticLib"
        filter { "not options:build-static" }
            defines         { "TM32CORE_EXPORTING" }
        filter {}

    -- Project: `tm32emu` - TM32 Core Emulator Frontend Application
    project "tm32emu"
        kind        "ConsoleApp"
        location    "./build/tm32emu"
        targetdir   "./build/%{outputdir}/bin"
        objdir      "./build/%{outputdir}/obj/tm32emu"
        files       { "./projects/tm32emu/**.h", "./projects/tm32emu/**.c" }
        includedirs { 
            "./projects", 
            "./projects/tm32cpu/include",
            "./projects/tm32core/include",
            "./projects/tm32emu/include",
            table.unpack(external_includes) 
        }
        links {
            "tm32cpu",
            "tm32core"
        }

    -- Project `tm32asm` - TM32 CPU Assembler Tool
    project "tm32asm"
        kind        "ConsoleApp"
        location    "./build/tm32asm"
        targetdir   "./build/%{outputdir}/bin"
        objdir      "./build/%{outputdir}/obj/tm32asm"
        files       { "./projects/tm32asm/include/**.h", "./projects/tm32asm/src/**.c" }
        includedirs { 
            "./projects", 
            "./projects/tm32cpu/include",
            "./projects/tm32asm/include",
            table.unpack(external_includes) 
        }
        links {
            "tm32cpu"
        }
        
    -- Project: `tm32link` - TM32 CPU Linker Tool
    project "tm32link"
        kind        "ConsoleApp"
        location    "./build/tm32link"
        targetdir   "./build/%{outputdir}/bin"
        objdir      "./build/%{outputdir}/obj/tm32link"
        files       { "./projects/tm32link/include/**.h", "./projects/tm32link/src/**.c" }
        includedirs { 
            "./projects", 
            "./projects/tm32cpu/include",
            "./projects/tm32link/include",
            table.unpack(external_includes) 
        }
        links {
            "tm32cpu"
        }

    -- Project: `tm32test` - Unit and Integeration Test Suite for `tm32` libraries.
    project "tm32test"
        kind        "ConsoleApp"
        location    "./build/tm32test"
        targetdir   "./build/%{outputdir}/bin"
        objdir      "./build/%{outputdir}/obj/tm32test"
        files       { "./projects/tm32test/**.h", "./projects/tm32test/**.c" }
        includedirs { 
            "./projects", 
            "./projects/tm32cpu/include",
            "./projects/tm32core/include",
            "./projects/tm32test/include",
            table.unpack(external_includes) 
        }
        links {
            "tm32cpu"
        }
