const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const exe = b.addExecutable(.{
        .name = "chip8",
        .target = target,
        .optimize = optimize,
    });

    exe.addCSourceFiles(.{
        .files = &.{
            "src/main.cpp",
            "src/chip8.cpp",
            "src/graphics.cpp",
            "src/display.cpp",
            "src/keyboard.cpp",

            "imgui/imgui.cpp",
            "imgui/imgui_demo.cpp",
            "imgui/imgui_draw.cpp",
            "imgui/imgui_widgets.cpp",
            "imgui/imgui_tables.cpp",
            "imgui/backends/imgui_impl_sdlrenderer2.cpp",
            "imgui/backends/imgui_impl_sdl2.cpp",
        },
    });

    exe.addIncludePath(b.path("include"));
    exe.addIncludePath(b.path("imgui"));

    exe.linkSystemLibrary("SDL2");
    exe.linkLibCpp();

    b.installArtifact(exe);
}
