#!/usr/bin/env python3
"""Regression checks for accidentally shipping different W103D firmware."""
from pathlib import Path
import shutil
import tempfile
import unittest

from verify_image import verify


class FirmwareImageTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='w103d-firmware-test-')
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name) / 'rootfs'
        repo = Path(__file__).resolve().parents[3]
        shutil.copytree(repo / 'build-armbian/armbian-files/different-files/w103d/rootfs', self.root)
        self.n9 = self.root / 'usr/lib/firmware/mediatek/mt7663_n9_v3.bin'

    def test_shipped_pair_is_the_validated_pair(self):
        verify(self.root)

    def test_missing_n9_fails_instead_of_using_base_image_firmware(self):
        self.n9.unlink()
        with self.assertRaisesRegex(RuntimeError, 'firmware missing or unvalidated'):
            verify(self.root)

    def test_replaced_n9_fails(self):
        data = bytearray(self.n9.read_bytes())
        data[0] ^= 1
        self.n9.write_bytes(data)
        with self.assertRaisesRegex(RuntimeError, 'firmware missing or unvalidated'):
            verify(self.root)

    def test_kernel_release_without_ophub_is_rejected(self):
        with self.assertRaisesRegex(RuntimeError, 'Unsupported W103D target release'):
            verify(self.root, self.root / 'boot', '6.18.49')


if __name__ == '__main__':
    unittest.main()
