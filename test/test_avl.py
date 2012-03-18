import unittest
import avl

class testAvl(unittest.TestCase):
    """A test class for the avl module.
    """

    def setUp(self):
        """set up data used in the tests. setUp is called before each
        test function execution.
        """
        self.avl_tree = avl.Avl()

    def testCount(self):
        self.assertEquals(0, len(self.avl_tree))
        for i in range(99, -1, -1):
            self.avl_tree.insert(i)
        self.assertEquals(100, len(self.avl_tree))

    def testKeyInsertion(self):
        self.avl_tree.insert(42, "Forty-two")
        self.assertTrue(42 in self.avl_tree)

    def testKeyGetter(self):
        self.avl_tree.insert(42, "Forty-two")
        self.assertEquals(self.avl_tree.get(42, "What?!?"), "Forty-two")

    def testPopExisting(self):
        self.assertEquals(0, len(self.avl_tree))
        self.avl_tree.insert(42, "Forty-two")
        self.assertEquals(1, len(self.avl_tree))

        tmp = self.avl_tree.pop(42)
        self.assertEquals(tmp, "Forty-two")
        self.assertEquals(0, len(self.avl_tree))

    def testPopNonExisting(self):
        self.assertEquals(0, len(self.avl_tree))
        self.avl_tree.insert(42, "Forty-two")
        self.assertEquals(1, len(self.avl_tree))

        tmp = self.avl_tree.pop(44, "Forty-four")
        self.assertEquals(tmp, "Forty-four")
        self.assertEquals(1, len(self.avl_tree))

    def testPopItemExisting(self):
        self.assertEquals(0, len(self.avl_tree))
        self.avl_tree.insert(42, "Forty-two")
        self.assertEquals(1, len(self.avl_tree))

        tmp = self.avl_tree.popitem(42, "Forty-two")
        self.assertEquals(tmp, (42, "Forty-two"))
        self.assertEquals(0, len(self.avl_tree))

    def testPopItemWithDuplicates(self):
        self.assertEquals(0, len(self.avl_tree))
        self.avl_tree.insert(42, "Forty-two")
        self.assertEquals(1, len(self.avl_tree))
        self.avl_tree.insert(42, "Duplicate")
        self.assertEquals(2, len(self.avl_tree))

        tmp = self.avl_tree.popitem(42, "Forty-two")
        self.assertEquals(tmp, (42, "Forty-two"))
        self.assertEquals(1, len(self.avl_tree))

        self.assertTrue(42 in self.avl_tree)
        self.assertEquals(self.avl_tree[42], "Duplicate")

    def testKeyValueInsertion(self):
        self.avl_tree.insert(33, "Thirty-three")
        self.assertTrue(33 in self.avl_tree)
        self.assertEquals(self.avl_tree[33], "Thirty-three")

    def testIntegerOrdering(self):
        for i in range(99, -1, -1):
            self.avl_tree.insert(i)

        self.assertEquals(self.avl_tree.keys(), range(0, 100))

    def testLexicographicOrdering(self):
        strings = [
            "alpha", "beta", "gamma", "delta", "epsilon", "zeta",
            "eta", "theta", "iota", "kappa", "lambda", "mu", "nu",
            "xi", "omicron", "pi", "rho", "sigma", "tau", "upsilon",
            "phi", "chi", "psi", "omega" ]

        for s in strings:
            self.avl_tree.insert(s)

        last = None
        for s in self.avl_tree.keys():
            self.assertTrue(last is None or last < s)
            last = s

    def testMin(self):
        for i in range(99, -1, -1):
            self.avl_tree.insert(i)
        for i in range(100, 200):
            self.avl_tree.insert(i)

        self.assertEqual(min(self.avl_tree), (0, None))

    def testMax(self):
        for i in range(99, -1, -1):
            self.avl_tree.insert(i)
        for i in range(100, 200):
            self.avl_tree.insert(i)

        self.assertEqual(max(self.avl_tree), (199, None))

    def testForwardIterators(self):
        for i in range(0, 100):
            self.avl_tree.insert(i, str(i))

        items = iter(self.avl_tree)
        for (i, k) in zip(xrange(0, 100), items):
            self.assertEquals(i, k[0])
            self.assertEquals(str(i), k[1])

    def testBackwardIterators(self):
        for i in range(0, 100):
            self.avl_tree.insert(i, str(i))

        items = reversed(self.avl_tree)
        for (i, k) in zip(xrange(0, 100), items):
            self.assertEquals(99 - i, k[0])
            self.assertEquals(str(99 - i), k[1])

def suite():

    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(testavl))

    return suite

if __name__ == '__main__':

    unittest.main()
