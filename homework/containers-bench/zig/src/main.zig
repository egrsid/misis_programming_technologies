const std = @import("std");

const Input = struct {
    container: []const u8,
    workload: []const u8,
    n: u64,
    seed: u64,
};

// ---------- Генератор splitmix64 (общий для всех языков) ----------
const Rng = struct {
    state: u64,

    fn next(self: *Rng) u64 {
        self.state +%= 0x9E3779B97F4A7C15;
        var z = self.state;
        z = (z ^ (z >> 30)) *% 0xBF58476D1CE4E5B9;
        z = (z ^ (z >> 27)) *% 0x94D049BB133111EB;
        return z ^ (z >> 31);
    }

    fn rnd(self: *Rng) u64 {
        return self.next() % 1_000_000;
    }
};

// ---------- Решение ----------
const Result = struct { len: usize, checksum: u64 };

// TODO: container ∈ {array, linked, deque} → std.ArrayList / std.DoublyLinkedList
// (интрузивный: узел — поле вашей структуры, значение достаёте через
// @fieldParentPtr; узлы выделяете сами) / std.Deque; workload ∈ {push_back, push_front, queue,
// sorted_insert} — см. README. checksum = Σ (i + 1) · a[i] mod 1 000 000 007.
// Вся память освобождается — детектор утечек в main не трогайте.
fn run(alloc: std.mem.Allocator, in: Input) !Result {
    _ = alloc;
    var r = Rng{ .state = in.seed };
    _ = r.next();
    return .{ .len = @intCast(in.n), .checksum = 0 };
}

pub fn main(init: std.process.Init) !void {
    const io = init.io;
    var gpa: std.heap.DebugAllocator(.{}) = .init;
    const alloc = gpa.allocator();
    var out_buf: [256]u8 = undefined;
    var stdout_writer: std.Io.File.Writer = .init(.stdout(), io, &out_buf);
    const stdout = &stdout_writer.interface;
    {
        var in_buf: [4096]u8 = undefined;
        var stdin_reader: std.Io.File.Reader = .init(.stdin(), io, &in_buf);
        const raw = try stdin_reader.interface.allocRemaining(alloc, .limited(1 << 20));
        defer alloc.free(raw);
        const parsed = try std.json.parseFromSlice(Input, alloc, raw, .{ .ignore_unknown_fields = true });
        defer parsed.deinit();
        const in = parsed.value;
        const res = try run(alloc, in);
        try stdout.print("{{\"len\": {d}, \"checksum\": {d}}}", .{ res.len, res.checksum });
    }
    // Утечка памяти = проваленный тест: мусор после JSON ломает сравнение. Не удаляйте.
    if (gpa.deinit() == .leak) {
        try stdout.writeAll(" LEAK");
        try stdout.flush();
        std.process.exit(1);
    }
    try stdout.flush();
}
