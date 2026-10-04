const std = @import("std");

const Input = struct { initial: i64, ops: []const u8, ids: []const i64, counts: []const i64 };
const Output = struct { results: []u8, snapshots: [][3]i64 };

fn solve(alloc: std.mem.Allocator, in: Input) !Output {
    // TODO: Реализуйте Warehouse и его методы; состояние меняется только внутри объекта.
    const results = try alloc.alloc(u8, in.ops.len);
    errdefer alloc.free(results);
    @memset(results, '?');
    return .{ .results = results, .snapshots = try alloc.alloc([3]i64, 0) };
}

pub fn main(init: std.process.Init) !void {
    var gpa: std.heap.DebugAllocator(.{}) = .init;
    defer if (gpa.deinit() == .leak) @panic("memory leak");
    const alloc = gpa.allocator();
    var in_buffer: [4096]u8 = undefined;
    var reader = std.Io.File.stdin().readerStreaming(init.io, &in_buffer);
    const raw = try reader.interface.allocRemaining(alloc, .limited(1 << 26));
    defer alloc.free(raw);
    const parsed = try std.json.parseFromSlice(Input, alloc, raw, .{});
    defer parsed.deinit();
    const out = try solve(alloc, parsed.value);
    defer alloc.free(out.results);
    defer alloc.free(out.snapshots);
    var out_buffer: [4096]u8 = undefined;
    var writer = std.Io.File.stdout().writerStreaming(init.io, &out_buffer);
    try std.json.Stringify.value(out, .{}, &writer.interface);
    try writer.interface.writeByte('\n');
    try writer.interface.flush();
}
