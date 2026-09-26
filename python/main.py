import sys
import json


def classify(price: float, description: str) -> str:
    # TODO: определите класс по описанию (и при желании цене).
    # Классы: electronics, groceries, clothing, transport, other.
    return "other"


def main() -> None:
    line = sys.stdin.readline()
    obj = json.loads(line)
    result = classify(obj.get("price"), obj.get("description", ""))
    # Ровно одна JSON-строка на выход, ничего лишнего.
    sys.stdout.write(json.dumps(result, ensure_ascii=False))


if __name__ == "__main__":
    main()
