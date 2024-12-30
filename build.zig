const std = @import("std");

pub fn build(b: *std.Build) void {
    b.exe_dir = "";
    b.lib_dir = "";
    b.dest_dir = "";
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const lib = b.addSharedLibrary(.{
        .name = "brv",
        .target = target,
        .optimize = optimize,
    });
    lib.addCSourceFiles(.{
        .files = &.{
            //
            "source/parser.c",
            "source/analyzer.c",
            "source/builder.c",
        },
    });
    lib.linkLibC();
    const lib_artifact = b.addInstallArtifact(lib, .{});
    lib_artifact.dest_dir = .{ .custom = "../lib/" };
    lib_artifact.pdb_dir = lib_artifact.dest_dir;
    lib_artifact.h_dir = lib_artifact.dest_dir;
    lib_artifact.implib_dir = lib_artifact.dest_dir;
    b.getInstallStep().dependOn(&lib_artifact.step);

    // build examples
    const parsing = b.addExecutable(.{ .name = "parsing", .target = target, .optimize = optimize });
    parsing.addCSourceFile(.{ .file = b.path("examples/parsing/main.c") });
    if (target.result.os.tag == .windows) { // getopt fix
        //parsing.addCSourceFile(.{ .file = b.path("") });
    }
    parsing.linkLibC();
    parsing.linkLibrary(lib);
    const parsing_artifact = b.addInstallArtifact(parsing, .{});
    parsing_artifact.dest_dir = .{ .custom = "../bin/parsing" };
    b.getInstallStep().dependOn(&parsing_artifact.step);
}
