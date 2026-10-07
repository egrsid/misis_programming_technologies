const std = @import("std");

const Input = struct { prices: []const i64, rules: []const u8, amounts: []const i64, thresholds: []const i64 };

fn solve(alloc: std.mem.Allocator, in: Input) ![]i64 {
    // TODO: Создайте три типа Discount с apply и общий конвейер без ветвления по типам.
    _ = in;
    return alloc.alloc(i64, 0);
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
    defer alloc.free(out);
    var out_buffer: [4096]u8 = undefined;
    var writer = std.Io.File.stdout().writerStreaming(init.io, &out_buffer);
    try std.json.Stringify.value(out, .{}, &writer.interface);
    try writer.interface.writeByte('\n');
    try writer.interface.flush();
}
