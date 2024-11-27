const std = @import("std");

pub fn build(b: *std.Build) void {
    b.lib_dir = "lib/";
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const lib = b.addStaticLibrary(.{
        .name = "brv",
        .target = target,
        .optimize = optimize,
    });
    lib.addCSourceFiles(.{
        .files = &.{
            //
            "source/parser.c",
            "source/analyzer.c",
        },
    });
    lib.linkLibC();
    b.installArtifact(lib);

    const testexe = b.addExecutable(.{
        .name = "testexe",
        .target = target,
        .optimize = optimize,
    });
    testexe.linkLibrary(lib);
    testexe.addCSourceFiles(.{
        .files = &.{
            //
            "tests/main.c",
        },
    });
    testexe.linkLibC();
    b.installArtifact(testexe);
}
