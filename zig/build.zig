const std = @import("std");

// Zig 0.16. По умолчанию ReleaseSafe: оптимизации + проверки границ и детектор
// утечек. Для замеров: `zig build -Doptimize=ReleaseFast`.
pub fn build(b: *std.Build) void {
    const optimize = b.option(std.builtin.OptimizeMode, "optimize", "режим сборки") orelse .ReleaseSafe;
    const exe = b.addExecutable(.{
        .name = "solution",
        .root_module = b.createModule(.{
            .root_source_file = b.path("src/main.zig"),
            .target = b.graph.host,
            .optimize = optimize,
        }),
    });
    b.installArtifact(exe);
    const run = b.addRunArtifact(exe);
    b.step("run", "run solution").dependOn(&run.step); // `zig build run`
}
