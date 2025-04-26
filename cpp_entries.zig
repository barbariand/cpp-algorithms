const CppEntries = @This();

const std = @import("std");
const ArrayList = std.ArrayList;

b: *std.Build,
target: std.Build.ResolvedTarget,
optimize: std.builtin.OptimizeMode,
libraries: ArrayList(*CppLibrary),
entries: ArrayList(*std.Build.Step.Compile),

const cflags = [_][]const u8{
    "-pedantic-errors",
    "-Wc++11-extensions",
    "-std=c++17",
    "-g",
};

const CppEntriesOptions = struct {
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
};

pub fn init(
    b: *std.Build,
    options: CppEntriesOptions,
) CppEntries {
    const libraries = ArrayList(*CppLibrary).init(b.allocator);
    const entries = ArrayList(*std.Build.Step.Compile).init(b.allocator);

    return .{
        .b = b,
        .target = options.target,
        .optimize = options.optimize,
        .libraries = libraries,
        .entries = entries,
    };
}

pub fn deinit(self: CppEntries) void {
    for (self.libraries.items) |lib| {
        self.b.allocator.destroy(lib);
    }
    self.libraries.deinit();
    self.entries.deinit();
}

pub fn add_entry(self: *CppEntries, name: []const u8, file: std.Build.LazyPath) void {
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

    entry.addIncludePath(self.b.path("zig-out/include"));
    entry.linkLibCpp();

    for (self.libraries.items) |library| {
        entry.linkLibrary(library.step);
    }

    self.b.installArtifact(entry);

    self.entries.append(entry) catch @panic("OOM");
}

pub fn install_zig_library(self: *CppEntries, name: []const u8, options: std.Build.Module.CreateOptions) *CppLibrary {
    const module = self.b.createModule(options);

    const library = self.b.addLibrary(.{
        .name = name,
        .root_module = module,
    });

    self.b.installArtifact(library);

    const wrapper = self.b.allocator.create(CppLibrary) catch @panic("OOM");
    wrapper.* = CppLibrary{
        .step = library,
        .type = .zig,
    };
    self.libraries.append(wrapper) catch @panic("OOM");
    return wrapper;
}

pub fn install_cpp_library(self: *CppEntries, name: []const u8, files: []const []const u8) *CppLibrary {
    const root_module = self.b.createModule(.{
        .target = self.target,
        .optimize = self.optimize,
        .link_libcpp = true,
        .pic = true,
    });
    for (files) |file| {
        root_module.addCSourceFile(.{
            .file = self.b.path(file),
            .flags = &cflags,
            .language = .cpp,
        });
    }
    const library = self.b.addLibrary(.{
        .name = name,
        .root_module = root_module,
    });

    self.b.installArtifact(library);

    const wrapper = self.b.allocator.create(CppLibrary) catch @panic("OOM");
    wrapper.* = CppLibrary{
        .step = library,
        .type = .cpp,
    };
    self.libraries.append(wrapper) catch @panic("OOM");
    return wrapper;
}

const default_header_extensions = [_][]const u8{ ".h", ".hpp" };

const CppLibrary = struct {
    step: *std.Build.Step.Compile,
    type: CppLibraryType,
    options: ?LibraryOptions = null,
    const LibraryOptions = struct {
        root_directory: std.Build.LazyPath,
        extensions: ?[]const []const u8 = null,
    };

    const CppLibraryType = enum {
        obj,
        cpp,
        zig,
    };

    pub fn configure(self: *CppLibrary, options: LibraryOptions) void {
        self.options = options;
    }

    pub fn install_headers(self: *CppLibrary) !void {
        switch (self.type) {
            .cpp, .obj => {
                if (self.options == null) {
                    @panic("Please run `configure` before running `install_headers`");
                }
                const options = self.options.?;
                const header_extensions = options.extensions orelse &default_header_extensions;

                self.step.installHeadersDirectory(options.root_directory, "include", .{ .include_extensions = header_extensions });
            },
            .zig => {
                _ = self.step.getEmittedH();
            },
        }
    }
};
