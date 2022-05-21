#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "compressor.h"
/* Pointer to the BIT_ARRAY used by the tests. */
static BIT_ARRAY* code = NULL;
static WaveletNode* root = NULL;
struct fs_state *PAPFS_data;
/* The suite initialization function.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void)
{
   // creates bit_array for tests
   code = barcreate(12);
   bit_array_from_str(code, "100110010100");
   // creates the whole Wavelet-tree for tests
   root = malloc(sizeof(WaveletNode));
   if(!root){
      return -1;
   }
   root->bitmap = barcreate(4);
   bit_array_from_str(root->bitmap, "1001");
   root->left = malloc(sizeof(WaveletNode));
   if(!root->left){
      return -1;
   }
   root->right = malloc(sizeof(WaveletNode));
   if(!root->right){
      return -1;
   }
   root->left->bitmap = barcreate(2);
   bit_array_from_str(root->left->bitmap, "10");
   root->left->left = NULL;
   root->left->right = NULL;

   root->right->bitmap = barcreate(2);
   bit_array_from_str(root->right->bitmap, "00");
   root->right->left = NULL;
   root->right->right = NULL;

   return 0;
}

void free_tree(WaveletNode* node){
   if(node->left){
      free_tree(node->left);
   }
   if(node->right){
      free_tree(node->right);
   }
   free(node);
}
/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void)
{
   bardestroy(code);
   free_tree(root);
   return 0;
}


void testBinaryRank(void)
{
   int answers[] = {1, 1, 1, 2, 3, 3, 3, 4, 4, 5, 5, 5};
   for(int i = 0 ; i <= 11 ; i++){
      CU_ASSERT(rank(1, i, code) == answers[i]);
      CU_ASSERT(rank(0, i, code) == i + 1 - answers[i]);
   }
}

void testGetHuffmanCode(void)
{
   char* string_answers[] = {"10", "01", "00", "10"};
   BIT_ARRAY* real = barcreate(2);
   for(int i = 0 ; i < 4 ; i++){
      getHuffmanCode(root, i, real, 1);
      char string_real[3] = {0, 0, 0};
      bit_array_to_str(real, string_real);
      CU_ASSERT_STRING_EQUAL(string_real, string_answers[i]);
   }
   bardestroy(real);
}

void testGetTreeRank(void)
{
   BIT_ARRAY* code = barcreate(2);
   bit_array_from_str(code, "10");
   CU_ASSERT(getTreeRank(root, 1, code, 0) == 1);
   CU_ASSERT(getTreeRank(root, 4, code, 0) == 2);
   bit_array_from_str(code, "00");
   CU_ASSERT(getTreeRank(root, 1, code, 0) == 0);
   CU_ASSERT(getTreeRank(root, 2, code, 0) == 0);
   CU_ASSERT(getTreeRank(root, 3, code, 0) == 1);
   CU_ASSERT(getTreeRank(root, 4, code, 0) == 1);
}

void testDecodeHuffmanCode(void)
{
   BIT_ARRAY* code = barcreate(2);
   bit_array_from_str(code, "10");
   CU_ASSERT(decodeHuffmanCode(code) == 'a');
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "test of rank()", testBinaryRank)) ||
       (NULL == CU_add_test(pSuite, "test of getHuffmanCode()", testGetHuffmanCode)) ||
       (NULL == CU_add_test(pSuite, "test of getTreeRank()", testGetTreeRank)) || 
       (NULL == CU_add_test(pSuite, "test of decodeHuffmanCode()", testDecodeHuffmanCode)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
