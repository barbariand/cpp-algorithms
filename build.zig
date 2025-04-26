const std = @import("std");
const ArrayList = std.ArrayList;
const CppEntries = @import("./cpp_entries.zig");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    var cpp_entries = CppEntries.init(b, .{ .target = target, .optimize = optimize });
    defer cpp_entries.deinit();
    const cpp_testing = cpp_entries.install_cpp_library("testing", &.{ "include/testing_lib/complexity.cc", "include/testing_lib/sort.cc", "include/testing_lib/framework.cc" });
    const testing = cpp_entries.install_zig_library("zig_testing", .{
        .root_source_file = b.path("include/algo_test_lib/main.zig"),
        .target = target,
        .optimize = optimize,
    });
    cpp_testing.configure(.{ .root_directory = b.path("include/testing_lib/"), .extensions = &.{ ".hpp", "" } });
    cpp_testing.install_headers() catch @panic("IO error installing cpp_testing");
    testing.install_headers() catch @panic("IO error");

    cpp_entries.add_entry("a1", b.path("src/a1/main.cc"));
    cpp_entries.add_entry("a2", b.path("src/a2/main.cc"));
    cpp_entries.add_entry("b1", b.path("src/b1/main.cc"));
    cpp_entries.add_entry("b2", b.path("src/b2/main.cc"));
    const run_step = b.step("run", "Run the app");
    run_step.dependOn(b.getInstallStep());

    for (cpp_entries.entries.items) |entry| {
        const run_cmd = b.addRunArtifact(entry);
        if (b.args) |args| {
            run_cmd.addArgs(args);
        }
        run_step.dependOn(&run_cmd.step);
    }
}
