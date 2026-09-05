#!/usr/bin/env python3
"""Apply a W103D patch series in a disposable kernel worktree and verify it."""
import argparse
from pathlib import Path
import subprocess
import tempfile


def run(*args, cwd=None):
    return subprocess.run(args, cwd=cwd, check=True, capture_output=True, text=True)


def same_text(left, right):
    if left.read_text(encoding='utf-8') != right.read_text(encoding='utf-8'):
        raise RuntimeError(f'Source copy differs: {left} != {right}')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('kernel', type=Path, help='Git checkout of the unpatched kernel revision')
    parser.add_argument('--series', required=True, choices=['6.12', '6.18'])
    args = parser.parse_args()
    kernel = args.kernel.resolve()
    repo = Path(__file__).resolve().parents[3]
    patches = sorted((repo / f'compile-kernel/tools/patch/linux-{args.series}.y').glob('*.patch'))
    if len(patches) != 6:
        raise RuntimeError('Expected the complete six-patch W103D series')
    revision = run('git', 'rev-parse', 'HEAD', cwd=kernel).stdout.strip()
    with tempfile.TemporaryDirectory(prefix='w103d-patchcheck-') as temporary:
        tree = Path(temporary) / 'kernel'
        run('git', 'worktree', 'add', '--detach', str(tree), revision, cwd=kernel)
        try:
            version = (tree / 'Makefile').read_text(encoding='utf-8')
            major, minor = args.series.split('.')
            if f'VERSION = {major}\n' not in version or f'PATCHLEVEL = {minor}\n' not in version:
                raise RuntimeError('Kernel checkout does not match the requested series')
            for patch in patches:
                # Windows checkouts can use CRLF; patch context must match
                # the Linux source bytes rather than checkout line endings.
                normalized = Path(temporary) / patch.name
                normalized.write_text(patch.read_text(encoding='utf-8'), encoding='utf-8', newline='\n')
                run('git', 'apply', '--check', str(normalized), cwd=tree)
                run('git', 'apply', str(normalized), cwd=tree)
                print(f'Applied {patch.name}', flush=True)
            mt76 = 'drivers/net/wireless/mediatek/mt76/mt7663s'
            for source in (repo / 'compile-kernel/tools/mt76/mt7663s').iterdir():
                if source.name == 'Makefile' or source.suffix in ('.c', '.h'):
                    same_text(source, tree / mt76 / source.name)
            for source, target in [
                ('board/w103d/linux/meson-g12a-w103d.dts', 'arch/arm64/boot/dts/amlogic/meson-g12a-w103d.dts'),
                ('compile-kernel/tools/mt76/combo/mt7663-combo.h', 'include/linux/mt7663-combo.h'),
                ('compile-kernel/tools/mt76/combo/btmtksdio-w103d.h', 'drivers/bluetooth/btmtksdio-w103d.h'),
            ]:
                same_text(repo / source, tree / target)
            run('git', 'diff', '--check', cwd=tree)
            tests = repo / 'compile-kernel/tools/mt76/tests'
            run('python3', str(tests / 'test_mmc_irq.py'), str(tree))
            print(f'PASS: Linux {args.series}, source copies, source whitespace and MMC IRQ regression; base {revision}')
        finally:
            run('git', 'worktree', 'remove', '--force', str(tree), cwd=kernel)


if __name__ == '__main__':
    try:
        main()
    except subprocess.CalledProcessError as error:
        raise SystemExit(error.stdout + error.stderr) from None
