import sys
import json

def solve(ops: str, names: list[str], tags: list[list[int]]) -> str:
    # TODO: класс Item с __slots__, __eq__ и __hash__, хранение в set.
    # Внутри __hash__ нельзя hash(frozenset(...)) и hash(tuple(...)) от тегов —
    # комбинирование хэшей тегов пишется самостоятельно. Строка-ключ запрещена.
    # Название сравнивается без учёта регистра, теги — как множество.
    return "-" * len(ops)


def main() -> None:
    obj = json.loads(sys.stdin.read())
    sys.stdout.write(json.dumps(solve(obj["ops"], obj["names"], obj["tags"])))


if __name__ == "__main__":
    main()
