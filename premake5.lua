
-- Output directory for binaries
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Option: Build Libraries as Static instead of Shared
newoption {
    trigger     = "build-static",
    description = "Build libraries as static instead of shared"
}

-- TM32 Workspace
workspace "TM32"

    -- Language and Standards
    language "C++"
    cppdialect "C++23"

    -- Architecture Settings
    architecture "x64"
    startproject "tm32emu"

    -- Build Configurations
    configurations {
        "Debug",
        "Release",
        "Distribute"
    }

    -- Configuration-Specific Settings
    filter { "configurations:Debug" }
        defines { "TM32_DEBUG", "DEBUG" }
        symbols "On"
    filter { "configurations:Release" }
        defines { "TM32_RELEASE", "NDEBUG" }
        optimize "On"
    filter { "configurations:Distribute" }
        defines { "TM32_DISTRIBUTE", "NDEBUG" }
        optimize "Full"
    filter {}

    -- Operating-System-Specific Settings
    filter { "system:windows" }
        systemversion "latest"
        defines { "TM32_PLATFORM_WINDOWS" }
    filter { "system:linux" }
        defines { "TM32_PLATFORM_LINUX" }
        pic "On"
    filter { "system:macosx" }
        defines { "TM32_PLATFORM_MACOSX" }
        pic "On"
    filter {}

    -- Project: `tm32cpu` - TM32 CPU Backend Library
    project "tm32cpu"
        kind "SharedLib"
        location "./build/tm32cpu"
        targetdir "./build/bin/%{outputdir}"
        objdir "./build/obj/%{outputdir}/%{prj.name}"
        files {
            "./projects/TM32/CPU/**.hpp",
            "./projects/TM32/CPU/**.cpp"
        }
        includedirs {
            "./projects"
        }

        -- Static or Shared Library
        filter { "options:build-static" }
            kind "StaticLib"
            defines { "TM32CPU_BUILD_STATIC" }
        filter { "not options:build-static" }
            defines { "TM32CPU_BUILDING_SHARED" }
        filter {}

    -- Project: `tm32core` - TM32 Core Hardware Platform Library
    project "tm32core"
        kind "SharedLib"
        location "./build/tm32core"
        targetdir "./build/bin/%{outputdir}"
        objdir "./build/obj/%{outputdir}/%{prj.name}"
        files {
            "./projects/TM32/Core/**.hpp",
            "./projects/TM32/Core/**.cpp"
        }
        includedirs {
            "./projects"
        }

        -- Static or Shared Library
        filter { "options:build-static" }
            kind "StaticLib"
            defines { "TM32CORE_BUILD_STATIC" }
        filter { "not options:build-static" }
            defines { "TM32CORE_BUILDING_SHARED" }
        filter {}

    -- Project: `tm32emu` - TM32 Emulator Application
    project "tm32emu"
        kind "ConsoleApp"
        location "./build/tm32emu"
        targetdir "./build/bin/%{outputdir}"
        objdir "./build/obj/%{outputdir}/%{prj.name}"
        files {
            "./projects/TM32/Emulator/**.hpp",
            "./projects/TM32/Emulator/**.cpp"
        }
        includedirs {
            "./projects"
        }
        links {
            "tm32cpu",
            "tm32core"
        }

    -- Project: `tm32asm` - TM32 Assembler Tool
    project "tm32asm"
        kind "ConsoleApp"
        location "./build/tm32asm"
        targetdir "./build/bin/%{outputdir}"
        objdir "./build/obj/%{outputdir}/%{prj.name}"
        files {
            "./projects/TM32/Assembler/**.hpp",
            "./projects/TM32/Assembler/**.cpp"
        }
        includedirs {
            "./projects"
        }
        links {
            "tm32cpu",
            "tm32core"
        }

    -- Project: `tm32link` - TM32 Linker Tool
    project "tm32link"
        kind "ConsoleApp"
        location "./build/tm32link"
        targetdir "./build/bin/%{outputdir}"
        objdir "./build/obj/%{outputdir}/%{prj.name}"
        files {
            "./projects/TM32/Linker/**.hpp",
            "./projects/TM32/Linker/**.cpp"
        }
        includedirs {
            "./projects"
        }
        links {
            "tm32cpu",
            "tm32core"
        }

    -- Project: `tm32test` - Unit and Integration Tests for `tm32cpu` and `tm32core`
    project "tm32test"
        kind "ConsoleApp"
        location "./build/tm32test"
        targetdir "./build/bin/%{outputdir}"
        objdir "./build/obj/%{outputdir}/%{prj.name}"
        files {
            "./projects/TM32/Test/**.hpp",
            "./projects/TM32/Test/**.cpp"
        }
        includedirs {
            "./projects"
        }
        links {
            "tm32cpu",
            "tm32core"
        }
        