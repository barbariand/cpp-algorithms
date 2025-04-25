const std = @import("std");
const ArrayList = std.ArrayList;

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const testing_lib = get_testing_lib(b, target, optimize);
    var cpp_entries = CppEntries{
        .b = b,
        .target = target,
        .optimize = optimize,
        .entries = ArrayList(*std.Build.Step.Compile).init(b.allocator),
        .libraries = ArrayList(*std.Build.Step.Compile).init(b.allocator),
    };
    defer cpp_entries.entries.deinit();
    cpp_entries.add_library(testing_lib);
    cpp_entries.add_entry("a1", b.path("src/a1/main.cc"));
    cpp_entries.add_entry("a2", b.path("src/a2/main.cc"));
    cpp_entries.add_entry("b1", b.path("src/b1/main.cc"));
    const run_step = b.step("run", "Run the specific app (use zig build run -- <app_name> [args])");
    run_step.dependOn(b.getInstallStep());
    for (cpp_entries.entries.items) |entry_step| { // entry_step is *std.Build.Step.Compile
        const run_cmd = b.addRunArtifact(entry_step);
        if (b.args) |args| {
            run_cmd.addArgs(args);
        }
        run_step.dependOn(&run_cmd.step);
    }
}
pub fn get_testing_lib(b: *std.Build, target: std.Build.ResolvedTarget, optimize: std.builtin.OptimizeMode) *std.Build.Step.Compile {
    const testing_lib = b.addStaticLibrary(.{
        .name = "testing",
        .target = target,
        .optimize = optimize,
    });
    const lib_cflags = [_][]const u8{
        "-std=c++20",
        "-g",
    };
    const files: []const []const u8 = &[_][]const u8{ "include/testing_lib/sort.cc", "include/testing_lib/framework.cc" };
    testing_lib.addCSourceFiles(.{
        .files = files,
        .flags = &lib_cflags,
    });
    testing_lib.linkLibCpp();
    testing_lib.addIncludePath(b.path("include"));
    return testing_lib;
}
const CppEntries = struct {
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    libraries: ArrayList(*std.Build.Step.Compile),
    entries: ArrayList(*std.Build.Step.Compile),

    const cflags = [_][]const u8{
        "-pedantic-errors",
        "-Wc++11-extensions",
        "-std=c++17",
        "-g",
    };

    fn add_library(self: *@This(), library: *std.Build.Step.Compile) void {
        self.libraries.append(library) catch |err| {
            // Log the error and exit - allocation failure is critical in builds
            std.debug.print("Failed to append entry to ArrayList: {any}\n", .{err});
            std.process.exit(1);
        };
    }

    fn add_entry(self: *@This(), name: []const u8, file: std.Build.LazyPath) void {
        const entry = self.b.addExecutable(.{
            .name = name,
            .root_source_file = null,
            .target = self.target,
            .optimize = self.optimize,
        });

        entry.addCSourceFile(.{
            .file = file,
            .flags = &cflags,
        });

        entry.linkLibCpp();

        for (self.libraries.items) |library| {
            entry.linkLibrary(library);
            entry.installLibraryHeaders(library);
        }

        self.b.installArtifact(entry);

        self.entries.append(entry) catch |err| {
            // Log the error and exit - allocation failure is critical in builds
            std.debug.print("Failed to append entry '{s}' to ArrayList: {any}\n", .{ name, err });
            std.process.exit(1);
        };
    }
};
