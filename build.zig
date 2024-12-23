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
            "source/brv.c",
            "source/parser.c",
            "source/analyzer.c",
            "source/builder.c",
        },
    });
    lib.linkLibC();
    b.installArtifact(lib);
}
