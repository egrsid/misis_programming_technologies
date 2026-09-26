const std = @import("std");

// "key": "..."  — терпимо к пробелам после ':'.
fn fieldStr(line: []const u8, key: []const u8, out: []u8) []const u8 {
    var kb: [64]u8 = undefined;
    const k = std.fmt.bufPrint(&kb, "\"{s}\"", .{key}) catch return "";
    const kpos = std.mem.indexOf(u8, line, k) orelse return "";
    const after = line[kpos + k.len ..];
    const colon = std.mem.indexOfScalar(u8, after, ':') orelse return "";
    const rest0 = after[colon + 1 ..];
    const q1 = std.mem.indexOfScalar(u8, rest0, '"') orelse return "";
    const rest = rest0[q1 + 1 ..];
    const q2 = std.mem.indexOfScalar(u8, rest, '"') orelse return "";
    const n = @min(q2, out.len);
    std.mem.copyForwards(u8, out[0..n], rest[0..n]);
    return out[0..n];
}

// TODO: electronics / groceries / clothing / transport / other.
fn classify(description: []const u8) []const u8 {
    _ = description;
    return "other";
}

pub fn main(init: std.process.Init) !void {
    const io = init.io;
    var in_buf: [8192]u8 = undefined;
    var stdin_reader: std.Io.File.Reader = .init(.stdin(), io, &in_buf);
    const line = stdin_reader.interface.takeDelimiterExclusive('\n') catch |err| switch (err) {
        error.EndOfStream => stdin_reader.interface.buffered(),
        else => return err,
    };
    var desc_buf: [4096]u8 = undefined;
    const cls = classify(fieldStr(line, "description", &desc_buf));
    var out_buf: [256]u8 = undefined;
    var stdout_writer: std.Io.File.Writer = .init(.stdout(), io, &out_buf);
    try stdout_writer.interface.print("\"{s}\"", .{cls});
    try stdout_writer.interface.flush();
}
