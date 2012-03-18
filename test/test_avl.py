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


    def testKeyInsertion(self):
        self.avl_tree.insert(42, "Forty-two")
        self.assertTrue(42 in self.avl_tree)

    def testKeyValueInsertion(self):
        self.avl_tree.insert(33, "Thirty-three")
        self.assertTrue(33in self.avl_tree)
        self.assertEquals(self.avl_tree[33], "Thirty-three")

    # def testGetFeedPostingURL(self):

    #     posting_url = "http://www.avl_tree.com/atom/9276918"

    #     self.assertEqual(self.avl_tree.get_feed_posting_url(), posting_url)



    # def testGetFeedPostingHost(self):

    #     posting_host = "www.avl_tree.com"

    #     self.assertEqual(self.avl_tree.get_feed_posting_host(), posting_host)



    # def testPostNewEntry(self):

    #     init_num_entries = self.avl_tree.get_num_entries()

    #     title = "testPostNewEntry"

    #     content = "testPostNewEntry"

    #     self.assertTrue(self.avl_tree.post_new_entry(title, content))

    #     self.assertEqual(self.avl_tree.get_num_entries(), init_num_entries+1)

    #     # Entries are ordered most-recent first

    #     # Newest entry should be first

    #     self.assertEqual(title, self.avl_tree.get_nth_entry_title(1))

    #     self.assertEqual(content, self.avl_tree.get_nth_entry_content_strip_html(1))



    # def testDeleteAllEntries(self):

    #     self.avl_tree.delete_all_entries()

    #     self.assertEqual(self.avl_tree.get_num_entries(), 0)


def suite():

    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(testavl))

    return suite

if __name__ == '__main__':

    unittest.main()

    # suiteFew = unittest.TestSuite()

    # suiteFew.addTest(testavl("testPostNewEntry"))

    # suiteFew.addTest(testavl("testDeleteAllEntries"))

    # #unittest.TextTestRunner(verbosity=2).run(suiteFew)

    # unittest.TextTestRunner(verbosity=2).run(suite())
