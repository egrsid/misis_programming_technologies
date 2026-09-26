const std = @import("std");

const Input = struct {
    ops: []const u8,
    names: []const []const u8,
    tags: []const []const u32,
};

// TODO: тип Item и свой контекст (hash + eql) для std.HashMap. Склеивать товар в
// строку-ключ нельзя. Название сравнивается без учёта регистра, теги — как множество.
// Вся память освобождается — детектор утечек в main не трогайте.
fn solve(alloc: std.mem.Allocator, in: Input) ![]u8 {
    const out = try alloc.alloc(u8, in.ops.len);
    @memset(out, '-');
    return out;
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const alloc = gpa.allocator();
    const stdout = std.io.getStdOut().writer();
    {
        const raw = try std.io.getStdIn().readToEndAlloc(alloc, 1 << 28);
        defer alloc.free(raw);
        const parsed = try std.json.parseFromSlice(Input, alloc, raw, .{ .ignore_unknown_fields = true });
        defer parsed.deinit();
        const res = try solve(alloc, parsed.value);
        defer alloc.free(res);
        try stdout.print("\"{s}\"", .{res});
    }
    // Утечка памяти = проваленный тест: мусор после JSON ломает сравнение. Не удаляйте.
    if (gpa.deinit() == .leak) {
        try stdout.writeAll(" LEAK");
        std.process.exit(1);
    }
}
