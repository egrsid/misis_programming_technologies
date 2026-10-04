import json
import sys


def solve(data: dict):
    # TODO: Реализуйте Warehouse и его методы; состояние меняется только внутри объекта.
    return {'results': '?' * len(data['ops']), 'snapshots': []}


if __name__ == '__main__':
    json.dump(solve(json.load(sys.stdin)), sys.stdout)
    print()
