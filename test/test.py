import unittest
from test_avl import TestAvl
from test_ht import TestHt

if __name__ == '__main__':
    suite = unittest.TestSuite()

    suite.addTest(unittest.makeSuite(TestAvl))
    suite.addTest(unittest.makeSuite(TestHt))

    unittest.TextTestRunner(verbosity=2).run(suite)
