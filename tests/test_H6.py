import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "host"))
import h6_cli as m


class TestEnergy(unittest.TestCase):
    def test_parse_energy_log(self):
        rows = m.analyze("Ch1 [-81.0]\nCh6 [-61.0]\nCh11 [-74.0]\n")
        self.assertEqual(len(rows), 3)
        self.assertEqual(rows[0], (1, -81.0))

    def test_busiest(self):
        rows = m.analyze("Ch1 [-81.0]\nCh6 [-61.0]\n")
        self.assertEqual(m.busiest(rows)[0], 6)

    def test_empty(self):
        self.assertIsNone(m.busiest(m.analyze("no data")))


if __name__ == "__main__":
    unittest.main()
