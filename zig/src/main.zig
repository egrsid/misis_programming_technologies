const std = @import("std");

const Input = struct {
    rules: []const [2]i64,
    users: []const i64,
    times: []const i64,
};

// TODO: для каждого запроса записать 'A' (пропустить) или 'D' (отклонить).
// Требование для Zig: вся память освобождается — детектор утечек в main не трогайте.
fn solve(alloc: std.mem.Allocator, in: Input) ![]u8 {
    const out = try alloc.alloc(u8, in.users.len);
    @memset(out, 'A');
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
