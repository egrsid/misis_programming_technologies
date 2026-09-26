const std = @import("std");

// Zig 0.16.
pub fn build(b: *std.Build) void {
    const exe = b.addExecutable(.{
        .name = "solution",
        .root_module = b.createModule(.{
            .root_source_file = b.path("src/main.zig"),
            .target = b.graph.host,
        }),
    });
    b.installArtifact(exe);
    const run = b.addRunArtifact(exe);
    b.step("run", "run solution").dependOn(&run.step); // `zig build run`
}
