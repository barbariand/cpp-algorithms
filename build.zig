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
        .testing_lib = testing_lib,
        .entries = ArrayList(*std.Build.Step.Compile).init(b.allocator),
    };
    defer cpp_entries.entries.deinit();

    cpp_entries.add_entry(b.path("src/a1/main.cc"), "a1_main");
    cpp_entries.add_entry(b.path("src/a2/main.cc"), "a2_main");
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
        "-std=c++17",
        "-g",
    };
    testing_lib.addCSourceFile(.{
        .file = b.path("include/testing/testing_sort.cc"),
        .flags = &lib_cflags,
    });
    testing_lib.addCSourceFile(.{
        .file = b.path("include/testing/testing_framework.cc"), // Make sure this path is correct
        .flags = &lib_cflags,
    });
    testing_lib.linkLibCpp();
    testing_lib.addIncludePath(b.path("include"));
    return testing_lib;
}
// --- CppEntries Struct using ArrayList ---
const CppEntries = struct {
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    testing_lib: *std.Build.Step.Compile,
    entries: ArrayList(*std.Build.Step.Compile),

    fn add_entry(self: *@This(), file: std.Build.LazyPath, exe_name: []const u8) void {
        const entry = self.b.addExecutable(.{
            .name = exe_name,
            .target = self.target,
            .optimize = self.optimize,
        });

        const cflags = [_][]const u8{
            "-pedantic-errors",
            "-Wc++11-extensions",
            "-std=c++17",
            "-g",
        };

        entry.addCSourceFile(.{ .file = file, .flags = &cflags });
        entry.addIncludePath(self.b.path("./include"));
        entry.linkLibrary(self.testing_lib);
        entry.linkLibCpp();
        self.b.installArtifact(entry);

        self.entries.append(entry) catch |err| {
            // Log the error and exit - allocation failure is critical in builds
            std.debug.print("Failed to append entry '{s}' to ArrayList: {any}\n", .{ exe_name, err });
            std.process.exit(1);
        };
    }
};
