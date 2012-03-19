import unittest

from test_avl import TestAvl
from test_ht import TestHt
from test_array import TestArray

if __name__ == '__main__':
    suite = unittest.TestSuite()

    suite.addTest(unittest.makeSuite(TestAvl))
    suite.addTest(unittest.makeSuite(TestHt))
    suite.addTest(unittest.makeSuite(TestArray))

    unittest.TextTestRunner(verbosity=2).run(suite)
