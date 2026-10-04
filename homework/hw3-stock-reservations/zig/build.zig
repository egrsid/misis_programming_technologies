const std = @import("std");
const builtin = @import("builtin");
comptime {
    const v = builtin.zig_version;
    if (v.major != 0 or v.minor != 16 or v.pre != null)
        @compileError("This project requires Zig 0.16.x (stable)");
}
pub fn build(b: *std.Build) void {
    const exe = b.addExecutable(.{
        .name = "solution",
        .root_module = b.createModule(.{
            .root_source_file = b.path("src/main.zig"),
            .target = b.standardTargetOptions(.{}),
            .optimize = .ReleaseSafe,
        }),
    });
    b.installArtifact(exe);
    const run = b.addRunArtifact(exe);
    b.step("run", "Run solution").dependOn(&run.step);
}
