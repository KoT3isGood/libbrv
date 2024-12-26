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
    b.getInstallStep().dependOn(&lib_artifact.step);

    // build examples
    const parsing = b.addExecutable(.{ .name = "parsing", .target = target, .optimize = optimize });
    parsing.addCSourceFile(.{ .file = b.path("examples/parsing/main.c") });
    if (target.result.os.tag == .windows) { // getopt fix
        parsing.addCSourceFile(.{ .file = b.path("") });
    }
    parsing.linkLibC();
    parsing.linkLibrary(lib);
    const parsing_artifact = b.addInstallArtifact(parsing, .{});
    parsing_artifact.dest_dir = .{ .custom = "../bin/parsing" };
    b.getInstallStep().dependOn(&parsing_artifact.step);

    const objtools = b.addExecutable(.{ .name = "objtools", .target = target, .optimize = optimize });
    objtools.addCSourceFile(.{ .file = b.path("examples/objtools/main.c") });
    if (target.result.os.tag == .windows) { // getopt fix
        objtools.addCSourceFile(.{ .file = b.path("") });
    }
    objtools.linkLibC();
    objtools.linkLibrary(lib);
    const objtools_artifact = b.addInstallArtifact(objtools, .{});
    objtools_artifact.dest_dir = .{ .custom = "../bin/objtools" };
    b.getInstallStep().dependOn(&objtools_artifact.step);
}
