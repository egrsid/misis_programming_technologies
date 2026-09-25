import sys
import json


def solve(rules: list[list[int]], users: list[int], times: list[int]) -> str:
    # TODO: для каждого запроса вернуть "A" (пропустить) или "D" (отклонить).
    # Ограничение для Python: без collections.deque и без list.pop(0). Своя очередь на
    # list с индексом головы и периодическим сжатием; обоснуйте в комментарии, почему
    # это амортизированное O(1).
    return "A" * len(users)


def main() -> None:
    obj = json.loads(sys.stdin.read())
    sys.stdout.write(json.dumps(solve(obj["rules"], obj["users"], obj["times"])))


if __name__ == "__main__":
    main()
