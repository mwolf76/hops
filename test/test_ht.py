import unittest
import ht

class TestHt(unittest.TestCase):
    """A test class for the ht module.
    """
    def setUp(self):
        """set up data used in the tests. setUp is called before each
        test function execution.
        """
        self.ht = ht.Ht()

    def testCount(self):
        self.assertEquals(0, len(self.ht))
        for i in range(99, -1, -1):
            self.ht.insert(i)
        self.assertEquals(100, len(self.ht))

    def testClear(self):
        self.assertEquals(0, len(self.ht))
        for i in range(99, -1, -1):
            self.ht.insert(i)
        self.ht.clear()
        self.assertEquals(0, len(self.ht))

    def testKeyInsertion(self):
        self.ht.insert(42, "Forty-two")
        self.assertTrue(42 in self.ht)

    def testKeyGetter(self):
        self.ht.insert(42, "Forty-two")
        self.assertEquals(self.ht.get(42, "What?!?"), "Forty-two")

    # def testPopExisting(self):
    #     self.assertEquals(0, len(self.ht))
    #     self.ht.insert(42, "Forty-two")
    #     self.assertEquals(1, len(self.ht))

    #     tmp = self.ht.pop(42)
    #     self.assertEquals(tmp, "Forty-two")
    #     self.assertEquals(0, len(self.ht))

    # def testPopNonExisting(self):
    #     self.assertEquals(0, len(self.ht))
    #     self.ht.insert(42, "Forty-two")
    #     self.assertEquals(1, len(self.ht))

    #     tmp = self.ht.pop(44, "Forty-four")
    #     self.assertEquals(tmp, "Forty-four")
    #     self.assertEquals(1, len(self.ht))

    # def testPopItemExisting(self):
    #     self.assertEquals(0, len(self.ht))
    #     self.ht.insert(42, "Forty-two")
    #     self.assertEquals(1, len(self.ht))

    #     tmp = self.ht.popitem(42, "Forty-two")
    #     self.assertEquals(tmp, (42, "Forty-two"))
    #     self.assertEquals(0, len(self.ht))

    # def testPopItemWithDuplicates(self):
    #     self.assertEquals(0, len(self.ht))
    #     self.ht.insert(42, "Forty-two")
    #     self.assertEquals(1, len(self.ht))
    #     self.ht.insert(42, "Duplicate")
    #     self.assertEquals(2, len(self.ht))

    #     tmp = self.ht.popitem(42, "Forty-two")
    #     self.assertEquals(tmp, (42, "Forty-two"))
    #     self.assertEquals(1, len(self.ht))

    #     self.assertTrue(42 in self.ht)
    #     self.assertEquals(self.ht[42], "Duplicate")

    def testKeyValueInsertion(self):
        self.ht.insert(33, "Thirty-three")
        self.assertTrue(33 in self.ht)
        self.assertEquals(self.ht[33], "Thirty-three")

    def testForwardIterators(self):
        for i in range(0, 100):
            self.ht.insert(i, str(i))

        items = iter(self.ht)
        count = 0
        for (i, j) in items:
            self.assertEquals(str(i), j)
            count += 1
        self.assertEquals(count, 100)
