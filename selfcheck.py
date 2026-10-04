#!/usr/bin/env python3
"""Build once, run each JSON case in a fresh process; no network dependencies."""
import argparse
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import time

LIMITS = {'cpp': 2, 'rust': 2, 'zig': 2, 'go': 3, 'python': 5}


def run(command, **kwargs):
    return subprocess.run(command, check=True, text=True, capture_output=True, **kwargs)


def version(command, pattern, expected, env):
    output = run(command, env=env).stdout.strip()
    match = re.search(pattern, output)
    if not match or tuple(map(int, match.groups())) != expected:
        raise RuntimeError(f'Expected {expected[0]}.{expected[1]}.x; got: {output}')
    return output


def build(lang, solution, tmp, env):
    src = solution / lang
    if lang == 'cpp':
        run(['cmake', '-S', str(src), '-B', str(tmp / 'cpp'), '-DCMAKE_BUILD_TYPE=Release'], env=env)
        run(['cmake', '--build', str(tmp / 'cpp'), '--parallel', '2'], env=env)
        return [str(tmp / 'cpp' / 'solution')]
    if lang == 'rust':
        rustc = env.get('RUSTC', 'rustc')
        version([rustc, '--version'], r'rustc (\d+)\.(\d+)\.', (1, 97), env)
        cargo = env.get('CARGO', 'cargo')
        run([cargo, 'build', '--offline', '--locked', '--quiet', '--target-dir', str(tmp / 'rust')], cwd=src, env=env)
        return [str(tmp / 'rust' / 'debug' / 'solution')]
    if lang == 'go':
        go = env.get('GO', 'go')
        env['GOTOOLCHAIN'] = 'local'
        version([go, 'version'], r'go(\d+)\.(\d+)\.', (1, 27), env)
        run([go, 'build', '-buildvcs=false', '-o', str(tmp / 'go-solution'), '.'], cwd=src, env=env)
        return [str(tmp / 'go-solution')]
    if lang == 'zig':
        zig = env.get('ZIG', 'zig')
        version([zig, 'version'], r'^(\d+)\.(\d+)\.\d+$', (0, 16), env)
        run([zig, 'build', '--prefix', str(tmp / 'zig'), '--cache-dir', str(tmp / 'zig-local')], cwd=src, env=env)
        return [str(tmp / 'zig' / 'bin' / 'solution')]
    python = env.get('PYTHON', sys.executable)
    v = json.loads(run([python, '-c', 'import sys,json;print(json.dumps(list(sys.version_info[:2])))'], env=env).stdout)
    if tuple(v) < (3, 12):
        raise RuntimeError(f'Python >=3.12 required; got {v}')
    return [python, str(src / 'main.py')]


def equal_json(a, b):
    # Python считает True == 1; в контракте JSON это разные типы.
    if type(a) is not type(b):
        return False
    if isinstance(a, list):
        return len(a) == len(b) and all(equal_json(x, y) for x, y in zip(a, b))
    if isinstance(a, dict):
        return a.keys() == b.keys() and all(equal_json(a[k], b[k]) for k in a)
    return a == b


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('solution', nargs='?', default=str(Path(__file__).parent))
    default_cases = 'examples.ndjson' if Path(__file__).name == 'selfcheck.py' else 'cases.ndjson'
    parser.add_argument('cases', nargs='?', default=str(Path(__file__).with_name(default_cases)))
    parser.add_argument('--lang', default=','.join(LIMITS))
    args = parser.parse_args()
    langs = args.lang.split(',')
    if any(lang not in LIMITS for lang in langs):
        parser.error('Languages: ' + ','.join(LIMITS))
    cases = [json.loads(line) for line in Path(args.cases).read_text().splitlines() if line.strip()]
    if not cases:
        parser.error('No test cases')
    failed = False
    with tempfile.TemporaryDirectory(prefix='hw3-check-') as directory:
        tmp = Path(directory)
        env = os.environ.copy()
        env.setdefault('GOCACHE', str(tmp / 'go-cache'))
        env.setdefault('ZIG_GLOBAL_CACHE_DIR', str(tmp / 'zig-global'))
        for lang in langs:
            try:
                cmd = build(lang, Path(args.solution).resolve(), tmp, env)
            except (OSError, RuntimeError, subprocess.CalledProcessError) as error:
                print(f'FAIL {lang}: {error}', file=sys.stderr)
                if isinstance(error, subprocess.CalledProcessError):
                    print(error.stdout + error.stderr, file=sys.stderr)
                failed = True
                continue
            bad, worst = [], 0.0
            for case in cases:
                started = time.perf_counter()
                try:
                    proc = subprocess.run(cmd, input=json.dumps(case['input']), text=True,
                                          capture_output=True, timeout=LIMITS[lang], env=env)
                    elapsed = time.perf_counter() - started
                    worst = max(worst, elapsed)
                    if proc.returncode:
                        bad.append(f"{case['id']}: exit {proc.returncode}: {proc.stderr[:300]}")
                        continue
                    got = json.loads(proc.stdout)
                    if not equal_json(got, case['expected']):
                        bad.append(f"{case['id']}: wrong answer; got {str(got)[:140]}")
                except subprocess.TimeoutExpired:
                    bad.append(f"{case['id']}: time limit {LIMITS[lang]} s")
                except ValueError:
                    bad.append(f"{case['id']}: invalid JSON output")
            print(f"{'FAIL' if bad else 'OK'} {lang}: {len(cases)-len(bad)}/{len(cases)}, max {worst:.3f} s")
            for message in bad[:10]:
                print('  ' + message)
            failed |= bool(bad)
    return int(failed)


if __name__ == '__main__':
    raise SystemExit(main())
