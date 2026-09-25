const std = @import("std");

// Zig 0.14. ReleaseSafe: оптимизации + проверки границ и детектор утечек GPA.
pub fn build(b: *std.Build) void {
    const exe = b.addExecutable(.{
        .name = "solution",
        .root_source_file = b.path("src/main.zig"),
        .target = b.graph.host,
        .optimize = .ReleaseSafe,
    });
    b.installArtifact(exe);
    const run = b.addRunArtifact(exe);
    b.step("run", "run solution").dependOn(&run.step); // `zig build run`
}
