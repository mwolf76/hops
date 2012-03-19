import unittest
from hops import array

class TestArray(unittest.TestCase):
    """A test class for the array module.
    """
    def setUp(self):
        """set up data used in the tests. setUp is called before each
        test function execution.
        """
        self.array = array.Array()

    def testCount(self):
        self.assertEquals(0, len(self.array))
        for i in range(99, -1, -1):
            self.array[i] = i
        self.assertEquals(100, len(self.array))

    # def testClear(self):
    #     self.assertEquals(0, len(self.array))
    #     for i in range(99, -1, -1):
    #         self.array.insert(0, i)
    #     self.array.clear()
    #     self.assertEquals(0, len(self.array))

    # def testKeyInsertion(self):
    #     self.array.insert(42)
    #     self.assertTrue(42 in self.array)

    # def testKeyGetter(self):
    #     self.array.insert(42)
    #     self.assertEquals(self.array.get(42, "What?!?"), "Forty-two")

    # def testPopExisting(self):
    #     self.assertEquals(0, len(self.array))
    #     self.array.insert(42, "Forty-two")
    #     self.assertEquals(1, len(self.array))

    #     tmp = self.array.pop(42)
    #     self.assertEquals(tmp, "Forty-two")
    #     self.assertEquals(0, len(self.array))

    # def testPopNonExisting(self):
    #     self.assertEquals(0, len(self.array))
    #     self.array.insert(42, "Forty-two")
    #     self.assertEquals(1, len(self.array))

    #     tmp = self.array.pop(44, "Forty-four")
    #     self.assertEquals(tmp, "Forty-four")
    #     self.assertEquals(1, len(self.array))

    # def testPopItemExisting(self):
    #     self.assertEquals(0, len(self.array))
    #     self.array.insert(42, "Forty-two")
    #     self.assertEquals(1, len(self.array))

    #     tmp = self.array.popitem(42, "Forty-two")
    #     self.assertEquals(tmp, (42, "Forty-two"))
    #     self.assertEquals(0, len(self.array))

    # def testPopItemWithDuplicates(self):
    #     self.assertEquals(0, len(self.array))
    #     self.array.insert(42, "Forty-two")
    #     self.assertEquals(1, len(self.array))
    #     self.array.insert(42, "Duplicate")
    #     self.assertEquals(2, len(self.array))

    #     tmp = self.array.popitem(42, "Forty-two")
    #     self.assertEquals(tmp, (42, "Forty-two"))
    #     self.assertEquals(1, len(self.array))

    #     self.assertTrue(42 in self.array)
    #     self.assertEquals(self.array[42], "Duplicate")

    # def testKeyValueInsertion(self):
    #     self.array.insert(33, "Thirty-three")
    #     self.assertTrue(33 in self.array)
    #     self.assertEquals(self.array[33], "Thirty-three")

    # def testForwardIterators(self):
    #     for i in range(0, 100):
    #         self.array.insert(i, str(i))

    #     items = iter(self.array)
    #     count = 0
    #     for (i, j) in items:
    #         self.assertEquals(str(i), j)
    #         count += 1
    #     self.assertEquals(count, 100)
