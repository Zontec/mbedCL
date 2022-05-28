
LIB_PATH = /home/user/Projects/devs/mbedCrypto/lib/googletest/build/lib
LIB_PATH2 = /home/user/Projects/devs/mbedCrypto/lib/cmocka/build/src/
LIBS = $(LIB_PATH)/libgmock.a \
		$(LIB_PATH)/libgmock_main.a \
		$(LIB_PATH)/libgtest_main.a \
		$(LIB_PATH2)/libcmocka.a  \
		$(LIB_PATH)/libgtest.a 
		

INCLUDES = -I../../lib/googletest/googletest/include/ \
			-I../../includes \
			-I/home/user/Projects/devs/mbedCrypto/lib/cmocka/include \
			-I../../src

cmocka:
	gcc -Wall tests_main.c -fpermissive sha256_ut.c \
	 ../../src/sha256.c ../../src/sha1.c hash_test_vectors.c -L$(LIB_PATH2)\libcmocka.so.0 \
	 -DSHA256_UT -DSHA256_UT_CMOCKA  $(LIBS) $(INCLUDES) -lpthread

gtest:
	g++ -Wall tests_main.c -fpermissive sha256_ut.c \
	../../src/sha256.c ../../src/sha1.c hash_test_vectors.c -DSHA256_UT\
	 -DSHA256_UT_GTEST  $(LIBS) $(INCLUDES) -lpthread
